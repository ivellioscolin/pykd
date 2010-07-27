#include "stdafx.h"

#include "dbgext.h"
#include "dbgexcept.h"
#include "dbgmem.h"
#include "dbgsystem.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////

bool
loadMemory( ULONG64 address, PVOID dest, ULONG length )
{
    address = addr64( address );

    try {
    
        HRESULT     hres = dbgExt->dataSpaces->ReadVirtual( address, dest, length, NULL );
        if ( FAILED( hres ) )
           throw DbgException( "IDebugDataSpace::ReadVirtual  failed" );
           
        return true;           
        
    } 
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	            
	
	return false;
}    

///////////////////////////////////////////////////////////////////////////////////

ULONG64
addr64( ULONG64  addr )
{
    if ( *( (ULONG*)&addr + 1 ) == 0 )
        *( (ULONG*)&addr + 1 ) = 0xFFFFFFFF;
        
    return addr;        
}

///////////////////////////////////////////////////////////////////////////////////

bool
compareMemory( ULONG64 addr1, ULONG64 addr2, ULONG length )
{
    HRESULT     hres;
    bool        result = false;

    addr1 = addr64( addr1 );
    addr2 = addr64( addr2 );

    char*  m1 = new char[length];    
    char*  m2 = new char[length];        
   
    try {
    
        hres = dbgExt->dataSpaces->ReadVirtual( addr1, m1, length, NULL );
        if ( FAILED( hres ) )
           throw DbgException( "IDebugDataSpace::ReadVirtual  failed" );
           
        hres = dbgExt->dataSpaces->ReadVirtual( addr2, m2, length, NULL );
        if ( FAILED( hres ) )
           throw DbgException( "IDebugDataSpace::ReadVirtual  failed" );           
           
        result = memcmp( m1, m2, length ) == 0;     
        
    } 
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	 
	
    delete[] m1;
    delete[] m2;
    
    return result;
}

///////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadPtrArray( ULONG64 address, ULONG  number )
{
    if ( is64bitSystem() )
    {
        ULONG64   *buffer = new ULONG64[ number ];
        
        if ( loadMemory( address, buffer, number*sizeof(ULONG64) ) )
        {
            boost::python::dict    arr;
        
            for ( ULONG  i = 0; i < number; ++i )
                arr[i] = buffer[i];
                
            delete[]  buffer;            
            
            return   arr;
        }
       
        delete[]  buffer;
        
 	    return boost::python::object();    
    }
    else
    {
        ULONG   *buffer = new ULONG[ number ];
        
        if ( loadMemory( address, buffer, number*sizeof(ULONG) ) )
        {
            boost::python::dict    arr;
        
            for ( ULONG  i = 0; i < number; ++i )
                arr[i] = addr64( buffer[i] );
                
            delete[]  buffer;            
            
            return   arr;
        }
       
        delete[]  buffer;
        
 	    return boost::python::object();       
    }
}

///////////////////////////////////////////////////////////////////////////////////