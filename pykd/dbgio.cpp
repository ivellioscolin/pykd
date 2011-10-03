#include "stdafx.h"

#include <iostream>

#include "dbgio.h"
#include "dbgclient.h"
#include "windbg.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::dprint( const std::string &str, bool dml )
{
    if ( WindbgGlobalSession::isInit() )
    {
        for ( size_t   i = 0; i < str.size() / 100 + 1; ++i )
        {
           m_control->ControlledOutputWide(  
                dml ? DEBUG_OUTCTL_AMBIENT_DML : DEBUG_OUTCTL_AMBIENT_TEXT, DEBUG_OUTPUT_NORMAL, 
                L"%s",
                str.substr( i*100, min( str.size() - i*100, 100 ) ).c_str() 
                );
        }
    }
    else
    {
        std::cout << str;
    }
}

void dprint( const std::string &str, bool dml )
{
    g_dbgClient->dprint( str, dml );
}


///////////////////////////////////////////////////////////////////////////////////

void DebugClient::dprintln( const std::string &str, bool dml )
{
    this->dprint( str + "\r\n", dml );        
}

void dprintln( const std::string &str, bool dml )
{
    g_dbgClient->dprintln( str, dml );
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::eprint( const std::string &str )
{
    if ( WindbgGlobalSession::isInit() )
    {
        for ( size_t   i = 0; i < str.size() / 100 + 1; ++i )
        {
           m_control->OutputWide(  
                DEBUG_OUTPUT_ERROR, 
                L"%s",
                str.substr( i*100, min( str.size() - i*100, 100 ) ).c_str() 
                );
        }
    }
    else
    {
        std::cerr << str;
    }    
}

void eprint( const std::string &str )
{
    g_dbgClient->eprint( str );   
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::eprintln( const std::string &str )
{
    this->eprint( str + "\r\n");
}

void eprintln( const std::string &str )
{
    g_dbgClient->eprintln( str );
}

///////////////////////////////////////////////////////////////////////////////////

}; // namesapce pykd




















//
//void dbgPrint::dprint( const boost::python::object& obj, bool dml )
//{
//    std::wstring   str = boost::python::extract<std::wstring>( obj );
//
//    if ( isWindbgExt() )
//    {
//
//        for ( size_t   i = 0; i < str.size() / 100 + 1; ++i )
//        {
//            dbgExt->control4->ControlledOutputWide(  
//                dml ? DEBUG_OUTCTL_AMBIENT_DML : DEBUG_OUTCTL_AMBIENT_TEXT, DEBUG_OUTPUT_NORMAL, 
//                L"%ws",
//                str.substr( i*100, min( str.size() - i*100, 100 ) ).c_str() 
//                );
//        }
//    }
//    else
//    {
//        std::wcout << str;
//    }
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//void dbgPrint::dprintln( const boost::python::object& obj, bool dml  )
//{
//    std::wstring   str = boost::python::extract<std::wstring>( obj );
//    str += L"\r\n";
//
//    if ( isWindbgExt() )
//    {
//        for ( size_t   i = 0; i < str.size() / 100 + 1; ++i )
//        {
//            dbgExt->control4->ControlledOutputWide(  
//                dml ? DEBUG_OUTCTL_AMBIENT_DML : DEBUG_OUTCTL_AMBIENT_TEXT, DEBUG_OUTPUT_NORMAL, 
//                L"%ws",
//                str.substr( i*100, min( str.size() - i*100, 100 ) ).c_str() 
//                );
//        }
//    }
//    else
//    {
//        std::wcout << str;
//    }
//}
//
///////////////////////////////////////////////////////////////////////////////////
