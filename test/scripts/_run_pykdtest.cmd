:: Pass $(TargetDir)\<TestApp.exe> from Visual Studio 
set TestAppPath=%1

:: Pass $(PlatformName) from Visual Studio 
set TestAppPlatform=%2

set Arch=x64
if "%PROCESSOR_ARCHITECTURE%" == "x86" ( 
    if not defined PROCESSOR_ARCHITEW6432 set Arch=x86
) 

:: Select appropriate python.exe path
set PythonRegKey=HKLM\Software\Python\PythonCore\2.6\InstallPath
if "%Arch%" == "x64" ( 
    if "%TestAppPlatform%" == "Win32" set PythonRegKey=HKLM\Software\Wow6432Node\Python\PythonCore\2.6\InstallPath
) 

for /F "tokens=3*" %%A in ('reg.exe query %PythonRegKey% /ve 2^>NUL ^| FIND "REG_SZ"') do set PythonInstallPath=%%B
::echo %PythonInstallPath%

%PythonInstallPath%python.exe "%~dp0pykdtest.py" %TestAppPath%

::pause
