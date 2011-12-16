::
:: Copy to boost root dir
::

call :ExecBjam 32
call :ExecBjam 64
pause
goto :EOF


:ExecBjam
set arch=%1
set stagedir=stage
if "%arch%"=="64" set stagedir=stage64

bjam.exe ^
    -j 4 ^
    --toolset=msvc-9.0 ^
    release debug ^
    threading=multi link=static runtime-link=shared ^
    address-model=%arch% ^
    --with-python --with-date_time --with-regex --with-thread ^
    python=2.6 ^
    --stagedir=%stagedir% ^
    stage
goto :EOF
