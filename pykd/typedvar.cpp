#include "stdafx.h"    

#include "typedvar.h"
#include "dbgclient.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

TypedVar::TypedVar ( IDebugClient4 *client, const TypeInfo& typeInfo, ULONG64 offset ) :
    DbgObject( client ),
    m_typeInfo( typeInfo ),
    m_offset( offset )
{
    m_size = m_typeInfo.getSize();
}

///////////////////////////////////////////////////////////////////////////////////

TypedVar::TypedVar( const TypeInfo& typeInfo, ULONG64 offset ) :
    DbgObject( g_dbgClient->client() ),
    m_typeInfo( typeInfo ),
    m_offset( offset )
{
    m_size = m_typeInfo.getSize();
}

///////////////////////////////////////////////////////////////////////////////////

TypedVarPtr
TypedVar::getField( const std::string &fieldName ) 
{
    TypeInfo fieldType = m_typeInfo.getField( fieldName );

    TypedVarPtr     tv;

    if ( fieldType.isBasicType() )
    {
        tv.reset( new BasicTypedVar( m_client, fieldType, m_offset + fieldType.getOffset() ) );
    }
    else
        throw DbgException( "can not get field" );

    return tv;
}

///////////////////////////////////////////////////////////////////////////////////

ULONG64  
BasicTypedVar::getValue() const
{
    HRESULT     hres;
    ULONG64     val = 0;

    hres = m_dataSpaces->ReadVirtual( m_offset, &val, getSize(), NULL );

    if ( FAILED( hres ) )
        throw MemoryException( m_offset, false );

    return val;
}

///////////////////////////////////////////////////////////////////////////////////

} // end pykd namespace