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

ULONG64 VarDataMemory::readPtr( ULONG ptrSize ) const
{
    return addr64( ptrSize == 4 ? ptrDWord( m_addr ) : ptrQWord( m_addr ) );
}

//////////////////////////////////////////////////////////////////////////////

VarDataConst::VarDataConst( SymbolPtr &symData) :
     m_fieldOffset(0)
{
    symData->getValue(m_value);
}

     //////////////////////////////////////////////////////////////////////////////////

std::string VarDataConst::asString() const
{
    return "<constant>";
}

//////////////////////////////////////////////////////////////////////////////////

ULONG64 VarDataConst::getAddr() const
{
    throw DbgException("Constant does not have address");
}

//////////////////////////////////////////////////////////////////////////////////

VarDataPtr VarDataConst::fork(ULONG offset) const
{
    return VarDataPtr(new VarDataConst(*this, offset));
}

//////////////////////////////////////////////////////////////////////////////////

void VarDataConst::read(PVOID buffer, ULONG length, ULONG offset /*= 0*/) const
{
    ULONG64    val = boost::apply_visitor( VariantToULong64(), m_value );
    if (offset + length > sizeof(val) )
        throw DbgException("Internal error in " __FUNCTION__);

    RtlCopyMemory(buffer, (char*)&val + offset, length);
}

//////////////////////////////////////////////////////////////////////////////////'

ULONG64 VarDataConst::readPtr( ULONG ptrSize  ) const
{
    return addr64( ptrSize == 4 ? 
        boost::apply_visitor( VariantToULong64(), m_value ) : 
        boost::apply_visitor( VariantToULong(), m_value ) );
}

//////////////////////////////////////////////////////////////////////////////////

VarDataConst::VarDataConst(const VarDataConst &from, ULONG fieldOffset) :
    m_fieldOffset(from.m_fieldOffset + fieldOffset),
    m_value(from.m_value)
{
}

//////////////////////////////////////////////////////////////////////////////////

} // end pykd namespace
