#include "stdafx.h"

#include <engextcpp.hpp>

#include "dbgreg.h"
#include "dbgexcept.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadRegister( const std::string &registerName )
{
    HRESULT         hres;
   
   
    try {
    
        ULONG    registerIndex = 0;
    
        hres = g_Ext->m_Registers->GetIndexByName( registerName.c_str(), &registerIndex );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugRegister::GetIndexByName  failed" );
            
        DEBUG_VALUE    debugValue;            
        hres = g_Ext->m_Registers->GetValue( registerIndex, &debugValue );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugRegister::GetValue  failed" );
            
        switch( debugValue.Type )
        {
        case DEBUG_VALUE_INT8:
            return boost::python::long_( debugValue.I8 );
            break;
            
        case DEBUG_VALUE_INT16:
            return boost::python::long_( debugValue.I16 );
            break;
            
        case DEBUG_VALUE_INT32:
            return boost::python::long_( debugValue.I32 );
            break;
            
        case DEBUG_VALUE_INT64:
            return boost::python::long_(debugValue.I64 );
            break;
       }
    }
	catch( std::exception  &e )
	{
		g_Ext->Out( "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		g_Ext->Out( "pykd unexpected error\n" );
	}	 
	
	return boost::python::str( "REG_ERR" );
}

///////////////////////////////////////////////////////////////////////////////////
