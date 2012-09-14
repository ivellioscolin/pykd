
#include "stdafx.h"
#include "customtypes.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr CustomStruct::create(const std::string &name, ULONG alignReq /*= 0*/)
{
    return TypeInfoPtr( new CustomStruct(name, alignReq) );
}

////////////////////////////////////////////////////////////////////////////////

ULONG CustomStruct::getSize()
{
    if (UdtFieldColl::empty())
        return 0;

    UdtUtils::Field &field = UdtFieldColl::last();
    return field.m_offset + field.m_type->getSize();
}

////////////////////////////////////////////////////////////////////////////////

void CustomStruct::appendField(const std::string &fieldName, TypeInfoPtr fieldType)
{
    bool fieldExist = false;
    try
    {
        lookupField(fieldName);
        fieldExist = true;
    }
    catch (const TypeException &except)
    {
        DBG_UNREFERENCED_PARAMETER(except);
    }
    if (fieldExist)
        throw TypeException(getName(), "duplicate field name: " + fieldName);

    ULONG offset = getSize();
    offset += offset % (m_align ? m_align : fieldType->getAlignReq());
    UdtFieldColl::push_back(
        UdtUtils::Field(offset, fieldName, fieldType)
    );
}

////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr CustomUnion::create(const std::string &name)
{
    return TypeInfoPtr( new CustomUnion(name) );
}

////////////////////////////////////////////////////////////////////////////////

ULONG CustomUnion::getSize()
{
    ULONG size = 0;
    for (ULONG i = 0; i < getFieldCount(); ++i)
    {
        ULONG fieldSize = lookupField(i).m_type->getSize();
        if (fieldSize > size)
            size = fieldSize;
    }
    return size;
}

////////////////////////////////////////////////////////////////////////////////

void CustomUnion::appendField(const std::string &fieldName, TypeInfoPtr fieldType)
{
    bool fieldExist = false;
    try
    {
        lookupField(fieldName);
        fieldExist = true;
    }
    catch (const TypeException &except)
    {
        DBG_UNREFERENCED_PARAMETER(except);
    }
    if (fieldExist)
        throw TypeException(getName(), "duplicate field name: " + fieldName);

    UdtFieldColl::push_back(
        UdtUtils::Field(0, fieldName, fieldType)
    );
}

////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr PtrToVoid()
{
    return TypeInfoPtr( new PointerTypeInfo(ptrSize()) );
}

////////////////////////////////////////////////////////////////////////////////

}   // namespace pykd

////////////////////////////////////////////////////////////////////////////////
