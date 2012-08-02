
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

//////////////////////////////////////////////////////////////////////////////

class DiaException : public SymbolException {
public:
    DiaException(const std::string &desc, HRESULT hres)
        : SymbolException( makeFullDesc(desc, hres) )
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

    static std::string makeFullDesc(const std::string &desc, HRESULT hres);

    HRESULT m_hres;
};

////////////////////////////////////////////////////////////////////////////

class DiaSymbol : public Symbol {
public:

    DiaSymbol( DiaSymbolPtr &_symbol );

    DiaSymbol( IDiaSymbol *_symbol );

    SymbolPtr getChildByName(const std::string &_name);

    ULONG getRva();

    SymbolPtrList findChildren(
        ULONG symTag,
        const std::string &name = "",
        bool caseSensitive = FALSE
    );

    //python::list findChildrenEx(
    //    ULONG symTag,
    //    const std::string &name = "",
    //    DWORD nameCmpFlags = 0
    //)
    //{
    //    return toPyList( findChildrenImpl(symTag, name, nameCmpFlags) );
    //}

    //python::list findChildren(
    //    const std::string &name
    //)
    //{
    //    return toPyList( findChildrenImpl(SymTagNull, name, nsfCaseSensitive) );
    //}

    ULONGLONG getSize();

    std::string getName();

    //std::string getUndecoratedName();

    SymbolPtr getType();

    //SymbolPtr getIndexType();

    ULONG getSymTag();

    //ULONG getRva();
    ULONGLONG getVa();

    ULONG getLocType();

    LONG getOffset();

    ULONG getCount();

    //static void getValueImpl(IDiaSymbol *_symbol, VARIANT &vtValue);
    //python::object getValue();
    
    void getValue( VARIANT &vtValue);

    //bool isBasicType();

    bool isVirtualBaseClass();

    //bool isIndirectVirtualBaseClass();

    ULONG getBaseType();

    ULONG getBitPosition();

    //ULONG getIndexId();

    //ULONG getUdtKind();

    ULONG getDataKind();

    //ULONG getRegisterId();

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
    //
    //bool isConstant();

    //std::string print();

    //bool eq(Symbol &rhs);

    int getVirtualBasePointerOffset();

    ULONG getVirtualBaseDispIndex();

    ULONG getVirtualBaseDispSize();

    //ULONG getSection();

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

    template <typename TRet>
    TRet callSymbolT(
        HRESULT(STDMETHODCALLTYPE IDiaSymbol::*method)(TRet *),
        const char *methodName
    )
    {
        TRet retValue;
        HRESULT hres = (m_symbol->*method)(&retValue);
        if (S_OK != hres)
            throw DiaException(std::string("Call IDiaSymbol::") + methodName, hres);

        return retValue;
    }

    DiaSymbolPtr m_symbol;

    DWORD m_machineType;
};

////////////////////////////////////////////////////////////////////////////

} // end pykd namespace































