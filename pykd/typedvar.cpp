#include "stdafx.h"    

#include "typedvar.h"
#include "dbgclient.h"
#include "dbgmem.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

TypedVarPtr   TypedVar::getTypedVar( IDebugClient4 *client, const TypeInfoPtr& typeInfo, ULONG64 offset )
{
    TypedVarPtr     tv;

    if ( typeInfo->isBasicType() )
    {
        tv.reset( new BasicTypedVar( client, typeInfo, offset) );
        return tv;
    }

    if ( typeInfo->isPointer() )
    {
        tv.reset( new PtrTypedVar( client, typeInfo, offset ) );
        return tv;
    }

    if ( typeInfo->isArray() )
    {
        tv.reset( new ArrayTypedVar( client, typeInfo, offset  ) );
        return tv;
    }

    if ( typeInfo->isUserDefined() )
    {
        tv.reset( new UdtTypedVar( client, typeInfo, offset ) );
        return tv;
    }

    if ( typeInfo->isBitField() )
    {
        tv.reset( new BitFieldVar( client, typeInfo, offset ) );
        return tv;
    }

    if ( typeInfo->isEnum() )
    {
        tv.reset( new EnumTypedVar( client, typeInfo, offset ) );
        return tv;
    }

    throw DbgException( "can not create typedVar for this type" );

    return tv;
}

///////////////////////////////////////////////////////////////////////////////////

TypedVar::TypedVar ( IDebugClient4 *client, const TypeInfoPtr& typeInfo, ULONG64 offset ) :
    DbgObject( client ),
    m_typeInfo( typeInfo ),
    m_offset( offset ),
    m_dataKind( DataIsGlobal )
{
    m_size = m_typeInfo->getSize();
}
///////////////////////////////////////////////////////////////////////////////////

BaseTypeVariant BasicTypedVar::getValue()
{
    ULONG64     val = 0;    

    readMemory( m_dataSpaces, m_offset, &val, getSize(), false );

    if ( m_typeInfo->getName() == "Char" )
        return (LONG)*(PCHAR)&val;

    if ( m_typeInfo->getName() == "WChar" )
        return (LONG)*(PWCHAR)&val;

    if ( m_typeInfo->getName() == "Int1B" )
        return (LONG)*(PCHAR)&val;

    if ( m_typeInfo->getName() == "UInt1B" )
        return (ULONG)*(PUCHAR)&val;

    if ( m_typeInfo->getName() == "Int2B" )
        return (LONG)*(PSHORT)&val;

    if ( m_typeInfo->getName() == "UInt2B" )
        return (ULONG)*(PUSHORT)&val;

    if ( m_typeInfo->getName() == "Int4B" )
        return *(PLONG)&val;

    if ( m_typeInfo->getName() == "UInt4B" )
        return *(PULONG)&val;

    if ( m_typeInfo->getName() == "Int8B" )
        return (LONG64)*(PLONG64)&val;

    if ( m_typeInfo->getName() == "UInt8B" )
        return (ULONG64)*(PULONG64)&val;

    if ( m_typeInfo->getName() == "Long" )
        return *(PLONG)&val;

    if ( m_typeInfo->getName() == "ULong" )
        return *(PULONG)&val;

    if ( m_typeInfo->getName() == "Bool" )
        return *(bool*)&val;
    
    throw DbgException( "failed get value " );
}

///////////////////////////////////////////////////////////////////////////////////

BaseTypeVariant PtrTypedVar::getValue()
{
    ULONG64     val = 0;

    readMemoryPtr( m_dataSpaces,  m_offset, &val );

    return val;
}

///////////////////////////////////////////////////////////////////////////////////

TypedVarPtr PtrTypedVar::deref()
{
    ULONG64     val = 0;

    readMemoryPtr( m_dataSpaces,  m_offset, &val );

    return TypedVar::getTypedVar( m_client, m_typeInfo->deref(), val );
}

///////////////////////////////////////////////////////////////////////////////////

TypedVarPtr
UdtTypedVar::getField( const std::string &fieldName ) 
{
    TypeInfoPtr fieldType = m_typeInfo->getField( fieldName );

    return  TypedVar::getTypedVar( m_client, fieldType, m_offset + fieldType->getOffset() );
}

///////////////////////////////////////////////////////////////////////////////////

BaseTypeVariant BitFieldVar::getValue()
{
    ULONG64     val = 0;

    readMemory( m_dataSpaces, m_offset, &val, getSize(), false );

    val >>= m_typeInfo->getBitOffset();
    val &= m_typeInfo->getBitWidth();

    switch ( m_typeInfo->getSize() )
    {
    case 1:
        return (ULONG)*(PUCHAR)&val;

    case 2:
        return (ULONG)*(PUSHORT)&val;

    case 4:
        return *(PULONG)&val;

    case 8:
        return *(PULONG64)&val;
    }

    throw DbgException( "failed get value " );
}

///////////////////////////////////////////////////////////////////////////////////

BaseTypeVariant  EnumTypedVar::getValue()
{
    ULONG       val = 0;

    readMemory( m_dataSpaces, m_offset, &val, getSize(), false );

    return val;
};

///////////////////////////////////////////////////////////////////////////////////

} // end pykd namespace