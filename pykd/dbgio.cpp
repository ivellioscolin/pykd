#include "stdafx.h"

#include <iostream>
#include <Fcntl.h>

#include "dbgio.h"
#include "dbgext.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////

void dbgPrint::dprint( const boost::python::object& obj, bool dml )
{
    std::wstring   str = boost::python::extract<std::wstring>( obj );

    if ( isWindbgExt() )
    {

        for ( size_t   i = 0; i < str.size() / 100 + 1; ++i )
        {
            dbgExt->control4->ControlledOutputWide(  
                dml ? DEBUG_OUTCTL_AMBIENT_DML : DEBUG_OUTCTL_AMBIENT_TEXT, DEBUG_OUTPUT_NORMAL, 
                L"%ws",
                str.substr( i*100, min( str.size() - i*100, 100 ) ).c_str() 
                );
        }
    }
    else
    {
        std::wcout << str;
    }
}

/////////////////////////////////////////////////////////////////////////////////

void dbgPrint::dprintln( const boost::python::object& obj, bool dml  )
{
    std::wstring   str = boost::python::extract<std::wstring>( obj );
    str += L"\r\n";

    if ( isWindbgExt() )
    {
        for ( size_t   i = 0; i < str.size() / 100 + 1; ++i )
        {
            dbgExt->control4->ControlledOutputWide(  
                dml ? DEBUG_OUTCTL_AMBIENT_DML : DEBUG_OUTCTL_AMBIENT_TEXT, DEBUG_OUTPUT_NORMAL, 
                L"%ws",
                str.substr( i*100, min( str.size() - i*100, 100 ) ).c_str() 
                );
        }
    }
    else
    {
        std::wcout << str;
    }
}

/////////////////////////////////////////////////////////////////////////////////