//#include <set>
//
//#include <boost\smart_ptr\scoped_ptr.hpp>
//
//#include <cvconst.h>
//
//#include "utils.h"
//#include "dbgexcept.h"
//
//namespace pyDia {
//
//typedef CComPtr< IDiaSymbol > DiaSymbolPtr;
//typedef CComPtr< IDiaEnumSymbols > DiaEnumSymbolsPtr;
//typedef CComPtr< IDiaDataSource > DiaDataSourcePtr;
//typedef CComPtr< IDiaSession > DiaSessionPtr;
//
//////////////////////////////////////////////////////////////////////////////////
//// DIA Exceptions
//////////////////////////////////////////////////////////////////////////////////
//class Exception : public pykd::SymbolException {
//public:
//    Exception(const std::string &desc, HRESULT hres)
//        : SymbolException( makeFullDesc(desc, hres) )
//        , m_hres(hres)
//    {
//    }
//
//    Exception(const std::string &desc)
//        : SymbolException(descPrefix + desc)
//        , m_hres(S_FALSE)
//    {
//    }
//
//    HRESULT getRes() const {
//        return m_hres;
//    }
//private:
//
//    static const std::string descPrefix;
//
//    static std::string makeFullDesc(const std::string &desc, HRESULT hres);
//
//    HRESULT m_hres;
//};
//
//class Symbol;
//typedef boost::shared_ptr< Symbol > SymbolPtr;
//typedef std::list< SymbolPtr > SymbolPtrList;
//
//////////////////////////////////////////////////////////////////////////////////
//// Symbol
//////////////////////////////////////////////////////////////////////////////////
//class Symbol {
//public:
//    Symbol()
//    {
//        throw Exception("DiaSymbol must be created over factory from DiaScope::...");
//    }
//    Symbol(__inout DiaSymbolPtr &_symbol, DWORD machineType) 
//        : m_machineType(machineType)
//    {
//        m_symbol = _symbol.Detach();
//    }
//    Symbol(__in IDiaSymbol *_symbol, DWORD machineType) 
//        : m_machineType(machineType)
//    {
//        m_symbol = _symbol;
//    }
//
//
//    SymbolPtrList findChildrenImpl(
//        ULONG symTag,
//        const std::string &name = "",
//        DWORD nameCmpFlags = 0
//    );
//
//    python::list findChildrenEx(
//        ULONG symTag,
//        const std::string &name = "",
//        DWORD nameCmpFlags = 0
//    )
//    {
//        return toPyList( findChildrenImpl(symTag, name, nameCmpFlags) );
//    }
//
//    python::list findChildren(
//        const std::string &name
//    )
//    {
//        return toPyList( findChildrenImpl(SymTagNull, name, nsfCaseSensitive) );
//    }
//
//    ULONGLONG getSize();
//
//    std::string getName();
//    std::string getUndecoratedName();
//
//    SymbolPtr getType();
//
//    SymbolPtr getIndexType();
//
//    ULONG getSymTag();
//
//    ULONG getRva();
//    ULONGLONG getVa();
//
//    ULONG getLocType();
//
//    LONG getOffset();
//
//    ULONG getCount();
//
//    static void getValueImpl(IDiaSymbol *_symbol, VARIANT &vtValue);
//    python::object getValue();
//    void getValue( VARIANT &vtValue);
//
//    bool isBasicType();
//
//    bool isVirtualBaseClass();
//
//    bool isIndirectVirtualBaseClass();
//
//    ULONG getBaseType();
//
//    ULONG getBitPosition();
//
//    ULONG getIndexId();
//
//    ULONG getUdtKind();
//
//    ULONG getDataKind();
//
//    ULONG getRegisterId();
//
//    ULONG getMachineType() const {
//        return m_machineType;
//    }
//
//    SymbolPtr getChildByName(const std::string &_name);
//
//    template<ULONG symTag>
//    ULONG getChildCount();
//
//    ULONG getChildCount() {
//        return getChildCount<SymTagNull>();
//    }
//
//    template<ULONG symTag>
//    SymbolPtr getChildByIndex(ULONG _index );
//
//    SymbolPtr getChildByIndex(ULONG _index ) {
//        return getChildByIndex<SymTagNull>( _index );
//    }
//    
//    bool isConstant();
//
//    std::string print();
//
//    bool eq(Symbol &rhs);
//
//    int getVirtualBasePointerOffset();
//
//    ULONG getVirtualBaseDispIndex();
//
//    ULONG getVirtualBaseDispSize();
//
//    ULONG getSection();
//
//public:
//    typedef std::pair<ULONG, const char *> ValueNameEntry;
//
//    static const ValueNameEntry dataKindName[DataIsConstant + 1];
//
//    static const ValueNameEntry symTagName[SymTagMax];
//
//    static const ValueNameEntry locTypeName[LocTypeMax];
//
//    static const ValueNameEntry basicTypeName[];
//    static const size_t cntBasicTypeName;
//
//    static const ValueNameEntry udtKindName[];
//    static const size_t cntUdtKindName;
//
//    static const ValueNameEntry i386RegName[];
//    static const size_t cntI386RegName;
//
//    static const ValueNameEntry amd64RegName[];
//    static const size_t cntAmd64RegName;
//
//    static std::string getBasicTypeName( ULONG basicType );
//
//protected:
//
//    // Check symbols loop
//    class checkSymLoop
//    {
//    public:
//        checkSymLoop(checkSymLoop *prev)
//            : m_symSetPtr( prev ? prev->m_symSetPtr : symSetPtr(new symSet) )
//        {
//        }
//
//        bool check(IDiaSymbol *_symbol)
//        {
//            DWORD symIndexId = 0;
//            _symbol->get_symIndexId(&symIndexId);
//            return !m_symSetPtr->insert(symIndexId).second;
//        }
//
//    private:
//        typedef std::set<DWORD> symSet;
//        typedef boost::shared_ptr<symSet> symSetPtr;
//        symSetPtr m_symSetPtr;
//    };
//
//    static std::string printImpl(
//        IDiaSymbol *_symbol,
//        DWORD machineType,
//        ULONG indent = 0,
//        checkSymLoop *checkLoopPrev = NULL,
//        const char *prefix = NULL
//    );
//
//    template <typename TRet>
//    TRet callSymbolT(
//        HRESULT(STDMETHODCALLTYPE IDiaSymbol::*method)(TRet *),
//        const char *methodName
//    )
//    {
//        TRet retValue;
//        HRESULT hres = (m_symbol->*method)(&retValue);
//        if (S_OK != hres)
//            throw Exception(std::string("Call IDiaSymbol::") + methodName, hres);
//
//        return retValue;
//    }
//
//    DiaSymbolPtr m_symbol;
//    DWORD m_machineType;
//};
//
//class GlobalScope;
//typedef boost::shared_ptr< GlobalScope > GlobalScopePtr;
//
//////////////////////////////////////////////////////////////////////////////////
//// Global scope: source + sessions
//////////////////////////////////////////////////////////////////////////////////
//class GlobalScope : public Symbol {
//public:
//    GlobalScope() {}
//
//    // GlobalScope factory
//    static GlobalScopePtr loadPdb(const std::string &filePath);
//    static GlobalScopePtr loadExe(const std::string &filePath, PCSTR searchPath = NULL);
//
//    // RVA -> Symbol
//    python::tuple findByRva(
//        ULONG rva,
//        ULONG symTag
//    )
//    {
//        LONG displacement;
//        SymbolPtr child = findByRvaImpl(rva, symTag, displacement);
//        return python::make_tuple(child, displacement);
//    }
//    SymbolPtr findByRvaImpl(
//        __in ULONG rva,
//        __in ULONG symTag,
//        __out LONG &displacement
//    );
//
//    // VA -> Symbol
//    python::tuple findByVa(
//        ULONG va,
//        ULONG symTag
//    )
//    {
//        LONG displacement;
//        SymbolPtr child = findByVaImpl(va, symTag, displacement);
//        return python::make_tuple(child, displacement);
//    }
//    SymbolPtr findByVaImpl(
//        __in ULONGLONG va,
//        __in ULONG symTag,
//        __out LONG &displacement
//    );
//
//    // get symbol by unique index
//    SymbolPtr getSymbolById(ULONG symId);
//
//    // get/set load address
//    ULONGLONG getLoadAddress();
//    void setLoadAddress(ULONGLONG loadAddress);
//
//private:
//    interface IScopeDataLoader {
//        virtual ~IScopeDataLoader() {}
//        virtual void loadData(IDiaDataSource *dataSource) = 0;
//    };
//    static GlobalScopePtr loadImpl(IScopeDataLoader &ScopeDataLoader);
//
//    GlobalScope(
//        __inout DiaDataSourcePtr &dataSource,
//        __inout DiaSessionPtr &_session,
//        __inout DiaSymbolPtr &_globalScope
//    );
//
//    DiaDataSourcePtr m_dataSource;
//    DiaSessionPtr m_session;
//};
//
//////////////////////////////////////////////////////////////////////////////////
//
//template<ULONG symTag>
//ULONG Symbol::getChildCount()
//{
//    DiaEnumSymbolsPtr symbols;
//    HRESULT hres = 
//        m_symbol->findChildren(
//            static_cast<enum SymTagEnum>(symTag),
//            NULL,
//            nsCaseSensitive,
//            &symbols);
//    if (S_OK != hres)
//        throw Exception("Call IDiaSymbol::findChildren", hres);
//
//    LONG count;
//    hres = symbols->get_Count(&count);
//    if (S_OK != hres)
//        throw Exception("Call IDiaEnumSymbols::get_Count", hres);
//
//    return count;
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//template<ULONG symTag>
//SymbolPtr Symbol::getChildByIndex(ULONG _index )
//{
//    DiaEnumSymbolsPtr symbols;
//    HRESULT hres = 
//        m_symbol->findChildren(
//             static_cast<enum SymTagEnum>(symTag),
//            NULL,
//            nsCaseSensitive,
//            &symbols);
//    if (S_OK != hres)
//        throw Exception("Call IDiaSymbol::findChildren", hres);
//
//    LONG count;
//    hres = symbols->get_Count(&count);
//    if (S_OK != hres)
//        throw Exception("Call IDiaEnumSymbols::get_Count", hres);
//
//    if (LONG(_index) >= count)
//    {
//        PyErr_SetString(PyExc_IndexError, "Index out of range");
//        boost::python::throw_error_already_set();
//    }
//
//    DiaSymbolPtr child;
//    hres = symbols->Item(_index, &child);
//    if (S_OK != hres)
//        throw Exception("Call IDiaEnumSymbols::Item", hres);
//
//    return SymbolPtr( new Symbol(child, m_machineType) );
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//};
