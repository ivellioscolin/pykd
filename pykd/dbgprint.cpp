#include "stdafx.h"
#include "dbgprint.h"
#include <engextcpp.hpp>

using namespace std;

void DbgPrint::dprint( const string&  str )
{
	g_Ext->Out( str.c_str() );
}

void DbgPrint::dprintln( const std::string&  str )
{
    g_Ext->Out( str.c_str() );
    g_Ext->Out( "\r\n" );
}