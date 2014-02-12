#include "stdafx.h"
#include "dbgio.h"
#include "win/dbgeng.h"
#include "win/windbg.h"


namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

void dprint( const std::wstring &str, bool dml )
{
    if ( WindbgGlobalSession::isInit() )
    {
        PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

        std::wstringstream  sstr(str);
        std::wstring  line;

        while (std::getline(sstr, line)) 
        {
            line += L'\n';

            g_dbgEng->control->ControlledOutputWide(  
                dml ? DEBUG_OUTCTL_AMBIENT_DML : DEBUG_OUTCTL_AMBIENT_TEXT,
                DEBUG_OUTPUT_NORMAL, 
                L"%ws",
                line.c_str()
                );
        }
    }
    else
    {
        python::object       sys = python::import("sys");
        sys.attr("stdout").attr("write")( str );
    }
}

///////////////////////////////////////////////////////////////////////////////////

void dprintln( const std::wstring &str, bool dml  )
{
    dprint( str + L"\r\n", dml );
}

///////////////////////////////////////////////////////////////////////////////////

void eprint( const std::wstring &str )
{
    if ( WindbgGlobalSession::isInit() )
    {
        PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

        g_dbgEng->control->OutputWide(  
            DEBUG_OUTPUT_ERROR, 
            L"%ws",
            str.c_str()
            );

    }
    else
    {
        python::object       sys = python::import("sys");
        sys.attr("stderr").attr("write")( str );
    }
}

///////////////////////////////////////////////////////////////////////////////////

void eprintln( const std::wstring &str )
{
    eprint( str + L"\r\n" );
}

///////////////////////////////////////////////////////////////////////////////////

std::wstring dreadline()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    wchar_t    str[0x100];
    ULONG     inputSize = 0;

    g_dbgEng->control->InputWide( str, sizeof(str), &inputSize );

    return std::wstring( str ) + L"\n";
}

///////////////////////////////////////////////////////////////////////////////////

} // end pykd namespace
