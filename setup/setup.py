import os
import shutil
import zipfile
import sys
import argparse
import re
import setuptools
import git

if sys.version_info.major == 2:
    import pathlib2

_name = "pykd"
_desc = "python windbg extension"

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--plat-name', choices = ['win32', 'win-amd64', 'x86', 'x64'], required = True, type = str.lower)
    parser.add_argument('--build-base', type = str, required = False)
    parser.add_argument('--dist-dir', type = str, required = False)
    parser.add_argument('--egg-base', type = str, required = False)
    args, unkonwn_args = parser.parse_known_args()

    platform = ''
    platform_alt = ''
    configuration = 'Release'
    if (args.plat_name == 'win32') or (args.plat_name == 'x86'):
        args.plat_name = 'win32'
        platform = 'win32'
        platform_alt = 'x86'
    elif (args.plat_name == 'win-amd64') or (args.plat_name == 'x64'):
        args.plat_name = 'win-amd64'
        platform = 'x64'
        platform_alt = 'x64'
    else:
        print("Unsupported platform %s" %(args.plat_name))
        exit

    python_tag = "cp%d%d" %(sys.version_info.major, sys.version_info.minor)

    dir_solution = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

    _version = ''

    # Using VERSION file on releaase branch
    if os.path.isfile(os.path.join(os.path.dirname(__file__), 'VERSION')):
        with open(os.path.join(os.path.dirname(__file__), 'VERSION'), 'r') as verf:
            _version = verf.readline()

    # Using versions from pykdver.h and branch HEAD
    if _version == '':
        versions = ['', '', '', '']
        if os.path.isfile(os.path.join(dir_solution, 'pykd', 'pykdver.h')):
            with open(os.path.join(os.path.join(dir_solution, 'pykd', 'pykdver.h')), 'r') as verf:
                lines = verf.readlines()
                for line in lines:
                    ver_raw_match = re.findall(r"^#define\s*PYKD_VERSION_MAJOR\s*(.*)$", line)
                    if (len(ver_raw_match)):
                        versions[0] = ver_raw_match[0]
                        continue
                    ver_raw_match = re.findall(r"^#define\s*PYKD_VERSION_MINOR\s*(.*)$", line)
                    if (len(ver_raw_match)):
                        versions[1] = ver_raw_match[0]
                        continue
                    ver_raw_match = re.findall(r"^#define\s*PYKD_VERSION_SUBVERSION\s*(.*)$", line)
                    if (len(ver_raw_match)):
                        versions[2] = ver_raw_match[0]
                        continue
                    ver_raw_match = re.findall(r"^#define\s*PYKD_VERSION_BUILDNO\s*(.*)$", line)
                    if (len(ver_raw_match)):
                        versions[3] = ver_raw_match[0]
                        continue

        _version = "%s.%s.%s.%s+g" % (versions[0], versions[1], versions[2], versions[3])

        try:
            repo = git.Repo(dir_solution)
            _version = "%s%s" % (_version, repo.git.rev_parse(repo.head, short = True))
        except:
            print("Skip using HEAD SHA as non git repo")

    dir_dbgsdk_bin = os.path.join(dir_solution, 'kdlibcpp', 'bin', platform_alt)
    if not os.path.exists(dir_dbgsdk_bin):
        print("DbgSDK bin path %s doesn't exist" %(dir_dbgsdk_bin))
        sys.exit()

    dir_pykd_bin = os.path.join(dir_solution, 'Out', platform, "%s_%d.%d" %(configuration, sys.version_info.major, sys.version_info.minor))
    if not os.path.exists(os.path.join(dir_pykd_bin, 'pykd.pyd')):
        print("pykd.pyd doesn't exist in %s" %(dir_pykd_bin))
        sys.exit()

    if args.build_base is not None:
        dir_build_base = args.build_base
    else:
        dir_build_base = os.path.join(dir_solution, 'Obj', 'Wheel', platform, "%s_%d.%d" %(configuration, sys.version_info.major, sys.version_info.minor), 'setuptools')

    if args.dist_dir is not None:
        dir_dist_wheel = args.dist_dir
    else:
        dir_dist_wheel = dir_pykd_bin

    if args.egg_base is not None:
        dir_egg_base = args.egg_base
    else:
        dir_egg_base = dir_build_base

    dir_wheel_package = os.path.join(dir_build_base, _name)

    if os.path.exists(dir_build_base):
        shutil.rmtree(dir_build_base)
    if sys.version_info.major == 2:
        pathlib2.Path(dir_build_base).mkdir(exist_ok = True, parents = True)
    else:
        os.makedirs(dir_build_base, exist_ok = True)

    if os.path.exists(dir_wheel_package):
        shutil.rmtree(dir_wheel_package)
    if sys.version_info.major == 2:
        pathlib2.Path(dir_wheel_package).mkdir(exist_ok = True, parents = True)
    else:
        os.makedirs(dir_wheel_package, exist_ok = True)

    print("Using DbgSDK from %s" %(dir_dbgsdk_bin))
    print("Using pykd binary from %s" %(dir_pykd_bin))
    print("Using build directory as %s" %(dir_build_base))
    print("Using egg info directory as %s" %(dir_egg_base))
    print("Using Wheel output directory as %s" %(dir_dist_wheel))

    shutil.copy2(os.path.join(dir_solution, 'setup', '__init__.py'), dir_wheel_package)
    shutil.copy2(os.path.join(dir_pykd_bin, 'pykd.pyd'), dir_wheel_package)
    for dbgSdkBin in  [ f for f in os.listdir(dir_dbgsdk_bin) if not os.path.isdir(f) ]:
        shutil.copy2(os.path.join(dir_dbgsdk_bin, dbgSdkBin), dir_wheel_package)

    if 'bdist_wheel' in unkonwn_args:
        print("Building bdist_wheel ...")
        sys.argv = ['setup.py']
        sys.argv.append("build")
        sys.argv.append("--build-base=%s" %(dir_build_base))
        sys.argv.append("--plat-name=%s" %(args.plat_name))
        sys.argv.append("bdist_wheel")
        sys.argv.append("--dist-dir=%s" %(dir_dist_wheel))
        sys.argv.append("--plat-name=%s" %(args.plat_name))
        sys.argv.append("--python-tag=%s" %(python_tag))
        sys.argv.append("egg_info")
        sys.argv.append("--egg-base=%s" %(dir_egg_base))

        setuptools.setup(
            name = _name,
            version = _version,
            description = _desc,
            packages = ['pykd'],
            package_dir = {'pykd': os.path.relpath(dir_wheel_package)},
            package_data = { 'pykd' :["*.pyd", "*.dll"]},
            include_package_data=True,
            zip_safe = False
        )

    if 'bdist_zip' in unkonwn_args:
        zip_name = "pykd-%s-%s-%s.zip" %(_version, python_tag, args.plat_name)
        print("Building bdist_zip %s ..." %(zip_name))
        with zipfile.ZipFile(os.path.join(dir_dist_wheel, zip_name), mode='w') as archive:
            for dbgSdkBin in  [ f for f in os.listdir(dir_dbgsdk_bin) if not os.path.isdir(f) ]:
                print("zipped %s" %(dbgSdkBin))
                archive.write(os.path.join(dir_dbgsdk_bin, dbgSdkBin), arcname = dbgSdkBin, compress_type = zipfile.ZIP_DEFLATED)
            print("zipped pykd.pyd")
            archive.write(os.path.join(dir_pykd_bin, 'pykd.pyd'), arcname = 'pykd.pyd', compress_type = zipfile.ZIP_DEFLATED)
            print("zipped pykd.pdb")
            archive.write(os.path.join(dir_pykd_bin, 'pykd.pdb'), arcname = 'pykd.pdb', compress_type = zipfile.ZIP_DEFLATED)

if __name__ == '__main__':
    main()

















