"""
PySideKick.Hatchet:  hack frozen PySide apps down to size
=========================================================

Hatchet is a tool for reducing the size of frozen PySide applications, by
re-building the PySide binaries to include only those classes and functions
that are actually used by the application.

In its simplest use, you give Hatchet the path to a frozen python application
and let it work its magic:

    python -m PySideKick.Hatchet /path/to/frozen/app

You might want to go for a coffee while it runs, or maybe even a pizza -- it
will take a while.  Here are the things Hatchet will do to your frozen app:

    * extract all the identifiers used throughout the application.
    * from this, calculate the set of all PySide classes and methods that the
      application might refer to.
    * download and unpack the latest PySide sources.
    * hack the PySide sources to build only those classes and methods used
      by the application.
    * configure the PySide sources with some additional tricks to reduce
      the final size of the binaries
    * build the new PySide binaries and insert them into the application.

The result can be a substantial reduction in the frozen application size.
I've seen the size drop by more than half compared to a naively-compiled
PySide binary.

For finer control over the details of the binary-hacking process, you can
use and customize the "Hatchet" class.  See its docstring for more details.

In order to successfully rebuild the PySide binary, you *must* have the
necessary build environment up and running.  This includes the Qt libraries
and development files, cmake, and the shiboken bindings generator.  If
these are installed in a non-standard location, you can set the environment
variable CMAKE_INSTALL_PREFIX to let Hatchet find them.

See the following pages for how to build PySide from source:

    http://developer.qt.nokia.com/wiki/Building_PySide_on_Linux
    http://developer.qt.nokia.com/wiki/Building_PySide_on_Windows
    http://developer.qt.nokia.com/wiki/Building_PySide_on_Mac_OS_X

If you need to customize the build process, make a subclass of Hatchet and
override the "build_pyside_source" method.

"""

import sys
import os
import imp
import re
import zipfile
import tarfile
import tempfile
import tokenize
import shutil
import modulefinder
import urlparse
import urllib
import urllib2
import hashlib
import subprocess
import logging
import inspect
from xml.dom import minidom
from collections import deque
from distutils import sysconfig
from textwrap import dedent

import PySideKick

#  Download details for the latest PySide release.
PYSIDE_SOURCE_MD5 = "5589a883cebcb799a48b184a46db6386"
PYSIDE_SOURCE_URL = "http://www.pyside.org/files/pyside-qt4.7+1.0.0.tar.bz2"


#  Name of file used to mark cached build directories
BUILD_OK_MARKER = "PySideKick.Hatchet.Built.txt"


#  Classes that must not be hacked out of the PySide binary.
#  These are used for various things internally.
KEEP_CLASSES = set((
    "QApplication",
    "QWidget",
    "QFlag",
    "QFlags",
    "QBuffer",
    "QVariant",
    "QByteArray",
    "QLayout",    #  used by glue code for QWidget. Can we remove it somehow?
    "QDeclarativeItem",  # used internally by QtDeclarative
))


#  Methods that must not be hacked off of various objects.
#  Mostly this is voodoo to stop things from segfaulting.
KEEP_METHODS = {
    "*": set(("metaObject",  # much breakage ensues if this is missing!
              "devType",     # rejecting this segfaults on by linux box
              "metric",      # without this fonts don't display correctly
         )),
    "QBitArray": set(("setBit",)),
    "QByteArray": set(("insert",)),
    "QFileDialog": set(("*",)),
}


#  These are protected methods, and win32 can't use the "protected hack"
#  to access them so we are forced to generate the bindings.
if sys.platform == "win32":
    KEEP_METHODS["QObject"] = set((
        "connectNotify",
        "disconnectNotify",
    ))


#  Simple regular expression for matching valid python identifiers.
_identifier_re = re.compile("^"+tokenize.Name+"$")
is_identifier = _identifier_re.match


