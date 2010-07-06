#include "stdafx.h"

#include <engextcpp.hpp>

#include "dbgmodule.h"
#include "dbgexcept.h"
#include "dbgmem.h"

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadModule( const std::string &moduleName )
{
    HRESULT         hres;
   
    try {
        ULONG64    moduleBase;
        hres = g_Ext->m_Symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &moduleBase );
        if ( FAILED( hres ) )
             throw DbgException( "IDebugSymbol::GetModuleByModuleName  failed" );
        
        DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };
        hres = g_Ext->m_Symbols->GetModuleParameters( 1, &moduleBase, 0, &moduleParam );
        if ( FAILED( hres ) )
             throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );      
       
             
        return boost::python::object( dbgModuleClass( moduleBase, moduleParam.Size ) );            
        
    }
	catch( std::exception  &e )
	{
		g_Ext->Out( "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		g_Ext->Out( "pykd unexpected error\n" );
	}	 
	
	return boost::python::object();
}

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
findModule( ULONG64 addr )
{
    HRESULT         hres;
    
    addr = addr64( addr );
      
    try {
    
        ULONG     moduleIndex;
        ULONG64   moduleBase;            
        hres = g_Ext->m_Symbols->GetModuleByOffset( addr, 0, &moduleIndex, &moduleBase );    
        
        if ( FAILED( hres ) )
        {
            return boost::python::object();   
        }       
        
        DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };
        hres = g_Ext->m_Symbols->GetModuleParameters( 1, &moduleBase, 0, &moduleParam );
        if ( FAILED( hres ) )
             throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );      
       
             
        return boost::python::object( dbgModuleClass( moduleBase, moduleParam.Size ) );            
        
    }
	catch( std::exception  &e )
	{
		g_Ext->Out( "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		g_Ext->Out( "pykd unexpected error\n" );
	}	 
	
	return boost::python::object();    
}

/////////////////////////////////////////////////////////////////////////////////
