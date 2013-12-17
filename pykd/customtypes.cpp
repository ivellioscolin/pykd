
#include "stdafx.h"
#include "customtypes.h"
#include "dbgexcept.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr TypeBuilder::createStruct( const std::string &name, ULONG align )
{
    return TypeInfoPtr( new CustomStruct( name, m_ptrSize, align ) );
}

////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr TypeBuilder::createUnion( const std::string &name, ULONG align )
{
    return TypeInfoPtr( new CustomUnion( name, m_ptrSize, align ) );
}

////////////////////////////////////////////////////////////////////////////////

ULONG CustomBase::getSize()
{
    return alignUp( m_size, getAlignReq() );
}

////////////////////////////////////////////////////////////////////////////////

ULONG CustomBase::alignUp(ULONG val, ULONG align)
{
    return val + (val % align ? align - (val % align) : 0);
}

////////////////////////////////////////////////////////////////////////////////

void CustomBase::throwIfFiledExist(const std::string &fieldName)
{
    try
    {
        lookupField(fieldName);
    }
    catch (const TypeException&)
    {
        return;
    }
    throw TypeException(getName(), "duplicate field name: " + fieldName);
}

////////////////////////////////////////////////////////////////////////////////

void CustomBase::throwIfTypeRecursive(TypeInfoPtr type)
{
    if (type->is(this))
        throw TypeException(getName(), "recursive type definition");

    if ( !type->isUserDefined() )
        return;

    const ULONG fields = type->getFieldCount();
    for (ULONG i = 0; i < fields; ++i)
        throwIfTypeRecursive(type->getFieldByIndex(i));
}

void CustomBase::checkAppendField(const std::string &fieldName, TypeInfoPtr &fieldType)
{
    if ( !fieldType )
         throw DbgException( "typeInfo can not be None" );

    throwIfFiledExist(fieldName);
    throwIfTypeRecursive(fieldType);
}

////////////////////////////////////////////////////////////////////////////////

void CustomStruct::appendField(const std::string &fieldName, TypeInfoPtr &fieldType)
{
    checkAppendField(fieldName, fieldType);

    CustomUdtField  *field = new CustomUdtField( fieldType, fieldName );

    const ULONG fieldAlignReq = fieldType->getAlignReq();
    const ULONG align = fieldAlignReq < m_align ? fieldAlignReq : m_align;
    const ULONG fieldOffset = alignUp(m_size, align);

    field->setOffset( fieldOffset );

    m_size = fieldOffset + fieldType->getSize();

    m_fields.push_back( UdtFieldPtr( field ) );
}

////////////////////////////////////////////////////////////////////////////////

void CustomUnion::appendField(const std::string &fieldName, TypeInfoPtr &fieldType)
{
    checkAppendField(fieldName, fieldType);

    CustomUdtField  *field = new CustomUdtField( fieldType, fieldName );

    ULONG fieldSize = fieldType->getSize();

    m_size = fieldSize > m_size ? fieldSize : m_size;

    m_fields.push_back( UdtFieldPtr( field ) );
}

////////////////////////////////////////////////////////////////////////////////

}   // namespace pykd

////////////////////////////////////////////////////////////////////////////////
