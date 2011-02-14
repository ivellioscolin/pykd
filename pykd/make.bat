@set CL=/nologo /EHsc /LD /I%BOOST_ROOT% /I%PYTHON_ROOT%\include /I%DBG_SDK_ROOT%\inc
@rc /fo resource.res pykd.rc > nul
@cvtres /out:resource.obj /nologo /machine:x86 resource.res
@cl pykd.cpp       ^
    dbgcmd.cpp     ^
    dbgdump.cpp    ^
    dbgext.cpp     ^
    dbgmem.cpp     ^
    dbgmodule.cpp  ^
    dbgpath.cpp    ^
    dbgprint.cpp   ^
    dbgprocess.cpp ^
    dbgreg.cpp     ^
    dbgsession.cpp ^
    dbgsym.cpp     ^
    dbgsystem.cpp  ^
    dbgtype.cpp    ^
    stdafx.cpp     ^
    pykd.def       ^
    resource.obj   ^
    %PYTHON_ROOT%\libs\python26.lib ^
    %DBG_SDK_ROOT%\lib\i386\dbgeng.lib
@if exist *.obj del *.obj
