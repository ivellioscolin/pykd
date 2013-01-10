
#include "stdafx.h"
#include "customtypes.h"
#include "dbgexcept.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {


////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr TypeBuilder::createStruct(  const std::string &name, ULONG align )
{
    return TypeInfoPtr( new CustomStruct( name, m_ptrSize, align ? align : m_ptrSize ) );
}

////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr TypeBuilder::createUnion( const std::string &name )
{
    return TypeInfoPtr( new CustomUnion( name, m_ptrSize, m_ptrSize ) );
}

////////////////////////////////////////////////////////////////////////////////

void CustomBase::throwIfFiledExist(const std::string &fieldName)
{
    bool fieldExist = false;
    try
    {
        lookupField(fieldName);
        fieldExist = true;
    }
    catch (const TypeException&)
    {
    }

    if (fieldExist)
        throw TypeException(getName(), "duplicate field name: " + fieldName);
}

////////////////////////////////////////////////////////////////////////////////

void CustomBase::throwIfTypeRecursive(TypeInfoPtr type)
{
    if (type->is(this))
        throw TypeException(getName(), "recursive type definition");

    if ( !type->isUserDefined() )
        return;

    try
    {
        const ULONG fields = type->getFieldCount();
        for (ULONG i = 0; i < fields; ++i)
        {
            TypeInfoPtr fileldType = type->getFieldByIndex(i);
            if (fileldType->is(this))
                throw TypeException(getName(), "wrong type of field");

            throwIfTypeRecursive(fileldType);
        }
    }
    catch (const TypeException &except)
    {
        DBG_UNREFERENCED_PARAMETER(except);
    }
}

////////////////////////////////////////////////////////////////////////////////

void CustomStruct::appendField(const std::string &fieldName, TypeInfoPtr &fieldType )
{
    throwIfFiledExist(fieldName);
    throwIfTypeRecursive(fieldType);

    CustomUdtField  *field = new CustomUdtField( fieldType, fieldName );

    ULONG fieldSize = fieldType->getSize();
    ULONG offset = m_size;
    ULONG align = fieldSize < m_align ? fieldSize : m_align;

    if (align)
        offset += offset % align > 0 ? align - offset % align : 0;

    field->setOffset( offset );

    m_size = offset + fieldSize;

    m_fields.push_back( UdtFieldPtr( field ) );
}

////////////////////////////////////////////////////////////////////////////////

void CustomUnion::appendField(const std::string &fieldName, TypeInfoPtr &fieldType )
{
    throwIfFiledExist(fieldName);
    throwIfTypeRecursive(fieldType);

    CustomUdtField  *field = new CustomUdtField( fieldType, fieldName );

    ULONG fieldSize = fieldType->getSize();

    m_size = fieldSize > m_size ? fieldSize : m_size;

    m_fields.push_back( UdtFieldPtr( field ) );
}

////////////////////////////////////////////////////////////////////////////////










//////////////////////////////////////////////////////////////////////////////////
//
//CustomTypeBase::CustomTypeBase(const std::string &name, ULONG pointerSize) 
//    : UdtFieldColl(name)
//{
//    m_ptrSize = pointerSize ? pointerSize : ptrSize();
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//void CustomTypeBase::throwIfFiledExist(const std::string &fieldName)
//{
//    bool fieldExist = false;
//    try
//    {
//        lookupField(fieldName);
//        fieldExist = true;
//    }
//    catch (const TypeException &except)
//    {
//        DBG_UNREFERENCED_PARAMETER(except);
//    }
//    if (fieldExist)
//        throw TypeException(getName(), "duplicate field name: " + fieldName);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//void CustomTypeBase::throwIfTypeRecursive(TypeInfoPtr type)
//{
//    if (type->is(this))
//        throw TypeException(getName(), "wrong type of field");
//
//    return throwIfTypeRecursiveImpl(type);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//void CustomTypeBase::throwIfTypeRecursiveImpl(TypeInfoPtr type)
//{
//    if (type->isEnum())
//        return;
//
//    try
//    {
//        const ULONG fields = type->getFieldCount();
//        for (ULONG i = 0; i < fields; ++i)
//        {
//            TypeInfoPtr fileldType = type->getFieldByIndex(i);
//            if (fileldType->is(this))
//                throw TypeException(getName(), "wrong type of field");
//
//            throwIfTypeRecursiveImpl(fileldType);
//        }
//    }
//    catch (const TypeException &except)
//    {
//        DBG_UNREFERENCED_PARAMETER(except);
//    }
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//TypeInfoPtr CustomStruct::create(
//    const std::string &name,
//    ULONG alignReq /*= 0*/,
//    ULONG pointerSize /*= 0*/)
//{
//    return TypeInfoPtr( new CustomStruct(name, alignReq, pointerSize) );
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONG CustomStruct::getSize()
//{
//    if (Base::empty())
//        return 0;
//
//    UdtUtils::Field &field = Base::last();
//    return field.m_offset + field.m_type->getSize();
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//void CustomStruct::appendField(const std::string &fieldName, TypeInfoPtr fieldType)
//{
//    Base::throwIfFiledExist(fieldName);
//    Base::throwIfTypeRecursive(fieldType);
//
//    ULONG offset = getSize();
//    offset += offset % (m_align ? m_align : fieldType->getAlignReq());
//    Base::push_back(
//        UdtUtils::Field(offset, fieldName, fieldType)
//    );
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//TypeInfoPtr CustomUnion::create(const std::string &name, ULONG pointerSize /*= 0*/)
//{
//    return TypeInfoPtr( new CustomUnion(name, pointerSize) );
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONG CustomUnion::getSize()
//{
//    ULONG size = 0;
//    for (ULONG i = 0; i < getFieldCount(); ++i)
//    {
//        ULONG fieldSize = lookupField(i).m_type->getSize();
//        if (fieldSize > size)
//            size = fieldSize;
//    }
//    return size;
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//void CustomUnion::appendField(const std::string &fieldName, TypeInfoPtr fieldType)
//{
//    Base::throwIfFiledExist(fieldName);
//    Base::throwIfTypeRecursive(fieldType);
//
//    Base::push_back(
//        UdtUtils::Field(0, fieldName, fieldType)
//    );
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//TypeInfoPtr PtrToVoid()
//{
//    return TypeInfoPtr( new PointerTypeInfo(ptrSize()) );
//}

////////////////////////////////////////////////////////////////////////////////

}   // namespace pykd

////////////////////////////////////////////////////////////////////////////////
