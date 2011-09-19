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

    HRESULT  hres = dbgExt->control4->ControlledOutputWide(  dml ? DEBUG_OUTCTL_AMBIENT_DML : DEBUG_OUTCTL_AMBIENT_TEXT, DEBUG_OUTPUT_NORMAL, L"%ws", str.c_str() );
   
    std::wcout << str;
}

/////////////////////////////////////////////////////////////////////////////////

void dbgPrint::dprintln( const boost::python::object& obj, bool dml  )
{
    std::wstring   str = boost::python::extract<std::wstring>( obj );
    str += L"\r\n";

    dbgExt->control4->ControlledOutputWide(  dml ? DEBUG_OUTCTL_AMBIENT_DML : DEBUG_OUTCTL_AMBIENT_TEXT, DEBUG_OUTPUT_NORMAL, L"%ws", str.c_str() );
    
    std::wcout << str;    
}

/////////////////////////////////////////////////////////////////////////////////
