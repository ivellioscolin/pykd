#include "stdafx.h"
#include "diasymbol.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

#define _DEF_BASIC_TYPE(x)  DiaSymbol::ValueNameEntry(bt##x, #x)
const DiaSymbol::ValueNameEntry DiaSymbol::basicTypeName[] = {
    _DEF_BASIC_TYPE(NoType),
    _DEF_BASIC_TYPE(Void),
    _DEF_BASIC_TYPE(Char),
    _DEF_BASIC_TYPE(WChar),
    _DEF_BASIC_TYPE(Int),
    _DEF_BASIC_TYPE(UInt),
    _DEF_BASIC_TYPE(Float),
    _DEF_BASIC_TYPE(BCD),
    _DEF_BASIC_TYPE(Bool),
    _DEF_BASIC_TYPE(Long),
    _DEF_BASIC_TYPE(ULong),
    _DEF_BASIC_TYPE(Currency),
    _DEF_BASIC_TYPE(Date),
    _DEF_BASIC_TYPE(Variant),
    _DEF_BASIC_TYPE(Complex),
    _DEF_BASIC_TYPE(Bit),
    _DEF_BASIC_TYPE(BSTR),
    _DEF_BASIC_TYPE(Hresult)
};
#undef _DEF_BASIC_TYPE

const size_t DiaSymbol::cntBasicTypeName = _countof(DiaSymbol::basicTypeName);

///////////////////////////////////////////////////////////////////////////////

namespace
{

const struct DiaRegToRegRelativeAmd64 : DiaRegToRegRelativeBase
{
    typedef std::map<ULONG, ULONG> Base;
    DiaRegToRegRelativeAmd64();
} g_DiaRegToRegRelativeAmd64;

DiaRegToRegRelativeAmd64::DiaRegToRegRelativeAmd64()
{
    (*this)[CV_AMD64_RIP] = rriInstructionPointer;
    (*this)[CV_AMD64_RBP] = rriStackFrame;
    (*this)[CV_AMD64_RSP] = rriStackPointer;
}

}

const DiaRegToRegRelativeBase &DiaSymbol::regToRegRelativeAmd64 = g_DiaRegToRegRelativeAmd64;

///////////////////////////////////////////////////////////////////////////////

namespace
{

const struct DiaRegToRegRelativeI386 : DiaRegToRegRelativeBase
{
    typedef std::map<ULONG, ULONG> Base;
    DiaRegToRegRelativeI386();
} g_DiaRegToRegRelativeI386;

DiaRegToRegRelativeI386::DiaRegToRegRelativeI386()
{
    (*this)[CV_REG_EIP] = rriInstructionPointer;
    (*this)[CV_REG_EBP] = rriStackFrame;
    (*this)[CV_REG_ESP] = rriStackPointer;
}

}

const DiaRegToRegRelativeBase &DiaSymbol::regToRegRelativeI386 = g_DiaRegToRegRelativeI386;

///////////////////////////////////////////////////////////////////////////////

} //pykd namespace end

