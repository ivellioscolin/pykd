
#pragma once

#include "dia\diadecls.h"

namespace pykd {

//////////////////////////////////////////////////////////////////////////////

typedef std::map<ULONG, ULONG> DiaRegToRegRelativeBase;

//////////////////////////////////////////////////////////////////////////////

class DiaSymbol : public Symbol {
public:
    DiaSymbol( DiaSymbolPtr &_symbol, DWORD machineType );

    static SymbolPtr fromGlobalScope( IDiaSymbol *_symbol );

    SymbolPtr getChildByName(const std::string &_name );

    ULONG getRva();

    SymbolPtrList findChildren(
        ULONG symTag,
        const std::string &name = "",
        bool caseSensitive = FALSE
    );

    ULONGLONG getSize();

    std::string getName();

    SymbolPtr getType();

    SymbolPtr getIndexType();

    ULONG getSymTag();

    ULONGLONG getVa();

    ULONG getLocType();

    LONG getOffset();

    ULONG getCount();

    void getValue( BaseTypeVariant &vtValue );

    bool isBasicType();

    bool isIndirectVirtualBaseClass();

    bool isVirtualBaseClass();

    ULONG getBaseType();

    ULONG getBitPosition();

    ULONG getIndexId();

    ULONG getUdtKind();

    ULONG getDataKind();

    //ULONG getRegisterId();
    virtual ULONG getRegRealativeId() override;

    ULONG getMachineType() {
        return m_machineType;
    }

    //SymbolPtr getChildByName(const std::string &_name);

    ULONG getChildCount( ULONG symTag );

    ULONG getChildCount() {
        return getChildCount(SymTagNull);
    }

    SymbolPtr getChildByIndex(ULONG symTag, ULONG _index );

    SymbolPtr getChildByIndex(ULONG _index ) {
        return getChildByIndex( SymTagNull, _index );
    }
    
    bool isConstant();

    //std::string print();

    //bool eq(Symbol &rhs);

    int getVirtualBasePointerOffset();

    ULONG getVirtualBaseDispIndex();

    ULONG getVirtualBaseDispSize();

    //ULONG getSection();

    void setLoadAddress( ULONGLONG baseAddress );

public:
    typedef std::pair<ULONG, const char *> ValueNameEntry;

    static const ValueNameEntry basicTypeName[];
    static const size_t cntBasicTypeName;

    static std::string getBasicTypeName( ULONG basicType );

protected:
    static const DiaRegToRegRelativeBase &regToRegRelativeAmd64;
    static const DiaRegToRegRelativeBase &regToRegRelativeI386;
    ULONG getRegRealativeIdImpl(const DiaRegToRegRelativeBase &DiaRegToRegRelative);

    template <typename TRet>
    TRet callSymbolT(
        HRESULT(STDMETHODCALLTYPE IDiaSymbol::*method)(TRet *),
        const char *methodName
    )
    {
        TRet retValue;
        HRESULT hres = (m_symbol->*method)(&retValue);
        if (S_OK != hres)
            throw DiaException(std::string("Call IDiaSymbol::") + methodName, hres, m_symbol);

        return retValue;
    }

    DiaSymbolPtr m_symbol;

    DWORD m_machineType;
};


////////////////////////////////////////////////////////////////////////////

} // end pykd namespace

