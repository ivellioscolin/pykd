from setuptools import setup
from setuptools.dist import Distribution
import pkg_resources
import argparse
import os
import shutil
import zipfile

_name = "pykd"
_desc = "python windbg extension"
_version = '0.3.0.26'

def  makeWheel(args):

    # remove build catalog
    build_dir = os.path.join(os.path.curdir, 'build' )
    if os.path.exists(build_dir):
        shutil.rmtree(build_dir)

    # make package catalog
    package_dir = os.path.join(os.path.curdir, _name )
    if os.path.exists(package_dir):
        shutil.rmtree(package_dir)
    os.mkdir(package_dir)

    with open(os.path.join(package_dir, '__init__.py'),'w') as f:
        f.write("from pykd import *\n")
        f.write("__version__ = pykd.__version__\n")
        f.write("__file__ = pykd.__file__\n")

    bin_dir = os.path.join( os.path.curdir, '..', 'bin')
    pykd_dir =  os.path.join( os.path.curdir, '..', 'out')
    if args.plat_name == 'win32':
        bin_dir = os.path.join( bin_dir, 'x86')
        pykd_dir = os.path.join(pykd_dir, 'Win32', 'Release_2.7')
    elif args.plat_name == 'win-amd64':
        bin_dir = os.path.join( bin_dir, 'x64')
        pykd_dir = os.path.join(pykd_dir, 'X64', 'Release_2.7')
    else:
        assert(0)

    assert(os.path.isdir(bin_dir))

    for binFile in  [ f for f in os.listdir(bin_dir) if not os.path.isdir(f) ]:
        shutil.copy( os.path.join(bin_dir, binFile), os.path.join(package_dir, binFile) )

    shutil.copy( os.path.join(pykd_dir, 'pykd.pyd'), os.path.join(package_dir, 'pykd.pyd') )

    class BinaryDistribution(Distribution):
        def is_pure(self):
            return False
    
    setup(
        name = _name,
        version = _version,
        description = _desc,
        packages = ['pykd'],
        package_dir = {'pykd': package_dir},
        package_data = { 'pykd' :["*.pyd", "*.dll"]},
        include_package_data=True,
        zip_safe = False,
        distclass = BinaryDistribution,
        )

def makeZip(args):

    # make package catalog
    package_dir = os.path.join(os.path.curdir, _name )
    if os.path.exists(package_dir):
        shutil.rmtree(package_dir)
    os.mkdir(package_dir)

    bin_dir = os.path.join( os.path.curdir, '..', 'bin')
    pykd_dir =  os.path.join( os.path.curdir, '..', 'out')
    if args.plat_name == 'win32':
        bin_dir = os.path.join( bin_dir, 'x86')
        pykd_dir = os.path.join(pykd_dir, 'Win32', 'Release_2.7')
    elif args.plat_name == 'win-amd64':
        bin_dir = os.path.join( bin_dir, 'x64')
        pykd_dir = os.path.join(pykd_dir, 'X64', 'Release_2.7')
    else:
        assert(0)

    zip_str = "pykd-%s-py27-%s" % ( _version, args.plat_name )
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
            print "zipped %s" % (srcFile)
            archive.write( os.path.join(package_dir, srcFile), compress_type = zipfile.ZIP_DEFLATED)

    print "OK"




parser = argparse.ArgumentParser()

subparsers = parser.add_subparsers()
wheelParser = subparsers.add_parser('bdist_wheel')
wheelParser.add_argument('--plat-name', choices = ['win32', 'win-amd64'], default=pkg_resources.get_build_platform() )
wheelParser.set_defaults(func=makeWheel)

zipParser = subparsers.add_parser('bdist_zip')
zipParser.add_argument('--plat-name', choices = ['win32', 'win-amd64'], default=pkg_resources.get_build_platform() )
zipParser.set_defaults(func=makeZip)

args = parser.parse_args()
args.func(args)


