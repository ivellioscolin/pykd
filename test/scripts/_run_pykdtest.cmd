::@echo on

:: Pass $(TargetDir)\<TestApp.exe> from Visual Studio 
set TestAppPath=%1

:: Pass $(PlatformName) from Visual Studio 
set TestAppPlatform=%2

set Arch=x64
if "%PROCESSOR_ARCHITECTURE%" == "x86" ( 
    if not defined PROCESSOR_ARCHITEW6432 set Arch=x86
) 

set PythonRegKey=HKLM\Software\Python\PythonCore\2.6\InstallPath

set RegSwitch=64
if "%TestAppPlatform%"=="Win32" set RegSwitch=32

for /F "tokens=3*" %%A in ('reg.exe query %PythonRegKey% /ve /reg:%RegSwitch% 2^>NUL ^| FIND "REG_SZ"') do set PythonInstallPath=%%A

%PythonInstallPath%python.exe "%~dp0pykdtest.py" %TestAppPath%

::pause