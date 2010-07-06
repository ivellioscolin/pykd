#include "stdafx.h"

#include <engextcpp.hpp>

#include "dbgexcept.h"
#include "dbgmem.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////

bool
loadMemory( ULONG64 address, PVOID dest, ULONG length )
{
    address = addr64( address );

    try {
    
        HRESULT     hres = g_Ext->m_Data->ReadVirtual( address, dest, length, NULL );
        if ( FAILED( hres ) )
           throw DbgException( "IDebugDataSpace::ReadVirtual  failed" );
           
        return true;           
        
    } 
	catch( std::exception  &e )
	{
		g_Ext->Out( "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		g_Ext->Out( "Kd2Lua unexpected error\n" );
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
    
        hres = g_Ext->m_Data->ReadVirtual( addr1, m1, length, NULL );
        if ( FAILED( hres ) )
           throw DbgException( "IDebugDataSpace::ReadVirtual  failed" );
           
        hres = g_Ext->m_Data->ReadVirtual( addr2, m2, length, NULL );
        if ( FAILED( hres ) )
           throw DbgException( "IDebugDataSpace::ReadVirtual  failed" );           
           
        result = memcmp( m1, m2, length ) == 0;     
        
    } 
	catch( std::exception  &e )
	{
		g_Ext->Out( "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		g_Ext->Out( "Kd2Lua unexpected error\n" );
	}	 
	
    delete[] m1;
    delete[] m2;
    
    return result;
}

///////////////////////////////////////////////////////////////////////////////////

