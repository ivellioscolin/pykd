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

        for ( size_t   i = 0; i < str.size() / 100 + 1; ++i )
        {
           g_dbgEng->control->ControlledOutputWide(  
                dml ? DEBUG_OUTCTL_AMBIENT_DML : DEBUG_OUTCTL_AMBIENT_TEXT, DEBUG_OUTPUT_NORMAL, 
                L"%ws",
                str.substr( i*100, min( str.size() - i*100, 100 ) ).c_str() 
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

        for ( size_t   i = 0; i < str.size() / 100 + 1; ++i )
        {
           g_dbgEng->control->OutputWide(  
                DEBUG_OUTPUT_ERROR, 
                L"%ws",
                str.substr( i*100, min( str.size() - i*100, 100 ) ).c_str() 
                );
        }
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

std::string dreadline()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    char    str[0x100];
    ULONG   inputSize = 0;

    g_dbgEng->control->Input( str, sizeof(str), &inputSize );

    return std::string( str ) + "\n";
}

///////////////////////////////////////////////////////////////////////////////////

} // end pykd namespace
