#include "stdafx.h"

#include "dbgext.h"
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
        hres = dbgExt->symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &moduleBase );
        if ( FAILED( hres ) )
             throw DbgException( "IDebugSymbol::GetModuleByModuleName  failed" );
        
        DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };
        hres = dbgExt->symbols->GetModuleParameters( 1, &moduleBase, 0, &moduleParam );
        if ( FAILED( hres ) )
             throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );      
       
             
        return boost::python::object( dbgModuleClass( moduleName, moduleBase, moduleParam.Size ) );            
        
    }
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
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
        hres = dbgExt->symbols->GetModuleByOffset( addr, 0, &moduleIndex, &moduleBase );    
        
        if ( FAILED( hres ) )
        {
            return boost::python::object();   
        }       
        
        DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };
        hres = dbgExt->symbols->GetModuleParameters( 1, &moduleBase, 0, &moduleParam );
        if ( FAILED( hres ) )
             throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );      
             
        char   moduleName[0x100];             
             
        hres = 
            dbgExt->symbols->GetModuleNames(  
                moduleIndex,
                0,
                NULL,
                0,
                NULL,
                moduleName,
                sizeof( moduleName ),
                NULL,
                NULL,
                0,
                NULL );
                
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbol::GetModuleNames  failed" );                 
             
        return boost::python::object( dbgModuleClass( moduleName, moduleBase, moduleParam.Size ) );            
        
    }
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	 
	
	return boost::python::object();    
}

/////////////////////////////////////////////////////////////////////////////////

void
dbgModuleClass::reloadSymbols()
{
    HRESULT         hres;
     
    try {
 
        std::string   reloadParam = "/f "; //"/f /s ";
        reloadParam += m_name;
        hres = dbgExt->symbols->Reload( reloadParam.c_str() );
        
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbol::Reload  failed" );      
    }
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	 
	
}

/////////////////////////////////////////////////////////////////////////////////