class Hatchet(object):
    """Class for hacking unused code out of the PySide binaries.

    A Hatchet object controls what and how to hack things out of the PySide
    binaries for a frozen application.  It must be given a path to a directory
    containing a frozen PySide application.  When you call the hack() method
    it will:

        * extract all the identifiers used throughout the application.
        * from this, calculate the set of all PySide classes and methods
          that the application might refer to.
        * download and unpack the latest PySide sources.
        * hack the PySide sources to build only those classes and methods
          used by the application.
        * configure the PySide sources with some additional tricks to reduce
          the final size of the binaries
        * build the new PySide binaries and insert them into the application.

    You can customize the behaviour of the Hatchet by adjusting the following
    attributes:

        * mf:             a ModuleFinder object used to locate the app's code.
        * typedb:         a TypeDB instance used to get information about
                          the classes and methods available in PySide.
        * keep_classes:   a set of class names that must not be removed.
        * keep_methods:   a dict mapping class names to methods on those 
                          classes that must not be removed.

    You can adjust the modules searched for Qt identifiers by calling
    the following methods:

        * add_file:        directly add a .py or .pyc file
        * add_directory:   add the entire contents of a directory
        * add_zipfile:     add the entire contents of a zipfile

    If you don't call any of these methods, the contents of the given appdir
    will be used.
    """

    SOURCE_URL = PYSIDE_SOURCE_URL
    SOURCE_MD5 = PYSIDE_SOURCE_MD5

    def __init__(self,appdir,mf=None,typedb=None,logger=None):
        self.appdir = appdir
        if mf is None:
            mf = modulefinder.ModuleFinder()
        self.mf = mf
        if logger is None:
            logger = logging.getLogger("PySideKick.Hatchet")
        self.logger = logger
        if typedb is None:
            typedb = TypeDB(logger=self.logger)
        self.typedb = typedb
        self.keep_classes = set()
        self.keep_methods = {}

    def hack(self):
        """Hack away at the PySide binary for this frozen application.

        This method is the main entry-point for using the Hatchet class.
        It will examine the frozen application to find out what classes and
        methods it uses, then replace its PySide modules with new binaries
        hacked down to exclude useless code 
        """
        if not self.mf.modules:
            self.add_directory(self.appdir)
        self.analyse_code()
        #  Try to use a cached build if possible.
        #  We use a hash of the build parameters to identify the correct dir.
        fp = self.get_build_fingerprint()
        remove_builddir = False
        builddir = get_cache_dir("Hatchet","build",fp)
        if builddir is None:
            remove_builddir = True
            builddir = tempfile.mkdtemp()
        try:
            self.logger.debug("building PySide in %r",builddir)
            if os.path.exists(os.path.join(builddir,BUILD_OK_MARKER)):
                for nm in os.listdir(builddir):
                    if nm != BUILD_OK_MARKER:
                       sourcedir = os.path.join(builddir,nm)
                       break
                else:
                    msg = "Broken cached builddir: %s" % (builddir,)
                    raise RuntimeError(msg)
                self.logger.debug("using cached builddir: %r",sourcedir)
            else:
                sourcefile = self.fetch_pyside_source()
                sourcedir = self.unpack_tarball(sourcefile,builddir)
                self.hack_pyside_source(sourcedir)
                self.build_pyside_source(sourcedir)
                with open(os.path.join(builddir,BUILD_OK_MARKER),"wt") as f:
                    f.write(dedent("""
                    This PySide directory was built using PySideKick.Hatchet.
                    Don't use it for a regular install of PySide.
                    """))
            self.copy_hacked_pyside_modules(sourcedir,self.appdir)
        except:
            remove_builddir = True
            raise
        finally:
            if remove_builddir:
                shutil.rmtree(builddir)

    def add_script(self,pathname,follow_imports=True):
        """Add an additional script for the frozen application.

        This method adds the specified script to the internal modulefinder.
        It and all of its imports will be examined for pyside-related
        identifiers that must not be hacked out of the binary.

        To incude only the given file and not its imports, specify the
        "follow_imports" keyword argument as False.
        """
        try:
            if not follow_imports:
                self.mf.scan_code = lambda *a: None
            self.mf.run_script(pathname)
        finally:
            if not follow_imports:
                del self.mf.scan_code

    def add_file(self,pathname,pkgname="",follow_imports=True):
        """Add an additional python source file for the frozen application.

        This method adds the specified *.py or *.pyc file to the modulefinder.
        It and all of its imports will be examined for pyside-related
        identifiers that must not be hacked out of the binary.

        To incude only the given file and not its imports, specify the
        "follow_imports" keyword argument as False.
        """
        if pkgname and not pkgname.endswith("."):
            pkgname += "."
        nm = os.path.basename(pathname)
        base,ext = os.path.splitext(nm)
        if ext == ".py":
            fp = open(pathname,"rt")
            stuff = (ext, "r", imp.PY_SOURCE,)
        elif ext == ".pyc":
            fp = open(pathname,"rb")
            stuff = (ext, "r", imp.PY_COMPILED,)
        else:
            raise ValueError("unknown file type: %r" % (nm,))
        try:
            if not follow_imports:
                self.mf.scan_code = lambda *a: None
            self.mf.load_module(pkgname + base,fp,pathname,stuff)
        finally:
            if not follow_imports:
                del self.mf.scan_code
            fp.close()

    def add_zipfile(self,pathname,follow_imports=True):
        """Add an additional python zipfile for the frozen application.

        This method adds the specified zipfile to the internal modulefinder.
        All of its contained python modules, along with their imports, will
        be examined for pyside-related identifiers that must not be hacked
        out of the binary.

        To incude only the contained files and not their imports, specify the
        "follow_imports" keyword argument as False.
        """
        tdir = tempfile.mkdtemp()
        if not tdir.endswith(os.path.sep):
            tdir += os.path.sep
        try:
            zf = zipfile.ZipFile(pathname,"r")
            try:
                for nm in zf.namelist():
                    dstnm = os.path.join(tdir,nm)
                    if not dstnm.startswith(tdir):
                       continue
                    if not os.path.isdir(os.path.dirname(dstnm)):
                        os.makedirs(os.path.dirname(dstnm))
                    with open(dstnm,"wb") as f:
                        f.write(zf.read(nm))
            finally:
                zf.close()
            self.add_directory(tdir,follow_imports=follow_imports)
        finally:
            shutil.rmtree(tdir)

    def add_directory(self,pathname,fqname="",follow_imports=True):
        """Add an additional python directory for the frozen application.

        This method adds the specified directory to the internal modulefinder.
        All of its contained python files, along with their imports, will be
        examined for pyside-related identifiers that must not be hacked out
        of the binary.

        To incude only the contained files and not their imports, specify the
        "follow_imports" keyword argument as False.
        """
        if fqname and not fqname.endswith("."):
            fqname += "."
        rkwds = dict(follow_imports=follow_imports)
        for nm in os.listdir(pathname):
            subpath = os.path.join(pathname,nm)
            if os.path.isdir(subpath):
                for ininm in ("__init__.py","__init__.pyc",):
                    inipath = os.path.join(subpath,ininm)
                    if os.path.exists(inipath):
                        self.mf.load_package(fqname + nm,subpath)
                        self.add_directory(subpath,fqname+nm+".",**rkwds)
                        break
                else:
                    self.add_directory(subpath,**rkwds)
            else:
                if nm.endswith(".py") or nm.endswith(".pyc"):
                    self.add_file(subpath,fqname,**rkwds)
                elif nm.endswith(".zip"):
                    self.add_zipfile(subpath,**rkwds)
                elif nm.endswith(".exe"):
                    try:
                        self.add_zipfile(subpath,**rkwds)
                    except (zipfile.BadZipfile,):
                        pass
                else:
                    if sys.platform != "win32":
                        try:
                            if "executable" in _bt("file",subpath):
                                self.add_zipfile(subpath,**rkwds)
                        except (EnvironmentError,zipfile.BadZipfile,):
                            pass

    def analyse_code(self):
        """Analyse the code of the frozen application.

        This is the top-level method to start the code analysis process.
        It must be called after adding any extra files or directories, and
        before attempting to hack up a new version of PySide.
        """
        self.expand_kept_classes()

    def expand_kept_classes(self):
        """Find classes and methods that might be used by the application.

        This method examines the code in use by the application, and finds
        classes that it might use by matching their names against the
        identifiers present in the code.

        Any such classes found are added to the "keep_classes" attribute.
        """
        self.logger.debug("expanding kept classes")
        #  Find all python identifiers used in the application.
        #  It's a wide net, but it's easier than type inference! ;-)
        used_ids = set()
        for m in self.mf.modules.itervalues():
            if m.__code__ is not None:
                self.logger.debug("examining code: %s",m)
                self.find_identifiers_in_code(m.__code__,used_ids)
        #  Keep all classes used directly in code.
        for classnm in self.typedb.iterclasses():
            if classnm in used_ids:
                self.logger.debug("keeping class: %s [used]",classnm)
                self.keep_classes.add(classnm)
            if classnm in KEEP_CLASSES:
                self.logger.debug("keeping class: %s [pinned]",classnm)
                self.keep_classes.add(classnm)
        #  Keep all superclasses of all kept classes
        for classnm in list(self.keep_classes):
            for sclassnm in self.typedb.superclasses(classnm):
                if sclassnm not in self.keep_classes:
                    msg = "keeping class: %s [sup %s]"
                    self.logger.debug(msg,sclassnm,classnm)
                    self.keep_classes.add(sclassnm)
        #  Now iteratively expand the kept classess with possible return types
        #  of any methods called on the kept classes.
        todo_classes = deque(self.keep_classes)
        while todo_classes:
            classnm = todo_classes.popleft()
            num_done = len(self.keep_classes) - len(todo_classes)
            num_todo = len(self.keep_classes)
            self.logger.debug("expanding methods of %s (class %d of %d)",
                              classnm,num_done,num_todo)
            kept_methods = self.expand_kept_methods(classnm,used_ids)
            for methnm in self.typedb.itermethods(classnm):
                if methnm not in kept_methods:
                    continue
                self.logger.debug("expanding method %s::%s",classnm,methnm)
                for rtype in self.typedb.relatedtypes(classnm,methnm):
                    for sclassnm in self.typedb.superclasses(rtype):
                        if sclassnm not in self.keep_classes:
                            msg = "keeping class: %s [rtyp %s::%s]"
                            self.logger.debug(msg,sclassnm,classnm,methnm)
                            self.keep_classes.add(sclassnm)
                            todo_classes.append(sclassnm)

    def expand_kept_methods(self,classnm,used_ids):
        """Find all methods that must be kept for the given class.

        This method uses the given set of of used identifiers to find all
        methods on the given class that might be used by the application.
        Any such methods found are added to the "keep_methods" attribute,
        keyed by classname.

        The set of kept methods is also returned.
        """
        kept_methods = self.keep_methods.setdefault(classnm,set())
        for methnm in self.typedb.itermethods(classnm):
            if methnm in kept_methods:
                continue
            msg = "keeping method: %s::%s [%s]"
            if methnm in used_ids:
                self.logger.debug(msg,classnm,methnm,"used")
                kept_methods.add(methnm)
            elif methnm + "_" in used_ids:
                self.logger.debug(msg,classnm,methnm,"used")
                kept_methods.add(methnm)
            elif methnm == classnm:
                self.logger.debug(msg,classnm,methnm,"constructor")
                kept_methods.add(methnm)
            elif "*" in kept_methods:
                self.logger.debug(msg,classnm,methnm,"star")
                kept_methods.add(methnm)
            elif methnm in self.keep_methods.get("*",()):
                self.logger.debug(msg,classnm,methnm,"star")
                kept_methods.add(methnm)
            elif methnm in KEEP_METHODS.get(classnm,()):
                self.logger.debug(msg,classnm,methnm,"pinned")
                kept_methods.add(methnm)
            elif "*" in KEEP_METHODS.get(classnm,()):
                self.logger.debug(msg,classnm,methnm,"pinned")
                kept_methods.add(methnm)
            elif methnm in KEEP_METHODS.get("*",()):
                self.logger.debug(msg,classnm,methnm,"pinned")
                kept_methods.add(methnm)
            else:
                #  TODO: is this just superstition on my part?
                #  Shiboken doesn't like it when we reject methods
                #  that have a pure virtual override somewhere in the
                #  inheritence chain.
                for sclassnm in self.typedb.superclasses(classnm):
                    if self.typedb.ispurevirtual(sclassnm,methnm):
                        self.logger.debug(msg,classnm,methnm,"virtual")
                        kept_methods.add(methnm)
                        break
        return kept_methods

    def find_rejections(self):
        """Find classes and methods that can be rejected from PySide.

        This method uses the set of kept classes and methods to determine
        the classes and methods that can be hacked out of the PySide binary.

        It generates tuples of the form ("ClassName",) for useless classes,
        and the form ("ClassName","methodName",) for useless methods on
        otherwise useful classes.
        """
        for classnm in self.typedb.iterclasses():
            if classnm not in self.keep_classes:
                yield (classnm,)
            else:
                for methnm in self.typedb.itermethods(classnm):
                    if methnm not in self.keep_methods.get(classnm,()):
                        yield (classnm,methnm,)

    def find_identifiers_in_code(self,code,ids=None):
        """Find any possible identifiers used by the given code.

        This method performs a simplistic search for the identifiers used in
        the given code object.  It will detect attribute accesses and the use
        of getattr with a constant string, but can't do anything fancy about
        names created at runtime.  It will also find plenty of false positives.

        The set of all identifiers used in the code is returned.  If the
        argument 'ids' is not None, it is taken to be the set that is being
        built (mostly this is for easy recursive walking of code objects).
        """
        if ids is None:
            ids = set()
        for name in code.co_names:
            ids.add(name)
        for const in code.co_consts:
            if isinstance(const,basestring) and is_identifier(const):
                ids.add(const)
            elif isinstance(const,type(code)):
                self.find_identifiers_in_code(const,ids)
        return ids

    def fetch_pyside_source(self):
        """Fetch the sources for latest pyside version.

        This method fetches the sources for the latest pyside version.
        If the environment variable PYSIDEKICK_DOWNLOAD_CACHE is set then
        we first look there for a cached version.  PIP_DOWNLOAD_CACHE is
        used as a fallback location.
        """
        cachedir = get_cache_dir("Hatchet","src")
        nm = os.path.basename(urlparse.urlparse(self.SOURCE_URL).path)
        if cachedir is None:
            (fd,cachefile) = tempfile.mkstemp()
            os.close(fd)
        else:
            #  Use cached version if it has correct md5.
            cachefile = os.path.join(cachedir,nm)
            if os.path.exists(cachefile):
                if not self._check_pyside_source_md5(cachefile):
                    os.unlink(cachefile)
        #  Download if we can't use the cached version
        if cachedir is None or not os.path.exists(cachefile):
            self.logger.info("downloading %s",self.SOURCE_URL)
            fIn = urllib2.urlopen(self.SOURCE_URL)
            try:
                 with open(cachefile,"wb") as fOut:
                    shutil.copyfileobj(fIn,fOut)
            finally:
                fIn.close()
            if not self._check_pyside_source_md5(cachefile):
                msg = "corrupted download: %s" % (PYSIDE_SOURCE_URL,)
                raise RuntimeError(msg)
        return cachefile

    def _check_pyside_source_md5(self,cachefile):
        """Check the MD5 of a downloaded source file."""
        if self.SOURCE_MD5 is None:
            return True
        md5 = hashlib.md5()
        with open(cachefile,"rb") as f:
            data = f.read(1024*32)
            while data:
                md5.update(data)
                data = f.read(1024*32)
        if md5.hexdigest() != self.SOURCE_MD5:
            self.logger.critical("bad MD5 for %r",cachefile)
            self.logger.critical("    %s != %s",md5.hexdigest(),
                                                self.SOURCE_MD5)
            return False
        return True

    def unpack_tarball(self,sourcefile,destdir):
        """Unpack the given tarball into the given directory.

        This method unpacks the given tarball file into the given directory.
        It returns the path to the "root" directory of the tarball, i.e. the
        first directory that contains an actual file.  This is usually the
        directory you want for e.g. building a source distribution.
        """
        self.logger.info("unpacking %r => %r",sourcefile,destdir)
        tf = tarfile.open(sourcefile,"r:*")
        if not destdir.endswith(os.path.sep):
            destdir += os.path.sep
        try:
            for nm in tf.getnames():
                destpath = os.path.abspath(os.path.join(destdir,nm))
                #  Since we've checked the MD5 we should be safe from
                #  malicious filenames, but you can't be too careful...
                if not destpath.startswith(destdir):
                    raise RuntimeError("tarball contains malicious paths!")
            tf.extractall(destdir)
        finally:
            tf.close()
        rootdir = destdir
        names = os.listdir(rootdir)
        while len(names) == 1:
            rootdir = os.path.join(rootdir,names[0])
            names = os.listdir(rootdir)
        return rootdir
 
    def hack_pyside_source(self,sourcedir):
        """Hack useless code out of the given PySide source directory.

        This is where the fun happens!  We generate a list of classes and
        methods to reject from the build, and modify the PySide source dir
        to make it happen.  This involves two steps:

            * adding <rejection> elements to the typesystem files
            * removing <class>_wrapper.cpp entries from the makefiles

        """
        self.logger.info("hacking PySide sources in %r",sourcedir)
        logger = self.logger
        #  Find all rejections and store them for quick reference.
        reject_classes = set()
        reject_methods = {}
        num_rejected_methods = 0
        for rej in self.find_rejections():
            if len(rej) == 1:
               logger.debug("reject %s",rej[0])
               reject_classes.add(rej[0])
            else:
               logger.debug("reject %s::%s",rej[0],rej[1])
               num_rejected_methods += 1
               reject_methods.setdefault(rej[0],set()).add(rej[1])
        logger.info("keeping %d classes",len(self.keep_classes))
        logger.info("rejecting %d classes, %d methods",len(reject_classes),
                                                      num_rejected_methods)
        #  Find each top-level module directory and patch the contained files.
        psdir = os.path.join(sourcedir,"PySide")
        moddirs = []
        for modnm in os.listdir(psdir):
            if not modnm.startswith("Qt") and not modnm == "phonon":
                continue
            moddir = os.path.join(psdir,modnm)
            if os.path.isdir(moddir):
                #  Add <rejection> records for each class and method.
                #  Also strip any modifications to rejected functions.
                def adjust_typesystem_file(dom):
                    tsnode = None
                    for c in dom.childNodes:
                        if c.nodeType != c.ELEMENT_NODE:
                            continue
                        if c.tagName == "typesystem":
                            tsnode = c
                            break
                    else:
                        return dom
                    #  Adjust the existings decls to meet our needs
                    TYPE_TAGS = ("enum-type","value-type","object-type",)
                    for cn in list(tsnode.childNodes):
                        if cn.nodeType != c.ELEMENT_NODE:
                            continue
                        if cn.tagName in TYPE_TAGS:
                            #  Remove delcaration of any rejected classes.
                            clsnm = cn.getAttribute("name")
                            if clsnm in reject_classes:
                                tsnode.removeChild(cn)
                                continue
                            #  Remove any modifications for rejected methods
                            if clsnm not in reject_methods:
                                continue
                            FUNC_TAGS = ("modify-function","add-function",)
                            for mfn in list(cn.childNodes):
                                if mfn.nodeType != c.ELEMENT_NODE:
                                    continue
                                if mfn.tagName in FUNC_TAGS:
                                    sig = mfn.getAttribute("signature")
                                    fnm = sig.split("(")[0]
                                    if fnm in reject_methods[clsnm]:
                                        cn.removeChild(mfn)
                    #  Add explicit rejection records.
                    for cls in reject_classes:
                        rn = dom.createElement("rejection")
                        rn.setAttribute("class",cls)
                        tsnode.appendChild(rn)
                        nl = dom.createTextNode("\n")
                        tsnode.appendChild(nl)
                    for (cls,nms) in reject_methods.iteritems():
                        for nm in nms:
                            rn = dom.createElement("rejection")
                            rn.setAttribute("class",cls)
                            rn.setAttribute("function-name",nm)
                            tsnode.appendChild(rn)
                            rn = dom.createElement("rejection")
                            rn.setAttribute("class",cls)
                            rn.setAttribute("field-name",nm)
                            tsnode.appendChild(rn)
                            nl = dom.createTextNode("\n")
                            tsnode.appendChild(nl)
                    return dom
                for (dirnm,_,filenms) in os.walk(moddir):
                    for filenm in filenms:
                        if filenm.startswith("typesystem_") and "xml" in filenm:
                            tsfile = os.path.join(dirnm,filenm)
                            self.patch_xml_file(adjust_typesystem_file,tsfile)
                #  Remove rejected classes from the build deps list
                remaining_sources = []
                def dont_build_class(lines):
                    for ln in lines:
                        for rejcls in reject_classes:
                            if rejcls.lower()+"_" in ln:
                                if "wrapper.cpp" in ln:
                                    if "_module_wrapper.cpp" not in ln:
                                        break
                            if "_"+rejcls[1:].lower()+"_" in ln:
                                if "wrapper.cpp" in ln:
                                    if "_module_wrapper.cpp" not in ln:
                                        break
                            if rejcls in ln and "check_qt_class" in ln:
                                break
                        else:
                            if "wrapper.cpp" in ln:
                                remaining_sources.append(ln)
                            yield ln
                self.patch_file(dont_build_class,moddir,"CMakeLists.txt")
                #  If there aren't any sources left to build in that module,
                #  remove it from the main PySide build file.
                if len(remaining_sources) < 2:
                    def dont_build_module(lines):
                        for ln in lines:
                            if modnm not in ln:
                                yield ln
                    logger.debug("module empty, not building: %s",modnm)
                    self.patch_file(dont_build_module,psdir,"CMakeLists.txt")

    def patch_file(self,patchfunc,*paths):
        """Patch the given file by applying a line-filtering function.

        This method allows easy patching of a build file by applying a
        python function.

        The specified "patchfunc" must be a line filtering function - it takes
        as input the sequence of lines from the file, and outputs a modified
        sequence of lines.
        """
        filepath = os.path.join(*paths)
        self.logger.debug("patching file %r",filepath)
        mod = os.stat(filepath).st_mode
        (fd,tf) = tempfile.mkstemp()
        try:
            os.close(fd)
            with open(tf,"wt") as fOut:
                with open(filepath,"rt") as fIn:
                    for ln in patchfunc(fIn):
                        fOut.write(ln)
                fOut.flush()
            os.chmod(tf,mod)
            if sys.platform == "win32":
                os.unlink(filepath)
        except:
            os.unlink(tf)
            raise
        else:
            os.rename(tf,filepath)

    def patch_xml_file(self,patchfunc,*paths):
        """Patch the given file by applying an xml-filtering function.

        This method allows easy patching of a build file by applying a
        python function.

        The specified "patchfunc" must be an xml filtering function - it takes
        as input a DOM object and returns a modified DOM.
        """
        filepath = os.path.join(*paths)
        self.logger.debug("patching file %r",filepath)
        mod = os.stat(filepath).st_mode
        with open(filepath,"rt") as fIn:
            xml = minidom.parse(fIn)
        xml = patchfunc(xml)
        (fd,tf) = tempfile.mkstemp()
        try:
            os.close(fd)
            xmlstr = xml.toxml().encode("utf8")
            with open(tf,"wt") as fOut:
                fOut.write(xmlstr)
                fOut.flush()
            os.chmod(tf,mod)
            if sys.platform == "win32":
                os.unlink(filepath)
        except:
            os.unlink(tf)
            raise
        else:
            os.rename(tf,filepath)

    def build_pyside_source(self,sourcedir):
        """Build the PySide sources in the given directory.

        This is a simple wrapper around PySide's `cmake; make;` build process.
        For it to work, you must have the necessary tools installed on your
        system (e.g. cmake, shiboken)
        """
        self.logger.info("building PySide in %r",sourcedir)
        olddir = os.getcwd()
        os.chdir(sourcedir)
        try:
            #  Here we have some more tricks for getting smaller binaries:
            #     * CMAKE_BUILD_TYPE=MinSizeRel, to enable -Os
            #     * -fno-exceptions, to skip generation of stack-handling code
            #  We also try to use compiler options from python so that the
            #  libs will match as closely as possible.
            env = os.environ.copy()
            env = self.get_build_env(env)
            cmd = ["cmake",
                   "-DCMAKE_BUILD_TYPE=MinSizeRel",
                   "-DCMAKE_VERBOSE_MAKEFILE=ON",
                   "-DBUILD_TESTS=False",
                   "-DPYTHON_EXECUTABLE="+sys.executable,
                   "-DPYTHON_INCLUDE_DIR="+sysconfig.get_python_inc()
            ]
            if "CMAKE_INSTALL_PREFIX" in env:
                cmd.append(
                   "-DCMAKE_INSTALL_PREFIX="+env["CMAKE_INSTALL_PREFIX"]
                )
            if "ALTERNATIVE_QT_INCLUDE_DIR" in env:
                qt_include_dir = env["ALTERNATIVE_QT_INCLUDE_DIR"]
                if qt_include_dir:
                    cmd.append(
                       "-DALTERNATIVE_QT_INCLUDE_DIR=" + qt_include_dir
                    )
            elif sys.platform == "darwin":
                if os.path.exists("/Library/Frameworks/QtCore.framework"):
                    cmd.append(
                       "-DALTERNATIVE_QT_INCLUDE_DIR=/Library/Frameworks"
                    )
            subprocess.check_call(cmd,env=env)
            #  The actual build program is "nmake" on win32
            if sys.platform == "win32":
                cmd = ["nmake"]
            else:
                cmd = ["make"]
            subprocess.check_call(cmd,env=env)
        finally:
            os.chdir(olddir)

    def get_build_env(self,env=None):
        """Get environment variables for the build."""
        if env is None:
            env = {}
        def get_config_var(nm):
            val = sysconfig.get_config_var(nm)
            if val is None:
                val = os.environ.get(nm)
            return val
        cc = get_config_var("CC")
        if cc is not None:
            env.setdefault("CC",cc)
        cxx = get_config_var("CXX")
        if cxx is not None:
            env.setdefault("CXX",cxx)
        cflags = get_config_var("CFLAGS")
        if cflags is not None:
            env.setdefault("CFLAGS",cflags)
        cxxflags = env.get("CXXFLAGS",os.environ.get("CXXFLAGS",""))
        cxxflags += " " + (sysconfig.get_config_var("CFLAGS") or "")
        if sys.platform != "win32":
            cxxflags += " -fno-exceptions"
        if "linux" in sys.platform:
            cxxflags += " -Wl,--gc-sections"
        env["CXXFLAGS"] = cxxflags
        if "linux" in sys.platform:
            ldflags = env.get("LDFLAGS",os.environ.get("LDFLAGS",""))
            ldflags += " " + sysconfig.get_config_var("LDFLAGS")
            env["LDFLAGS"] = ldflags
        return env

    def get_build_fingerprint(self):
        """Get a unique fingerprint identifying all our build parameters.

        This method produces a unique fingerprint (actually an md5 hash) for
        the full set of build parameters used in this Hatchet object.  This
        includes the PySide and PySideKick version, list of rejections, and
        build flags.
        """
        fp = hashlib.md5()
        #  Include info about python, pyside and pysidekick
        fp.update(sys.version)
        fp.update(sys.platform)
        fp.update(sys.executable)
        fp.update(self.SOURCE_URL)
        fp.update(self.SOURCE_MD5)
        fp.update(PySideKick.__version__)
        try:
            fp.update(inspect.getsource(sys.modules[__name__]))
        except (NameError,KeyError,):
            pass
        #  Include info about the build environment
        for (k,v) in sorted(self.get_build_env().items()):
            fp.update(k)
            fp.update(v)
        #  Include info about the rejections used
        #  OK, I think that should cover it...
        for rej in self.find_rejections():
            fp.update(str(rej))
        return fp.hexdigest()

    def copy_hacked_pyside_modules(self,sourcedir,destdir):
        """Copy PySide modules from build dir back into the frozen app."""
        self.logger.debug("copying modules from %r => %r",sourcedir,destdir)
        def is_dll(nm):
            if nm.endswith(".so"):
                return True
            if nm.endswith(".so"):
                return True
        #  Find all the build modules we're able to copy over
        psdir = os.path.join(sourcedir,"PySide")
        modules = []
        for modnm in os.listdir(psdir):
            if modnm.startswith("Qt"):
                if modnm.endswith(".so") or modnm.endswith(".pyd"):
                    modules.append(modnm)
        #  Search for similarly-named files in the destdir and replace them
        for (dirnm,_,filenms) in os.walk(destdir):
            for filenm in filenms:
                filepath = os.path.join(dirnm,filenm)
                newfilepath = None
                #  If it's a PySide module, try to copy new version
                if "PySide" in filepath:
                    for modnm in modules:
                        if filenm.endswith(modnm):
                            newfilepath = os.path.join(psdir,modnm)
                            break
                #  If it's the pyside support lib, replace that as well
                elif filenm.startswith("libpyside"):
                    newfilepath = os.path.join(sourcedir,"libpyside",filenm)
                    if not os.path.exists(newfilepath):
                        newfilepath = None
                elif filenm.startswith("pyside") and filenm.endswith(".dll"):
                    newfilepath = os.path.join(sourcedir,"libpyside",filenm)
                    if not os.path.exists(newfilepath):
                        newfilepath = None
                #  If it's the shiboken lib, try to find that and replace it.
                #  This is necessary if it's a different version to the
                #  one bundled with the application.
                elif "shiboken." in filenm:
                    if "CMAKE_INSTALL_PREFIX" in os.environ:
                        instprf = os.environ["CMAKE_INSTALL_PREFIX"]
                        for dirnm in ("bin","lib",):
                            newfilepath = os.path.join(instprf,dirnm,filenm)
                            if os.path.exists(newfilepath):
                                break
                            newfilepath = None
                #  Copy the new lib into place, and mangle it to look
                #  like the old one (e.g. linker paths).
                if newfilepath is not None:
                    self.copy_linker_paths(filepath,newfilepath)
                    self.logger.info("copying %r => %r",newfilepath,filepath)
                    os.unlink(filepath)
                    shutil.copy2(newfilepath,filepath)
                    if "linux" in sys.platform:
                        try:
                            _do("strip",filepath)
                        except subprocess.CalledProcessError:
                            pass
                    elif sys.platform == "darwin":
                        try:
                            _do("strip","-S","-x",filepath)
                        except subprocess.CalledProcessError:
                            pass

    if sys.platform == "darwin":
        def copy_linker_paths(self,srcfile,dstfile):
            """Copy runtime linker paths from source to destination.

            On MacOSX, this uses install_name_tool to copy intallnames out
            of the sourcefile and into the destfile.
            """
            srclinks = _bt("otool","-L",srcfile).strip().split("\n")
            dstlinks = _bt("otool","-L",dstfile).strip().split("\n")
            for dstlink in dstlinks:
                if "compatibility version" not in dstlink:
                    continue
                dstlibpath = dstlink.strip().split()[0]
                dstlibname = os.path.basename(dstlibpath)
                for srclink in srclinks:
                    if "compatibility version" not in srclink:
                        continue
                    srclibpath = srclink.strip().split()[0]
                    srclibname = os.path.basename(srclibpath)
                    if srclibname == dstlibname:
                        _do("install_name_tool","-change",
                            dstlibpath,srclibpath,dstfile)
                        break
    elif sys.platform == "win32":
        def copy_linker_paths(self,srcfile,dstfile):
            """Copy runtime linker paths from source to destination.

            On win32, this does nothing.
            """
            pass
    else:
        def copy_linker_paths(self,srcfile,dstfile):
            """Copy runtime linker paths from source to destination.

            On Linux-like platforms, this uses readelf and patchelf to copy
            the rpath from sourcefile to destfile.
            """
            rpath = None
            for ln in _bt("readelf","-d",srcfile).split("\n"):
                if "RPATH" in ln and "Library rpath:" in ln:
                    rpath = ln.rsplit("[",1)[1].split("]",1)[0]
                    break
            if rpath is None:
                for ln in _bt("readelf","-d",srcfile).split("\n"):
                    if "RUNPATH" in ln and "Library runpath:" in ln:
                        rpath = ln.rsplit("[",1)[1].split("]",1)[0]
                        break
            if rpath is not None:
                _do("patchelf","--set-rpath",rpath,dstfile)


