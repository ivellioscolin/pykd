#include "stdafx.h"

#include <engextcpp.hpp>

#include "dbgtype.h"
#include "dbgexcept.h"
#include "dbgmem.h"
#include "dbgsystem.h"

using namespace std;

bool
isBaseType( const std::string  &typeName );

boost::python::object
loadBaseType( const std::string  &typeName, ULONG64  address, ULONG  size );

typedef
boost::python::object
(*basicTypeLoader)( ULONG64 address, ULONG size );

boost::python::object
voidLoader( ULONG64 address, ULONG size ) {
    return boost::python::object();
}

template< typename valType>
boost::python::object
valueLoader( ULONG64 address, ULONG size );

template<>
boost::python::object
valueLoader<void*>( ULONG64 address, ULONG size )
{
    if ( is64bitSystem() )
        return valueLoader<__int64>( address, size );
    else
        return valueLoader<long>( address, size );
}

static const char*   
basicTypeNames[] = {
    "unsigned char",
    "char",
    "unsigned short",
    "short", 
    "unsigned long",
    "long",
    "<function>",
    "void" 
};
    
basicTypeLoader     basicTypeLoaders[] = {
    valueLoader<unsigned char>,
    valueLoader<char>,
    valueLoader<unsigned short>,
    valueLoader<short>,
    valueLoader<unsigned long>,
    valueLoader<long>,
    valueLoader<void*>,
    voidLoader };

///////////////////////////////////////////////////////////////////////////////////
//
boost::python::object
loadTypedVar( const std::string &moduleName, const std::string &typeName, ULONG64 address )
{
	HRESULT      hres;

    try {
        
        ULONG64         moduleBase;
        
        if ( typeName.find("*") < typeName.size() )
        {
            return valueLoader<void*>( address, ptrSize() );
        }
        
        hres = g_Ext->m_Symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &moduleBase );
  		if ( FAILED( hres ) )
			throw  DbgException( "IDebugSymbol::GetModuleByModuleName  failed" ); 
			
        ULONG        typeId;
        hres = g_Ext->m_Symbols->GetTypeId( moduleBase, typeName.c_str(), &typeId );
		if ( FAILED( hres ) )
			throw  DbgException( "IDebugSymbol::GetTypeId  failed" ); 
			
        typedVarClass		      temp( address );
		boost::python::object     var( temp );
					
        for ( ULONG   i = 0; ; ++i )
        {
            char   fieldName[100];
            hres = g_Ext->m_Symbols2->GetFieldName( moduleBase, typeId, i, fieldName, sizeof(fieldName), NULL );
            
            if ( FAILED( hres ) )
                break;  
            
            ULONG   fieldTypeId;
            ULONG   fieldOffset;
            hres = g_Ext->m_Symbols3->GetFieldTypeAndOffset( moduleBase, typeId, fieldName, &fieldTypeId, &fieldOffset );
            
            if ( FAILED( hres ) )
                throw  DbgException( "IDebugSymbol3::GetFieldTypeAndOffset  failed" ); 
            
            char    fieldTypeName[100];
            hres = g_Ext->m_Symbols->GetTypeName( moduleBase, fieldTypeId, fieldTypeName, sizeof(fieldTypeName), NULL );
            
            ULONG   fieldSize;
            hres = g_Ext->m_Symbols->GetTypeSize( moduleBase, fieldTypeId, &fieldSize );
            
            if ( FAILED( hres ) )
               throw  DbgException( "IDebugSymbol::GetTypeName  failed" ); 
               
            if ( isBaseType( fieldTypeName ) )
            {
                var.attr( fieldName ) = loadBaseType( fieldTypeName, address + fieldOffset, fieldSize  );
            }   
            else
            {
                var.attr( fieldName ) = loadTypedVar( moduleName, fieldTypeName, address + fieldOffset );
            }               
        }
        
        return var; 
    }		
			
	catch( std::exception  &e )
	{
		g_Ext->Out( "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		g_Ext->Out( "pykd unexpected error\n" );
	}	
	
	return boost::python::str( "VAR_ERR" );	
}

