
from setuptools import setup
from setuptools.dist import Distribution
import pkg_resources
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('cmd', choices=['bdist_egg', 'bdist', 'bdist_wheel', 'install', 'clean'])
parser.add_argument('--plat-name', default=pkg_resources.get_build_platform() )

args = parser.parse_args()

pkg_dir = { 'win32' : 'pykd_x86', 'win-amd64' : 'pykd_x64' }.get( args.plat_name )

class BinaryDistribution(Distribution):
    def is_pure(self):
        return False

_name = "pykd"
_version = "0.3.0.11"
_desc = "python windbg extension"

setup(
    name = _name,
    version = _version,
    description = _desc,
    packages = ['pykd'],
    package_dir = {'pykd': pkg_dir},
    package_data = { 'pykd' :["*.pyd", "*.dll"]},
    include_package_data=True,
    zip_safe = False,
    distclass = BinaryDistribution,
    )


