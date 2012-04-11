#include "stdafx.h"

#include <iostream>

#include "dbgio.h"
#include "dbgclient.h"
#include "windbg.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::dprint( const std::wstring &str, bool dml )
{
    if ( WindbgGlobalSession::isInit() )
    {
        for ( size_t   i = 0; i < str.size() / 100 + 1; ++i )
        {
           m_control->ControlledOutputWide(  
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

void dprint( const std::wstring &str, bool dml )
{
    g_dbgClient->dprint( str, dml );
}


///////////////////////////////////////////////////////////////////////////////////

void DebugClient::dprintln( const std::wstring &str, bool dml )
{
    this->dprint( str + L"\r\n", dml );        
}

void dprintln( const std::wstring &str, bool dml )
{
    g_dbgClient->dprintln( str, dml );
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::eprint( const std::wstring &str )
{
    if ( WindbgGlobalSession::isInit() )
    {
        for ( size_t   i = 0; i < str.size() / 100 + 1; ++i )
        {
           m_control->OutputWide(  
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

void eprint( const std::wstring &str )
{
    g_dbgClient->eprint( str );   
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::eprintln( const std::wstring &str )
{
    this->eprint( str + L"\r\n");
}

void eprintln( const std::wstring &str )
{
    g_dbgClient->eprintln( str );
}

///////////////////////////////////////////////////////////////////////////////////

void
DbgOut::write( const std::wstring  &str )
{
    if ( WindbgGlobalSession::isInit() )
    {
        for ( size_t   i = 0; i < str.size() / 100 + 1; ++i )
        {
           m_control->ControlledOutputWide(  
                DEBUG_OUTCTL_AMBIENT_TEXT,
                DEBUG_OUTPUT_NORMAL, 
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

std::string
DbgIn::readline()
{
    char    str[0x100];
    ULONG   inputSize = 0;

    m_control->Input( str, sizeof(str), &inputSize );

    return std::string( str ) + "\n";
}

///////////////////////////////////////////////////////////////////////////////////

}; // namesapce pykd
