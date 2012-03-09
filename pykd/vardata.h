// 
// Access to variable data
// 

#pragma once

////////////////////////////////////////////////////////////////////////////////

#include <array>
#include <boost\smart_ptr\shared_ptr.hpp>

#include "diawrapper.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

// pointer to variable data access interface
interface IVarData;
typedef boost::shared_ptr< IVarData > VarDataPtr;

// access to variable data interface
interface IVarData
{
    virtual ~IVarData() {}

    virtual std::string asString() const = 0;
    virtual ULONG64 getAddr() const = 0;
    virtual VarDataPtr fork(ULONG offset) const = 0;

    virtual void read(PVOID buffer, ULONG length, ULONG offset = 0) const = 0;
    virtual ULONG64 readPtr() const = 0;
};

// variable in memory
class VarDataMemory : public IVarData
{
public:

    // IVarData implementation
    virtual std::string asString() const override;
    virtual ULONG64 getAddr() const override;
    virtual VarDataPtr fork(ULONG offset) const override;

    virtual void read(PVOID buffer, ULONG length, ULONG offset = 0) const override;
    virtual ULONG64 readPtr() const override;

    static VarDataPtr factory(CComPtr< IDebugDataSpaces4 > dataSpaces, ULONG64 addr) {
        return VarDataPtr( new VarDataMemory(dataSpaces, addr) );
    }

protected:
    VarDataMemory(CComPtr< IDebugDataSpaces4 > dataSpaces, ULONG64 addr);

private:
    CComPtr< IDebugDataSpaces4 > m_dataSpaces;
    ULONG64 m_addr;
};

// constant variable
class VarDataConst : public IVarData
{
public:
    // IVarData implementation
    virtual std::string asString() const override;
    virtual ULONG64 getAddr() const override;
    virtual VarDataPtr fork(ULONG offset) const override;

    virtual void read(PVOID buffer, ULONG length, ULONG offset = 0) const override;
    virtual ULONG64 readPtr() const override;

    static VarDataPtr factory(CComPtr< IDebugControl4 > control, pyDia::SymbolPtr symData) {
        return VarDataPtr( new VarDataConst(control, symData) );
    }

protected:
    VarDataConst(CComPtr< IDebugControl4 > control, pyDia::SymbolPtr symData);
    VarDataConst(const VarDataConst &from, ULONG fieldOffset);

    template<typename T>
    void fillDataBuff(const T &data)
    {
        RtlCopyMemory( &m_dataBuff->at(0), &data, min(sizeof(T), m_dataBuff->size()) );
    }

private:
    CComPtr< IDebugControl4 > m_control;

    ULONG m_fieldOffset;
    boost::shared_ptr< std::vector<UCHAR> > m_dataBuff;
};

}

//////////////////////////////////////////////////////////////////////////
