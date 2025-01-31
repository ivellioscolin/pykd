@ECHO OFF
SETLOCAL EnableDelayedExpansion

set py_list=2.7 3.5 3.6 3.7 3.8 3.9 3.10 3.11
set plat_list=win32 x64

REM Build wheel and zip
REM py -<py version> setup.py bdist_wheel --plat-name=<plat-name> [bdist_zip]
REM py -<py version> setup.py bdist_wheel --plat-name=<plat-name> [--build-base=build_dir] [--dist-dir=bdist_dir] [--egg-base=egg_dir] [bdist_zip]

for %%a in (%py_list%) do (
    for %%b in (%plat_list%) do (
        set py_ver=%%a
        set plat=%%b
        py -!py_ver! setup.py bdist_wheel --plat-name=!plat! bdist_zip
    )
)
exit