///////////////////////////////////////////////////////////////////////////////////		

boost::python::object
containingRecord( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &fieldName )
{
	HRESULT      hres;

    try {
        
        ULONG64         moduleBase;
        
        hres = g_Ext->m_Symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &moduleBase );
  		if ( FAILED( hres ) )
			throw  DbgException( "IDebugSymbol::GetModuleByModuleName  failed" ); 
			
        ULONG        typeId;
        hres = g_Ext->m_Symbols->GetTypeId( moduleBase, typeName.c_str(), &typeId );
		if ( FAILED( hres ) )
			throw  DbgException( "IDebugSymbol::GetTypeId  failed" ); 			
		
        ULONG       fieldTypeId;
        ULONG       fieldOffset;
        hres = g_Ext->m_Symbols3->GetFieldTypeAndOffset( moduleBase, typeId, fieldName.c_str(), &fieldTypeId, &fieldOffset );   
        
        return loadTypedVar( moduleName, typeName, address - fieldOffset );
    }		
			
	catch( std::exception  &e )
	{
		g_Ext->Out( "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		g_Ext->Out( "pykd unexpected error\n" );
	}	
	
	return boost::python::str( "VAR_ERR" );	
}

///////////////////////////////////////////////////////////////////////////////////		

bool
isBaseType( const std::string  &typeName )
{
    for ( int i = 0; i < sizeof( basicTypeNames ) / sizeof( char* ); ++i )
    {
        if ( typeName == basicTypeNames[i] )
            return true;
            
        if ( typeName == ( std::string( basicTypeNames[i] ) + "*" ) )
            return true;
            
        if ( typeName == ( std::string( basicTypeNames[i] ) + "[]" ) )
            return true;    
            
        if ( typeName == ( std::string( basicTypeNames[i] ) + "*[]" ) )
            return true;    
    }
            
    return false;   
}

///////////////////////////////////////////////////////////////////////////////////		

boost::python::object
loadBaseType( const std::string  &typeName, ULONG64  address, ULONG  size )
{
   for ( int i = 0; i < sizeof( basicTypeNames ) / sizeof( char* ); ++i )
   {
        if ( typeName == basicTypeNames[i] )
            return basicTypeLoaders[i]( address, size );
            
        if ( typeName == ( std::string( basicTypeNames[i] ) + "*" ) )
            return valueLoader<void*>( address, size );
            
        if ( typeName == ( std::string( basicTypeNames[i] ) + "[]" ) )
            return basicTypeLoaders[i]( address, size ); 
            
        if ( typeName == ( std::string( basicTypeNames[i] ) + "*[]" ) )
             return valueLoader<void*>( address, size );   
    }
            
    return boost::python::object();
}

///////////////////////////////////////////////////////////////////////////////////		



template< typename valType>
boost::python::object
valueLoader( ULONG64 address, ULONG size )
{
    if ( size == sizeof(valType) )
    {
        valType     v;          
        if ( loadMemory( address, &v, sizeof(v) ) )
            return boost::python::long_( (unsigned __int64)v );
    }
    else
    {    
        boost::python::dict     arr;
            
        for ( unsigned int i = 0; i < size / sizeof(valType); ++i )
        {
            valType  v;          
            if ( !loadMemory( address + i * sizeof(valType), &v, sizeof(v) ) )
                    return boost::python::object();
            
            arr[i] = boost::python::long_( (unsigned __int64)v );    
        }        
        
        return arr;
    }
    
    return boost::python::object();
}

///////////////////////////////////////////////////////////////////////////////////	

boost::python::object
loadComplexType( const std::string  &typeName, ULONG64  address, ULONG  size )
{
    return boost::python::object();
}

///////////////////////////////////////////////////////////////////////////////////		