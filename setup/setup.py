
from setuptools import setup, Extension
import pkg_resources
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('cmd', choices=['bdist_egg', 'bdist', 'install'])
parser.add_argument('--plat-name', default=pkg_resources.get_build_platform() )

args = parser.parse_args()

pkg_dir = { 'win32' : 'src/x86', 'win-amd64' : 'src/x64' }.get( args.plat_name )

setup(
    name = "pykd",
    version = "0.3.0.11",
    description = "python windbg extension",
    package_dir = { '' : pkg_dir},
    packages = [''],
    package_data = { '' :["*.pyd", "*.dll"]},
    include_package_data=True,
    zip_safe = False,
    ext_modules = [Extension('pykd', [])], 
    )