def get_cache_dir(*paths):
    """Get the directory in which we can cache downloads etc.

    This function uses the environment variable PYSIDEKICK_DOWNLOAD_CACHE,
    or failing that PIP_DOWNLOAD_CACHE, to construct a directory in which
    we can cache downloaded files and other expensive-to-generate content.

    If caching is disabled, None is returned.
    """
    cachedir = os.environ.get("PYSIDEKICK_DOWNLOAD_CACHE",None)
    if cachedir is None:
        cachedir = os.environ.get("PIP_DOWNLOAD_CACHE",None)
        if cachedir is not None:
            cachedir = os.path.join(cachedir,"PySideKick")
    if cachedir is not None:
        cachedir = os.path.join(cachedir,*paths)
        if not os.path.isdir(cachedir):
            os.makedirs(cachedir)
    return cachedir


def _do(*cmdline):
    """A simple shortcut to execute the given command."""
    subprocess.check_call(cmdline)


def _bt(*cmdline):
    """A simple shortbut to execute the command, returning stdout.

    "bt" is short for "backticks"; hopefully its use is obvious to shell
    scripters and the like.
    """
    p = subprocess.Popen(cmdline,stdout=subprocess.PIPE)
    output = p.stdout.read()
    retcode = p.wait()
    if retcode != 0:
        raise subprocess.CalledProcessError(retcode,cmdline)
    return output


