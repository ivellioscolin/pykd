from setuptools import setup
from wheel.bdist_wheel import bdist_wheel

import os
import shutil
import zipfile
import sys
import itertools

_name = "pykd"
_desc = "python windbg extension"
_version = '0.3.4.15'

def getReleaseSrc():
    return 'Release_%d.%d' % sys.version_info[0:2]

if "bdist_wheel" in sys.argv:

    # remove build catalog
    build_dir = os.path.join(os.path.curdir, 'build' )
    if os.path.exists(build_dir):
        shutil.rmtree(build_dir)

    # make package catalog
    package_dir = os.path.join(os.path.curdir, _name )
    if os.path.exists(package_dir):
        shutil.rmtree(package_dir)
    os.mkdir(package_dir)

    shutil.copy("__init__.py", package_dir)

    bin_dir = os.path.join( os.path.curdir, '..', 'kdlibcpp/bin')
    pykd_dir =  os.path.join( os.path.curdir, '..', 'out')
    if "--plat-name=win32" in sys.argv:
        bin_dir = os.path.join( bin_dir, 'x86')
        pykd_dir = os.path.join(pykd_dir, 'Win32', getReleaseSrc())
    elif "--plat-name=win-amd64" in sys.argv:
        bin_dir = os.path.join( bin_dir, 'x64')
        pykd_dir = os.path.join(pykd_dir, 'X64', getReleaseSrc())
    else:
        assert(0)

    assert(os.path.isdir(bin_dir))

    for binFile in  [ f for f in os.listdir(bin_dir) if not os.path.isdir(f) ]:
        shutil.copy( os.path.join(bin_dir, binFile), os.path.join(package_dir, binFile) )

    shutil.copy( os.path.join(pykd_dir, 'pykd.pyd'), os.path.join(package_dir, 'pykd.pyd') )

    setup(
        name = _name,
        version = _version,
        description = _desc,
       # cmdclass = { 'bdist_wheel' : bdist_wheel },
        packages = ['pykd'],
        package_dir = {'pykd': package_dir},
        package_data = { 'pykd' :["*.pyd", "*.dll"]},
        include_package_data=True,
        zip_safe = False
        )

elif "bdist_zip" in sys.argv:

    #make package catalog

    if "--plat-name=win32" in sys.argv:
        plat_name="win32"
    elif "--plat-name=win-amd64" in sys.argv:
        plat_name="win-amd64"

    package_dir = os.path.join(os.path.curdir, _name )
    if os.path.exists(package_dir):
        shutil.rmtree(package_dir)
    os.mkdir(package_dir)

    bin_dir = os.path.join( os.path.curdir, '..', 'kdlibcpp/bin')
    pykd_dir =  os.path.join( os.path.curdir, '..', 'out')
    if plat_name=="win32":
        bin_dir = os.path.join( bin_dir, 'x86')
        pykd_dir = os.path.join(pykd_dir, 'Win32', getReleaseSrc())
    elif plat_name=="win-amd64":
        bin_dir = os.path.join( bin_dir, 'x64')
        pykd_dir = os.path.join(pykd_dir, 'X64', getReleaseSrc())
    else:
        assert(0)

    pyver="%d%d" % sys.version_info[0:2]

    zip_str = "pykd-%s-cp%s-%s" % ( _version, pyver, plat_name )
    zip_name = zip_str + ".zip"

    assert(os.path.isdir(bin_dir))

    for binFile in  [ f for f in os.listdir(bin_dir) if not os.path.isdir(f) ]:
        shutil.copy( os.path.join(bin_dir, binFile), os.path.join(package_dir, binFile) )

    shutil.copy( os.path.join(pykd_dir, 'pykd.pyd'), os.path.join(package_dir, 'pykd.pyd') )

    dist_dir = os.path.join(os.path.curdir, 'dist')
    if not os.path.exists(dist_dir):
        os.mkdir(dist_dir)

    with zipfile.ZipFile(os.path.join(os.path.curdir, 'dist', zip_name), mode='w' ) as archive:
        for srcFile in os.listdir(package_dir):
            print( "zipped %s" % (srcFile) )
            archive.write( os.path.join(package_dir, srcFile), compress_type = zipfile.ZIP_DEFLATED)

    print("OK")
    
elif "bdist_pdb" in sys.argv:

    #make pdb archive
    pyVersion = ('2.7', '3.5', '3.6', '3.7', '3.8', '3.9', '3.10', '3.11')
    platform = ('Win32', 'x64')
    
    pdbFiles = [ os.path.join( platform, 'Release_' + version, 'pykd.pdb') for version, platform in itertools.product( pyVersion, platform ) ]
    
    zip_name = "pykd-%s-symbols.zip" %  _version
    
    with zipfile.ZipFile(os.path.join(os.path.curdir, 'dist', zip_name), mode='w' ) as archive:
        for pdbFile in pdbFiles:
            print( "zipped %s" % (pdbFile) )
            archive.write( os.path.join(os.path.curdir, '..', 'out', pdbFile), pdbFile, compress_type = zipfile.ZIP_DEFLATED)
    
    #pdbFileList = [ os.path.join( os.path.curdir, '..', 'out', fileName) for fileName in (
    

    pass

















