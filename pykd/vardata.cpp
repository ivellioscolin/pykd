// 
// Access to variable data
// 

#include "stdafx.h"

#include "vardata.h"
#include "dbgengine.h"
#include "dbgexcept.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

VarDataMemory::VarDataMemory(ULONG64 addr) :
     m_addr(addr)
{
}

////////////////////////////////////////////////////////////////////////////////

std::string VarDataMemory::asString() const
{
    std::stringstream sstr;
    sstr << "at 0x" << std::hex << m_addr;
    return sstr.str();
}

////////////////////////////////////////////////////////////////////////////////

ULONG64 VarDataMemory::getAddr() const
{
    return m_addr;
}

////////////////////////////////////////////////////////////////////////////////

VarDataPtr VarDataMemory::fork(ULONG offset) const
{
    return VarDataPtr( new VarDataMemory(m_addr + offset) );
}

////////////////////////////////////////////////////////////////////////////////

void VarDataMemory::read(PVOID buffer, ULONG length, ULONG offset /*= 0*/) const
{
    readMemory( m_addr + offset, buffer, length);
}

////////////////////////////////////////////////////////////////////////////////

ULONG64 VarDataMemory::readPtr() const
{
    return ptrPtr( m_addr );
}

//////////////////////////////////////////////////////////////////////////////

VarDataConst::VarDataConst( SymbolPtr &symData) :
     m_fieldOffset(0)
    , m_dataBuff( new std::vector< UCHAR >((size_t)symData->getType()->getSize(), 0) )
{
    VARIANT vtValue = {0};
    symData->getValue(vtValue);

    switch (vtValue.vt)
    {
    case VT_I1:
    case VT_UI1:
        fillDataBuff(vtValue.bVal);
        break;

    case VT_BOOL:
        fillDataBuff(!!vtValue.iVal);
        break;

    case VT_I2:
    case VT_UI2:
        fillDataBuff(vtValue.iVal);
        break;

    case VT_I4:
    case VT_UI4:
    case VT_INT:
    case VT_UINT:
    case VT_ERROR:
    case VT_HRESULT:
        fillDataBuff(vtValue.lVal);
        break;

    case VT_I8:
    case VT_UI8:
        fillDataBuff(vtValue.llVal);
        break;

    case VT_R4:
        fillDataBuff(vtValue.fltVal);
        break;

    case VT_R8:
        fillDataBuff(vtValue.dblVal);
        break;

    default:
        throw DbgException( "Unsupported const value" );
    }
}

////////////////////////////////////////////////////////////////////////////////

std::string VarDataConst::asString() const
{
    return "<constant>";
}

////////////////////////////////////////////////////////////////////////////////

ULONG64 VarDataConst::getAddr() const
{
    throw DbgException("Constant does not have address");
}

////////////////////////////////////////////////////////////////////////////////

VarDataPtr VarDataConst::fork(ULONG offset) const
{
    return VarDataPtr(new VarDataConst(*this, offset));
}

////////////////////////////////////////////////////////////////////////////////

void VarDataConst::read(PVOID buffer, ULONG length, ULONG offset /*= 0*/) const
{
    if (offset + length > m_dataBuff->size())
        throw DbgException("Internal error in " __FUNCTION__);
    RtlCopyMemory(buffer, &m_dataBuff->at(offset), length);
}

////////////////////////////////////////////////////////////////////////////////'

ULONG64 VarDataConst::readPtr() const
{
    ULONG64 val = 0;
    const ULONG length = ptrSize();
    if (length > m_dataBuff->size())
        throw DbgException("Internal error in " __FUNCTION__);
    RtlCopyMemory(&val, &m_dataBuff->at(0), length);
    return val;
}

////////////////////////////////////////////////////////////////////////////////

VarDataConst::VarDataConst(const VarDataConst &from, ULONG fieldOffset) :
    m_fieldOffset(from.m_fieldOffset + fieldOffset)
    , m_dataBuff(from.m_dataBuff)
{
}

////////////////////////////////////////////////////////////////////////////////

}

////////////////////////////////////////////////////////////////////////////////

