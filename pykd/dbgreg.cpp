#include "stdafx.h"

#include "dbgext.h"
#include "dbgreg.h"
#include "dbgexcept.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////

cpuReg::cpuReg( std::string  regName )
{
    HRESULT         hres;  

    m_name = regName;
    m_lived = false; 

    hres = dbgExt->registers->GetIndexByName( m_name.c_str(), &m_index );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegister::GetIndexByName  failed" );    
    
    reloadValue();       
}

///////////////////////////////////////////////////////////////////////////////////

cpuReg::cpuReg( ULONG index )
{
    HRESULT         hres;  

    m_index = index;
    m_lived = false; 

    ULONG       nameSize = 0;

    hres = 
        dbgExt->registers->GetDescription( 
            m_index,
            NULL,
            0,
            &nameSize,
            NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegister::GetDescription failed" );

    std::vector<char>   nameBuffer(nameSize);

    hres = 
        dbgExt->registers->GetDescription( 
            m_index,
            &nameBuffer[0],
            nameSize,
            NULL,
            NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegister::GetDescription failed" );

    m_name = std::string( &nameBuffer[0] );
    
    reloadValue();       
}

///////////////////////////////////////////////////////////////////////////////////

void  cpuReg::reloadValue() const
{
    HRESULT         hres;   
        
    DEBUG_VALUE    debugValue;            
    hres = dbgExt->registers->GetValue( m_index, &debugValue );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegister::GetValue  failed" );
        
    switch( debugValue.Type )
    {
    case DEBUG_VALUE_INT8:
        m_value = debugValue.I8;
        break;
        
    case DEBUG_VALUE_INT16:
        m_value = debugValue.I16;
        break;
        
    case DEBUG_VALUE_INT32:
        m_value = debugValue.I32;
        break;
        
    case DEBUG_VALUE_INT64:
        m_value = debugValue.I64;
        break;
   }   
}

///////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadRegister( const std::string &registerName )
{
    HRESULT         hres;

    ULONG    registerIndex = 0;

    hres = dbgExt->registers->GetIndexByName( registerName.c_str(), &registerIndex );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegister::GetIndexByName  failed" );
        
    DEBUG_VALUE    debugValue;            
    hres = dbgExt->registers->GetValue( registerIndex, &debugValue );
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

   throw DbgException( "Invalid register value" );  
}

///////////////////////////////////////////////////////////////////////////////////

ULONG64
loadMSR( ULONG  msr )
{
    HRESULT     hres;
    ULONG64     value;

    hres = dbgExt->dataSpaces->ReadMsr( msr, &value );
    if ( FAILED( hres ) )
         throw DbgException( "IDebugDataSpaces::ReadMsr  failed" );

    return value;
}

///////////////////////////////////////////////////////////////////////////////////

void setMSR( ULONG msr, ULONG64 value)
{
    HRESULT     hres;

    hres = dbgExt->dataSpaces->WriteMsr(msr, value);
    if ( FAILED( hres ) )
         throw DbgException( "IDebugDataSpaces::WriteMsr  failed" );
}

///////////////////////////////////////////////////////////////////////////////////
