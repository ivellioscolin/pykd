
#pragma once

#include "dia\diadecls.h"
#include "dia\diapubsymcache.h"

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
        const std::string &name = ""
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

    virtual ULONG getRegRealativeId() override;

    ULONG getMachineType() {
        return m_machineType;
    }

    ULONG getChildCount( ULONG symTag );

    ULONG getChildCount() {
        return getChildCount(SymTagNull);
    }

    SymbolPtr getChildByIndex(ULONG symTag, ULONG _index );

    SymbolPtr getChildByIndex(ULONG _index ) {
        return getChildByIndex( SymTagNull, _index );
    }
    
    bool isConstant();

    int getVirtualBasePointerOffset();

    ULONG getVirtualBaseDispIndex();

    ULONG getVirtualBaseDispSize();

    virtual std::string getBuildDescription() const;

public:
    typedef std::pair<ULONG, const char *> ValueNameEntry;

    static const ValueNameEntry basicTypeName[];
    static const size_t cntBasicTypeName;

    static std::string getBasicTypeName( ULONG basicType );

protected:
    static const DiaRegToRegRelativeBase &regToRegRelativeAmd64;
    static const DiaRegToRegRelativeBase &regToRegRelativeI386;
    ULONG getRegRealativeIdImpl(const DiaRegToRegRelativeBase &DiaRegToRegRelative);

    // IDiaSymbol::findChildren/IDiaEnumSymbols::get_Count wrapper
    typedef std::pair< DiaEnumSymbolsPtr, LONG > SelectedChilds;
    SelectedChilds selectChildren(
        ULONG symtag,
        LPCOLESTR name = NULL,
        DWORD compareFlags = nsNone
    );

    SymbolPtr getChildBySelected(const SelectedChilds &selected, const std::string &name);

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
    DiaPublicSymbolCachePtr m_publicSymbols;
};


////////////////////////////////////////////////////////////////////////////

} // end pykd namespace

