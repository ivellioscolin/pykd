#include "stdafx.h"

#include "cpureg.h"
#include "dbgclient.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

CpuReg::CpuReg( IDebugClient4 *client, const std::string &regName ) :
    DbgObject( client )  
{
    HRESULT         hres;  

    m_name = regName;

    hres = m_registers->GetIndexByName( m_name.c_str(), &m_index );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegister::GetIndexByName", hres );    
}

///////////////////////////////////////////////////////////////////////////////////

CpuReg::CpuReg( IDebugClient4 *client, ULONG index ) :
    DbgObject( client )  
{
    HRESULT         hres;  

    m_index = index;

    ULONG       nameSize = 0;

    hres = 
       m_registers->GetDescription( 
            m_index,
            NULL,
            0,
            &nameSize,
            NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegister::GetDescription", hres );

    std::vector<char>   nameBuffer(nameSize);

    hres = 
        m_registers->GetDescription( 
            m_index,
            &nameBuffer[0],
            nameSize,
            NULL,
            NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegister::GetDescription", hres );

    m_name = std::string( &nameBuffer[0] );
    
}

///////////////////////////////////////////////////////////////////////////////////

BaseTypeVariant CpuReg::getValue()
{
    HRESULT         hres;   
        
    DEBUG_VALUE    debugValue;            
    hres = m_registers->GetValue( m_index, &debugValue );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegister::GetValue", hres );
        
    switch( debugValue.Type )
    {
    case DEBUG_VALUE_INT8:
        return BaseTypeVariant( (LONG)debugValue.I8 );
        break;
        
    case DEBUG_VALUE_INT16:
        return BaseTypeVariant( (LONG)debugValue.I16 );
        break;
        
    case DEBUG_VALUE_INT32:
        return BaseTypeVariant( debugValue.I32 );
        break;
        
    case DEBUG_VALUE_INT64:
        return BaseTypeVariant( debugValue.I64 );
        break;
    } 

    throw DbgException( "Failed to convert register value" );
}

///////////////////////////////////////////////////////////////////////////////////

CpuReg DebugClient::getRegByName( const std::string &regName )
{
     return CpuReg( m_client, regName );  
}


CpuReg getRegByName( const std::string &regName )
{
     return g_dbgClient->getRegByName( regName );
}

///////////////////////////////////////////////////////////////////////////////////

CpuReg DebugClient::getRegByIndex( ULONG index )
{
    return CpuReg( m_client, index );
}


CpuReg getRegByIndex( ULONG index )
{
    return g_dbgClient->getRegByIndex( index );
}

///////////////////////////////////////////////////////////////////////////////////

ULONG64 DebugClient::loadMSR( ULONG  msr )
{
    HRESULT     hres;
    ULONG64     value;

    hres = m_dataSpaces->ReadMsr( msr, &value );
    if ( FAILED( hres ) )
         throw DbgException( "IDebugDataSpaces::ReadMsr", hres );

    return value;
}

ULONG64 loadMSR( ULONG  msr )
{
    return g_dbgClient->loadMSR( msr );
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::setMSR( ULONG msr, ULONG64 value)
{
    HRESULT     hres;

    hres = m_dataSpaces->WriteMsr(msr, value);
    if ( FAILED( hres ) )
         throw DbgException( "IDebugDataSpaces::WriteMsr", hres );
}

void setMSR( ULONG msr, ULONG64 value)
{
    g_dbgClient->setMSR( msr, value );
}

///////////////////////////////////////////////////////////////////////////////////

} // end namespace pykd