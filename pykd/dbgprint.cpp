#include "stdafx.h"

#include <iostream>

#include "dbgprint.h"
#include "dbgext.h"

using namespace std;

void DbgPrint::dprint( const string&  str )
{
    HRESULT  hres = dbgExt->control->ControlledOutput( DEBUG_OUTCTL_AMBIENT_DML, DEBUG_OUTPUT_NORMAL, str.c_str() );
    if ( FAILED( hres ) )
        std::cout << str;        
}

void DbgPrint::dprintln( const std::string&  str )
{
    DbgPrint::dprint( str );
    DbgPrint::dprint( "\r\n" );
}