class TypeDB(object):
    """PySide type database.

    A TypeDB instance encapsulates some basic information about the PySide API
    and can be used to query e.g. what classes are available or what methods
    are on a class.

    The current implementation gets this information in what might seem like
    a very silly way - it pokes around in the online API documentation.  This
    has the advantage of being very quick to code up, and not requiring any
    external dependencies.

    If PySide starts shipping with bindings for apiextractor, I'll write a
    new version of this class to use those instead.

    Besides, parsing data out of the API docs isn't as fragile as it might
    sound.  The docs are themselves generated by parsing the source code, so
    they have more than enough internal structure to support simple queries.
    """

    RE_CLASS_LINK=re.compile(r"<a href=\"(\w+).html\">([\w&;]+)</a>")
    RE_METHOD_LINK=re.compile(r"<a href=\"(\w+).html\#([\w\-\.]+)\">(\w+)</a>")

    #  These classes seem to be missing from the online docs.
    #  They are in the docs on the PySide website, but those don't seem
    #  to embed information about e.g. protected or virtual status.
    MISSING_CLASSES = {
        "QAudio": (),
        "QPyTextObject": ("QTextObjectInterface","QObject",),
        "QAbstractPageSetupDialog": ("QDialog",),
        "QTextStreamManipulator": (),
        "QFactoryInterface": (),
        "QScriptExtensionInterface": ("QFactoryInterface",),
        "QAudioEngineFactoryInterface": ("QFactoryInterface",),
        "QAudioEnginePlugin": ("QAudioEngineFactoryInterface","QObject",),
        "QAbstractAudioDeviceInfo": ("QObject",),
        "QAbstractAudioOutput": ("QObject",),
        "QAbstractAudioInput": ("QObject",),
        "QDeclarativeExtensionInterface": (),
    }

    #  These methods seem to be missing from the online docs.
    MISSING_METHODS = {
        "QAbstractItemModel": {
            "decodeData":  (("QModelIndex","QList","QDataStream",),
                            True,False),
            "encodeData":  (("QModelIndex","QList","QDataStream",),
                            True,False),
        },
        "QAbstractPageSetupDialog": {
            "printer":  (("QPrinter",),
                         True,False),
        },
        "QScriptExtensionInterface": {
            "initialize":  (("QScriptEngine",),
                            True,False),
        },
        "QAudioEngineFactoryInterface": {
            "availableDevices":  (("QAudio",),
                                  True,False),
            "createDeviceInfo":  (("QByteArray","QAudio",),
                                  True,False),
            "createInput":  (("QByteArray","QAudioFormat",),
                             True,False),
            "createOutput":  (("QByteArray","QAudioFormat",),
                             True,False),
        },
        "QAbstractAudioDeviceInfo": {
            "byteOrderList": ((),True,False),
            "channelsList": ((),True,False),
            "codecList": ((),True,False),
            "deviceName": ((),True,False),
            "frequencyList": ((),True,False),
            "isFormatSupported": (("QAudioFormat",),True,False),
            "nearestFormat": (("QAudioFormat",),True,False),
            "preferredFormat": ((),True,False),
            "sampleSizeList": ((),True,False),
            "sampleTypeList": ((),True,False),
        },
        "QAbstractAudioOutput": {
            "bufferSize": ((),True,False),
            "bytesFree": ((),True,False),
            "elapsedUSecs": ((),True,False),
            "error": ((),True,False),
            "format": ((),True,False),
            "notify": ((),True,False),
            "notifyInterval": ((),True,False),
            "periodSize": ((),True,False),
            "processedUSecs": ((),True,False),
            "reset": ((),True,False),
            "resume": ((),True,False),
            "setBufferSize": ((),True,False),
            "setNotifyInterval": ((),True,False),
            "start": (("QIODevice",),True,False),
            "state": ((),True,False),
            "stateChanged": ((),True,False),
            "stop": ((),True,False),
            "suspend": ((),True,False),
        },
        "QAbstractAudioInput": {
            "bufferSize": ((),True,False),
            "bytesready": ((),True,False),
            "elapsedUSecs": ((),True,False),
            "error": ((),True,False),
            "format": ((),True,False),
            "notify": ((),True,False),
            "notifyInterval": ((),True,False),
            "periodSize": ((),True,False),
            "processedUSecs": ((),True,False),
            "reset": ((),True,False),
            "resume": ((),True,False),
            "setBufferSize": ((),True,False),
            "setNotifyInterval": ((),True,False),
            "start": (("QIODevice",),True,False),
            "state": ((),True,False),
            "stateChanged": ((),True,False),
            "stop": ((),True,False),
            "suspend": ((),True,False),
        },
        "QDeclarativeExtensionInterface": {
            "initializeEngine": (("QDeclarativeEngine",),
                                 True,False),
            "registerTypes": ((),True,False),
        },
    }

    def __init__(self,root_url="http://doc.qt.nokia.com/4.7/",logger=None):
        if not root_url.endswith("/"):
            root_url += "/"
        self.root_url = root_url
        if logger is None:
            logger = logging.getLogger("PySideKick.Hatchet")
        self.logger = logger

    _url_cache = {}
    def _read_url(self,url):
        """Read the given URL, possibly using cached version."""
        url = urlparse.urljoin(self.root_url,url)
        try:
            return self._url_cache[url]
        except KeyError:
            pass
        cachedir = get_cache_dir("Hatchet","QtDocTypeDB")
        if cachedir is None:
            cachefile = None
            cachefile404 = None
        else:
            cachefile = os.path.join(cachedir,urllib.quote(url,""))
            cachefile404 = os.path.join(cachedir,"404_"+urllib.quote(url,""))
        if cachefile is not None:
            try:
                with open(cachefile,"rb") as f:
                    self._url_cache[url] = f.read()
                    return self._url_cache[url]
            except EnvironmentError:
                if os.path.exists(cachefile404):
                    msg = "not found: " + url
                    raise urllib2.HTTPError(url,"404",msg,{},None)
        f = None
        try:
            self.logger.info("reading Qt API: %s",url)
            f = urllib2.urlopen(url)
            if f.geturl() != url:
                msg = "not found: " + url
                raise urllib2.HTTPError(url,"404",msg,{},None)
            data = f.read()
        except urllib2.HTTPError, e:
            if "404" in str(e) and cachefile404 is not None:
                open(cachefile404,"w").close()
            raise
        finally:
            if f is not None:
                f.close()
        if cachefile is not None:
            with open(cachefile,"wb") as f:
               f.write(data)
        self._url_cache[url] = data
        return data

    def _get_linked_classes(self,data):
        """Extract all class names linked to from the given HTML data."""
        for match in self.RE_CLASS_LINK.finditer(data):
            #  Careful now, it might inherit from an instantiated template
            #  type, e.g. QList<QItemSelectionRange>.  We just yield both
            #  the template type and its argument.
            if match.group(1) in match.group(2).lower():
                if "&lt;" not in match.group(2):
                    yield match.group(2)
                else:
                    yield match.group(2).split("&lt;")[0]
                    yield match.group(2).split("&lt;")[1].split("&gt;")[0]

    def _get_linked_methods(self,data):
        """Extract all method names linked to from the given HTML data."""
        for match in self.RE_METHOD_LINK.finditer(data):
            if match.group(3) in match.group(2):
                yield match.group(3)

    def _canonical_class_names(self,classnm):
        """Get all canonical class names implied by the given identifier.

        This is a simple trick to decode common typedefs (e.g. QObjectList)
        into their respective concrete classes (e.g. QObject and QList).
        """
        if self.isclass(classnm):
            yield classnm
        else:
            if classnm == "T":
                #  This appears as a generic template type variable
                pass
            elif classnm == "RawHeader":
                yield "QPair"
                yield "QByteArray"
            elif classnm == "Event":
                yield "QPair"
                yield "QEvent"
                yield "QWidget"
            elif classnm.endswith("List"):
                #  These are usually typedefs for a QList<T>
                found_classes = False
                for cclassnm in self._canonical_class_names(classnm[:-4]):
                    found_classes = True
                    yield cclassnm
                if found_classes:
                    yield "QList"

    def iterclasses(self):
        """Iterator over all available class names."""
        for classnm in self.MISSING_CLASSES.iterkeys():
            yield classnm
        #  Everything else is conventiently listed on the "classes" page.
        classlist = self._read_url("classes.html")
        for ln in classlist.split("\n"):
            ln = ln.strip()
            if ln.startswith("<dd>"):
                for classnm in self._get_linked_classes(ln):
                    yield classnm
                    break

    def isclass(self,classnm):
        """Check whether the given name is indeed a class."""
        if classnm in self.MISSING_CLASSES:
            return True
        try:
            self._read_url(classnm.lower()+".html")
        except urllib2.HTTPError, e:
            if "404" not in str(e) and "300" not in str(e):
                raise
            return False
        else:
            return True

    def superclasses(self,classnm):
        """Get all superclasses for a given class."""
        yield classnm
        if classnm in self.MISSING_CLASSES:
            for bclassnm in self.MISSING_CLASSES[classnm]:
                for sclassnm in self.superclasses(bclassnm):
                    yield sclassnm
            return
        docstr = self._read_url(classnm.lower()+".html")
        for ln in docstr.split("\n"):
            ln = ln.strip()
            if "Inherits" in ln:
                for supcls in self._get_linked_classes(ln):
                    for cname in self._canonical_class_names(supcls):
                        for supsupcls in self.superclasses(cname):
                            yield supsupcls

    def itermethods(self,classnm):
        """Iterator over all methods on a given class."""
        #  These methods are missing from the online docs.
        if classnm in self.MISSING_METHODS:
            for methnm in self.MISSING_METHODS[classnm]:
                yield methnm
        if classnm in self.MISSING_CLASSES:
            for sclassnm in self.MISSING_CLASSES[classnm]:
                for methnm in self.itermethods(sclassnm):
                    yield methnm
            return
        try:
            docstr = self._read_url(classnm.lower()+"-members.html")
        except urllib2.HTTPError, e:
            if "404" not in str(e):
                raise
            assert self.isclass(classnm), "%r is not a class" % (classnm,)
        else:
            for ln in docstr.split("\n"):
                ln = ln.strip()
                if ln.startswith("<li class=\"fn\">"):
                    for methnm in self._get_linked_methods(ln):
                        yield methnm

    def relatedtypes(self,classnm,methnm):
        """Get all possible return types for a method.

        Given a classname and methodname, this method returns the set of all
        class names that are "related to" the specified method.  Basically,
        these are the classes that can be passed to the method as arguments
        or returned from it as values.
        """
        if classnm in self.MISSING_METHODS:
            if methnm in self.MISSING_METHODS[classnm]:
                for rtype in self.MISSING_METHODS[classnm][methnm][0]:
                    yield rtype
                return
        if classnm in self.MISSING_CLASSES:
            for bclassnm in self.MISSING_CLASSES[classnm]:
                for rtype in self.relatedtypes(bclassnm,methnm):
                    yield rtype
            return
        docstr = self._read_url(classnm.lower()+"-members.html")
        for ln in docstr.split("\n"):
            ln = ln.strip()
            if ln.startswith("<li class=\"fn\">"):
                if ">"+methnm+"<" not in ln:
                    continue
                methsig = ln.rsplit("</b>",1)[-1][:-5]
                #  The method signature can contain plently of C++
                #  junk, e.g. template instatiations and inner classes.
                #  We try our best to split them up into individual names.
                for word in methsig.split():
                   if word.endswith(","):
                       word = word[:-1]
                   word = word.split("::")[0]
                   if word.isalnum() and word[0].isupper():
                       for cname in self._canonical_class_names(word):
                           yield cname

    def ispurevirtual(self,classnm,methnm):
        """Check whether a given method is a pure virtual method."""
        if classnm in self.MISSING_METHODS:
            if methnm in self.MISSING_METHODS[classnm]:
                return self.MISSING_METHODS[classnm][methnm][1]
        if classnm in self.MISSING_CLASSES:
            for bclassnm in self.MISSING_CLASSES[classnm]:
                if self.ispurevirtual(bclassnm,methnm):
                    return True
            return False
        #  Pure virtual methods have a "= 0" at the end of their signature.
        docstr = self._read_url(classnm.lower()+".html")
        for ln in docstr.split("\n"):
            ln = ln.strip()
            if ln.startswith("<tr><td class=\"memItemLeft "):
                if ">"+methnm+"<" not in ln:
                    continue
                if "= 0</td>" in ln:
                    return True
        return False



