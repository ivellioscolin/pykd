#include "stdafx.h"

#include <iostream>

#include "dbgprint.h"
#include "dbgext.h"

using namespace std;

void DbgPrint::dprint( const boost::python::object& obj, bool dml )
{
    std::wstring   str = boost::python::extract<std::wstring>( obj );

    dbgExt->control4->ControlledOutputWide(  dml ? DEBUG_OUTCTL_AMBIENT_DML : DEBUG_OUTCTL_AMBIENT_TEXT, DEBUG_OUTPUT_NORMAL, str.c_str() );
}

void DbgPrint::dprintln( const boost::python::object& obj, bool dml  )
{
    std::wstring   str = boost::python::extract<std::wstring>( obj );
    str += L"\r\n";

    dbgExt->control4->ControlledOutputWide(  dml ? DEBUG_OUTCTL_AMBIENT_DML : DEBUG_OUTCTL_AMBIENT_TEXT, DEBUG_OUTPUT_NORMAL, str.c_str() );
}

