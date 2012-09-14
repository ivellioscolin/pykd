
#pragma once

#include "symengine.h"
#include "dbgexcept.h"

#include <dia2.h>

namespace pykd {

//////////////////////////////////////////////////////////////////////////////

typedef CComPtr< IDiaSymbol > DiaSymbolPtr;
typedef CComPtr< IDiaEnumSymbols > DiaEnumSymbolsPtr;
typedef CComPtr< IDiaDataSource > DiaDataSourcePtr;
typedef CComPtr< IDiaSession > DiaSessionPtr;
typedef CComPtr< IDiaEnumSymbolsByAddr > DiaEnumSymbolsByAddrPtr;
typedef CComPtr< IDiaEnumLineNumbers > DiaEnumLineNumbersPtr;
typedef CComPtr< IDiaLineNumber> DiaLineNumberPtr;
typedef CComPtr< IDiaSourceFile > DiaSourceFilePtr;

typedef std::map<ULONG, ULONG> DiaRegToRegRelativeBase;

//////////////////////////////////////////////////////////////////////////////

class DiaException : public SymbolException {
public:
    DiaException(const std::string &desc, HRESULT hres, IDiaSymbol *symbol = NULL)
        : SymbolException( makeFullDesc(desc, hres, symbol) )
        , m_hres(hres)
    {
    }

    DiaException(const std::string &desc)
        : SymbolException(descPrefix + desc)
        , m_hres(S_FALSE)
    {
    }

    HRESULT getRes() const {
        return m_hres;
    }
private:

    static const std::string descPrefix;

    static std::string makeFullDesc(const std::string &desc, HRESULT hres, IDiaSymbol *symbol = NULL);

    HRESULT m_hres;
};

////////////////////////////////////////////////////////////////////////////

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

    //static const ValueNameEntry dataKindName[DataIsConstant + 1];

    //static const ValueNameEntry symTagName[SymTagMax];

    //static const ValueNameEntry locTypeName[LocTypeMax];

    static const ValueNameEntry basicTypeName[];
    static const size_t cntBasicTypeName;

    //static const ValueNameEntry udtKindName[];
    //static const size_t cntUdtKindName;

    //static const ValueNameEntry i386RegName[];
    //static const size_t cntI386RegName;

    //static const ValueNameEntry amd64RegName[];
    //static const size_t cntAmd64RegName;

    static std::string getBasicTypeName( ULONG basicType );

protected:

    //// Check symbols loop
    //class checkSymLoop
    //{
    //public:
    //    checkSymLoop(checkSymLoop *prev)
    //        : m_symSetPtr( prev ? prev->m_symSetPtr : symSetPtr(new symSet) )
    //    {
    //    }

    //    bool check(IDiaSymbol *_symbol)
    //    {
    //        DWORD symIndexId = 0;
    //        _symbol->get_symIndexId(&symIndexId);
    //        return !m_symSetPtr->insert(symIndexId).second;
    //    }

    //private:
    //    typedef std::set<DWORD> symSet;
    //    typedef boost::shared_ptr<symSet> symSetPtr;
    //    symSetPtr m_symSetPtr;
    //};

    //static std::string printImpl(
    //    IDiaSymbol *_symbol,
    //    DWORD machineType,
    //    ULONG indent = 0,
    //    checkSymLoop *checkLoopPrev = NULL,
    //    const char *prefix = NULL
    //);

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

class DiaSession : public SymbolSession
{
public:

    DiaSession( IDiaSession* session, IDiaSymbol *globalScope ) :
        m_globalScope( globalScope ),
        m_globalSymbol( DiaSymbol::fromGlobalScope( globalScope ) ),
        m_session( session )
        {}

    SymbolPtr& getSymbolScope() {
        return m_globalSymbol;
    }

    SymbolPtr findByRva( ULONG rva, ULONG symTag = SymTagNull, LONG* displacement = NULL );

    ULONG findRvaByName( const std::string &name );

    void getSourceLine( ULONG64 offset, std::string &fileName, ULONG &lineNo, LONG &displacement );

private:

    DiaSymbolPtr    m_globalScope;
    SymbolPtr       m_globalSymbol;
    DiaSessionPtr   m_session;

};

////////////////////////////////////////////////////////////////////////////

} // end pykd namespace