def hack(appdir):
    """Convenience function for hacking a frozen PySide app down to size.

    This function is a simple convenience wrapper that creates a Hatchet
    instance and calls its main "hack" method.
    """
    h = Hatchet(appdir)
    h.hack()


if __name__ == "__main__":
    import optparse
    usage = "usage: Hatchet [options] /path/to/frozen/app [extra files]"
    op = optparse.OptionParser(usage=usage)
    op.add_option("-d","--debug",default="DEBUG",
                  help="set the logging debug level")
    op.add_option("","--follow-imports",
                  action="store_true",
                  dest="follow_imports",
                  help="follow import when loading code",
                  default=True)
    op.add_option("","--no-follow-imports",
                  action="store_false",
                  help="don't follow imports when loading code",
                  dest="follow_imports")
    op.add_option("","--analyse-only",
                  action="store_true",
                  help="just analyse the code, don't hack it",
                  dest="analyse_only")
    (opts,args) = op.parse_args()
    try:
        opts.debugs = int(opts.debug)
    except ValueError:
        try:
            opts.debug = getattr(logging,opts.debug)
        except AttributeError:
            print >>sys.stderr, "unknown debug level:", opts.debug
            sys.exit(1)
    logging.basicConfig(level=opts.debug,format="%(name)-12s:   %(message)s")
    if len(args) < 1:
        op.print_help()
        sys.exit(1)
    if not os.path.isdir(args[0]):
        print >>sys.stderr, "error: not a directory:", args[0]
        sys.exit(2)
    h = Hatchet(args[0])
    for fnm in args[1:]:
        if os.path.isdir(fnm):
            h.add_directory(fnm,follow_imports=opts.follow_imports)
        if fnm.endswith(".zip") or fnm.endswith(".exe"):
            h.add_zipfile(fnm,follow_imports=opts.follow_imports)
        else:
            h.add_file(fnm,follow_imports=opts.follow_imports)
    if not opts.analyse_only:
        h.hack()
    else:
        logger = logging.getLogger("PySideKick.Hatchet")
        if not h.mf.modules:
            h.add_directory(h.appdir)
        num_rejected_classes = 0
        num_rejected_methods = 0
        h.analyse_code()
        for rej in h.find_rejections():
            if len(rej) == 1:
               logger.debug("reject %s",rej[0])
               num_rejected_classes += 1
            else:
               logger.debug("reject %s::%s",rej[0],rej[1])
               num_rejected_methods += 1
        logger.info("keeping %d classes",len(h.keep_classes))
        logger.info("rejecting %d classes, %d methods",num_rejected_classes,
                                                       num_rejected_methods)

    sys.exit(0)


