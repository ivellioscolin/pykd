
#pragma once
#include "dbgexcept.h"

namespace pyDia {

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

    HRESULT getRes() const {
        return m_hres;
    }

    static void exceptionTranslate(const Exception &e);

    static void setTypeObject(PyObject *p) {
        diaExceptTypeObject = p;
    }

private:

    static PyObject *diaExceptTypeObject;

    static std::string makeFullDesc(const std::string &desc, HRESULT hres);

    HRESULT m_hres;
};

////////////////////////////////////////////////////////////////////////////////
// Symbol
////////////////////////////////////////////////////////////////////////////////
class Symbol {
public:
    Symbol() {}

    python::list findChildren(
        ULONG symTag,
        const std::string &name,
        bool fnMatch
    )
    {
        return 
            findChildrenImpl(
                symTag,
                name,
                fnMatch ? nsCaseSensitive : nsRegularExpression);
    }
    python::list findChildrenNoCase(
        ULONG symTag,
        const std::string &name,
        bool fnMatch
    )
    {
        return 
            findChildrenImpl(
                symTag,
                name,
                fnMatch ? nsCaseInsensitive : nsCaseInRegularExpression);
    }

    ULONGLONG getSize();

    ULONG getSymTag();

    python::object getChildByName(const std::string &_name);
    ULONG getChildCount();
    python::object getChildByIndex(ULONG _index);

protected:

    void throwIfNull(const char *desc)
    {
        if (!m_symbol)
            throw Exception(desc, S_FALSE);
    }

    Symbol(__inout CComPtr< IDiaSymbol > _symbol) {
        m_symbol = _symbol.Detach();
    }

    python::list findChildrenImpl(
        ULONG symTag,
        const std::string &name,
        DWORD nameCmpFlags
    );

    CComPtr< IDiaSymbol > m_symbol;
};

////////////////////////////////////////////////////////////////////////////////
// Global scope: source + sessions
////////////////////////////////////////////////////////////////////////////////
class GlobalScope : public Symbol {
public:
    GlobalScope() {}

    // create GlobalScope instance
    static python::object openPdb(const std::string &filePath);

private:

    GlobalScope(
        __inout CComPtr< IDiaDataSource > &_scope,
        __inout CComPtr< IDiaSession > &_session,
        __inout CComPtr< IDiaSymbol > &_globalScope
    );

    CComPtr< IDiaDataSource > m_source;
    CComPtr< IDiaSession > m_session;
};

////////////////////////////////////////////////////////////////////////////////

};
