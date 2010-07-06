#include "stdafx.h"

#include <engextcpp.hpp>
#include <exception>

#include "dbgsystem.h"

/////////////////////////////////////////////////////////////////////////////////

bool
is64bitSystem()
{
    HRESULT     hres;
    
    try {
        hres = g_Ext->m_Control->IsPointer64Bit();
        
        return hres == S_OK;       
        
    }    
	catch( std::exception  &e )
	{
		g_Ext->Out( "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		g_Ext->Out( "pykd unexpected error\n" );
	}	 
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////
