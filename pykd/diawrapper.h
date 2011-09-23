
#pragma once

#include <cvconst.h>

#include "utils.h"
#include "dbgexcept.h"

namespace pyDia {

typedef CComPtr< IDiaSymbol > DiaSymbolPtr;
typedef CComPtr< IDiaEnumSymbols > DiaEnumSymbolsPtr;
typedef CComPtr< IDiaDataSource > DiaDataSourcePtr;
typedef CComPtr< IDiaSession > DiaSessionPtr;

////////////////////////////////////////////////////////////////////////////////
// DIA Exceptions
////////////////////////////////////////////////////////////////////////////////
class Exception : public pykd::DbgException {
public:
    Exception(const std::string &desc, HRESULT hres)
        : DbgException( makeFullDesc(desc, hres) )
        , m_hres(hres)
    {
    }

    Exception(const std::string &desc)
        : DbgException(descPrefix + desc)
        , m_hres(S_FALSE)
    {
    }

    HRESULT getRes() const {
        return m_hres;
    }

    static void exceptionTranslate(const Exception &e);

    static void setTypeObject(PyObject *p) {
        diaExceptTypeObject = p;
    }

private:

    static const std::string descPrefix;

    static PyObject *diaExceptTypeObject;

    static std::string makeFullDesc(const std::string &desc, HRESULT hres);

    HRESULT m_hres;
};

////////////////////////////////////////////////////////////////////////////////
// Symbol
////////////////////////////////////////////////////////////////////////////////
class Symbol {
public:
    Symbol()
    {
        throw Exception("DiaSymbol must be created over factory from DiaScope::...");
    }

    std::list< Symbol > findChildrenImpl(
        ULONG symTag,
        const std::string &name,
        DWORD nameCmpFlags
    );

    python::list findChildrenEx(
        ULONG symTag,
        const std::string &name,
        DWORD nameCmpFlags
    )
    {
        return toPyList( findChildrenImpl(symTag, name, nameCmpFlags) );
    }

    python::list findChildren(
        const std::string &name
    )
    {
        return toPyList( findChildrenImpl(SymTagNull, name, nsfCaseSensitive) );
    }

    ULONGLONG getSize();

    std::string getName();

    Symbol getType();

    Symbol getIndexType();

    ULONG getSymTag();

    ULONG getRva();

    ULONG getLocType();

    LONG getOffset();

    static void getValueImpl(IDiaSymbol *_symbol, VARIANT &vtValue);
    python::object getValue();

    bool isBasicType();

    ULONG getBaseType();

    ULONG getBitPosition();

    ULONG getIndexId();

    ULONG getUdtKind();

    ULONG getRegisterId();

    Symbol getChildByName(const std::string &_name);
    ULONG getChildCount();
    Symbol getChildByIndex(ULONG _index);

    std::string print();

public:
    typedef std::pair<ULONG, const char *> ValueNameEntry;

    static const ValueNameEntry symTagName[SymTagMax];

    static const ValueNameEntry locTypeName[LocTypeMax];

    static const ValueNameEntry basicTypeName[];
    static const size_t cntBasicTypeName;

    static const ValueNameEntry udtKindName[];
    static const size_t cntUdtKindName;

    static const ValueNameEntry i386RegName[];
    static const size_t cntI386RegName;

    static const ValueNameEntry amd64RegName[];
    static const size_t cntAmd64RegName;

protected:

    // Check symbols loop
    class checkSymLoop
    {
    public:
        checkSymLoop(checkSymLoop *prev, IDiaSymbol *_symbol) 
            : m_prev(prev)
            , m_symIndexId(0)
        {
            _symbol->get_symIndexId(&m_symIndexId);
        }

        bool check() const
        {
            const checkSymLoop *prev = m_prev;
            while (prev)
            {
                if (prev->m_symIndexId == m_symIndexId)
                    return true;
                prev = prev->m_prev;
            }

            return false;
        }

    private:
        const checkSymLoop *m_prev;
        DWORD m_symIndexId;
    };

    static std::string printImpl(
        IDiaSymbol *_symbol,
        DWORD machineType,
        ULONG indent = 0,
        checkSymLoop *checkLoopPrev = NULL,
        const char *prefix = NULL
    );

    template <typename TRet>
    TRet callSymbolT(
        HRESULT(STDMETHODCALLTYPE IDiaSymbol::*method)(TRet *),
        const char *funcName,
        const char *methodName
    )
    {
        TRet retValue;
        HRESULT hres = (m_symbol->*method)(&retValue);
        if (S_OK != hres)
            throw Exception(std::string("Call IDiaSymbol::") + methodName, hres);

        return retValue;
    }

    Symbol(__inout DiaSymbolPtr &_symbol, DWORD machineType) 
        : m_machineType(machineType)
    {
        m_symbol = _symbol.Detach();
    }

    Symbol(__in IDiaSymbol *_symbol, DWORD machineType) 
        : m_machineType(machineType)
    {
        m_symbol = _symbol;
    }

    DiaSymbolPtr m_symbol;
    DWORD m_machineType;
};

////////////////////////////////////////////////////////////////////////////////
// Global scope: source + sessions
////////////////////////////////////////////////////////////////////////////////
class GlobalScope : public Symbol {
public:
    GlobalScope() {}

    // GlobalScope factory
    static GlobalScope loadPdb(const std::string &filePath);

    ULONG getMachineType() const {
        return m_machineType;
    }

    // RVA -> Symbol
    python::tuple findByRva(
        ULONG rva,
        ULONG symTag
    )
    {
        LONG displacement;
        Symbol child = findByRvaImpl(rva, symTag, displacement);
        return python::make_tuple(child, displacement);
    }
    Symbol findByRvaImpl(
        __in ULONG rva,
        __in ULONG symTag,
        __out LONG &displacement
    );

    // get symbol by unique index
    Symbol getSymbolById(ULONG symId);

private:

    GlobalScope(
        __inout DiaDataSourcePtr &_scope,
        __inout DiaSessionPtr &_session,
        __inout DiaSymbolPtr &_globalScope
    );

    DiaDataSourcePtr m_source;
    DiaSessionPtr m_session;
};

////////////////////////////////////////////////////////////////////////////////

};
