// 
// Access to variable data
// 

#pragma once

////////////////////////////////////////////////////////////////////////////////

//#include <array>
#include "symengine.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

class VarData;
typedef boost::shared_ptr< VarData > VarDataPtr;

// access to variable data interface
class VarData
{
public:
    virtual ~VarData() {}

    virtual std::string asString() const = 0;
    virtual ULONG64 getAddr() const = 0;
    virtual VarDataPtr fork(ULONG offset) const = 0;
    virtual void read(PVOID buffer, ULONG length, ULONG offset = 0) const = 0;
    virtual ULONG64 readPtr() const = 0;
};

// variable in memory
class VarDataMemory : public VarData
{
public:

    // IVarData implementation
    virtual std::string asString() const;
    virtual ULONG64 getAddr() const;
    virtual VarDataPtr fork(ULONG offset) const;

    virtual void read(PVOID buffer, ULONG length, ULONG offset = 0) const;
    virtual ULONG64 readPtr() const;

    static VarDataPtr factory(ULONG64 addr) {
        return VarDataPtr( new VarDataMemory(addr) );
    }

protected:
    VarDataMemory(ULONG64 addr);

private:
    ULONG64 m_addr;
};



// constant variable
class VarDataConst : public VarData
{
public:
    // IVarData implementation
    virtual std::string asString() const;
    virtual ULONG64 getAddr() const;
    virtual VarDataPtr fork(ULONG offset) const;

    virtual void read(PVOID buffer, ULONG length, ULONG offset = 0) const;
    virtual ULONG64 readPtr() const;

    static VarDataPtr factory(SymbolPtr &symData) {
        return VarDataPtr( new VarDataConst(symData) );
    }

protected:
    VarDataConst(SymbolPtr &symData);
    VarDataConst(const VarDataConst &from, ULONG fieldOffset);

    //template<typename T>
    //void fillDataBuff(const T &data)
    //{
    //    RtlCopyMemory( &m_dataBuff->at(0), &data, min(sizeof(T), m_dataBuff->size()) );
    //}

private:

    BaseTypeVariant  m_value;
    ULONG  m_fieldOffset;

    //ULONG m_fieldOffset;
    //boost::shared_ptr< std::vector<UCHAR> > m_dataBuff;

    
};

}

//////////////////////////////////////////////////////////////////////////
