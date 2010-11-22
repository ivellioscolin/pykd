#include "stdafx.h"

#include "dbgext.h"
#include "dbgsym.h"
#include "dbgexcept.h"
#include "dbgprint.h"
#include "dbgmem.h"

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
findSymbolForAddress( ULONG64 addr )
{
    HRESULT         hres;  
    
    try {
        
        addr = addr64( addr );     
            
        ULONG     moduleIndex;
        ULONG64   moduleBase;            
        hres = dbgExt->symbols->GetModuleByOffset( addr, 0, &moduleIndex, &moduleBase );
    
        if ( FAILED( hres ) )
        {
            return boost::python::object( "out of module" );   
        }         
        
        char   moduleName[0x100];
        hres =  dbgExt->symbols2->GetModuleNameString( DEBUG_MODNAME_MODULE, moduleIndex, moduleBase, 
                    moduleName, sizeof( moduleName ), NULL );
                    
        if ( FAILED( hres ) )
             throw DbgException( "IDebugSymbol2::GetModuleNameString  failed" );           
    
        char        symbolName[0x100];
        ULONG64     displace = 0;
        hres = dbgExt->symbols->GetNameByOffset( addr, symbolName, sizeof(symbolName), NULL, &displace );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbol::GetNameByOffset  failed" );    
        
        std::stringstream      ss;
        displace == 0 ?  ss << symbolName : ss << symbolName << '+' << std::hex << displace;
        
        return boost::python::object( ss.str() );
                               
    }
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	 
	
	return boost::python::object( addr );
}

/////////////////////////////////////////////////////////////////////////////////

ULONG64
findAddressForSymbol( const std::string  &moduleName, const std::string  &symbolName )
{
    HRESULT         hres;  
    
    try {
    
        std::string   ModuleSymName = moduleName;
        ModuleSymName += "!";
        ModuleSymName += symbolName;
    
        ULONG64    offset = 0ULL;
        hres = dbgExt->symbols->GetOffsetByName( ModuleSymName.c_str(), &offset );
        if ( FAILED( hres ) )
             throw DbgException( "IDebugSymbol::GetOffsetByName  failed" );                
        
        return offset;    
    }
    catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	 
	
	return (ULONG64)~0;
}

/////////////////////////////////////////////////////////////////////////////////