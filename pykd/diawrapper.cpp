
#include "stdafx.h"

#include <strstream>
#include <vector>
#include <memory>

#include "diawrapper.h"

using namespace pykd;

namespace pyDia {

////////////////////////////////////////////////////////////////////////////////

PyObject *Exception::diaExceptTypeObject =  NULL;

////////////////////////////////////////////////////////////////////////////////

std::string Exception::makeFullDesc(const std::string &desc, HRESULT hres)
{
    std::strstream res;
    res << "pyDia: " << desc << " failed" << std::endl;
    res << "Return value is 0x" << std::hex << hres;

    PCHAR errMessage = NULL;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        hres,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (PCHAR)&errMessage,
        0,
        NULL);
    if (errMessage)
    {
        res << ":" << std::endl;
        res << errMessage;
        LocalFree(errMessage);
    }
    else
    {
        res << std::endl;
    }

    return res.str();
}

////////////////////////////////////////////////////////////////////////////////

void Exception::exceptionTranslate( const Exception &e )
{
    boost::python::object pyExcept(e);

    PyErr_SetObject( diaExceptTypeObject, pyExcept.ptr() );
}

////////////////////////////////////////////////////////////////////////////////
// Convert to OLESTR helper
////////////////////////////////////////////////////////////////////////////////
class toOleStr {
public:
    toOleStr(const std::string &sz)
    {
        m_buf.resize( sz.size() + 1, L'\0' );
        ::MultiByteToWideChar( CP_ACP, 0, sz.c_str(), sz.size(), &m_buf[0], m_buf.size() );
    }

    operator const OLECHAR *() const {
        return m_buf.empty() ? NULL : &m_buf[0];
    }

private:
    std::vector<OLECHAR> m_buf;
};

////////////////////////////////////////////////////////////////////////////////

python::list Symbol::findChildrenImpl(
    ULONG symTag,
    const std::string &name,
    DWORD nameCmpFlags
)
{
    throwIfNull(__FUNCTION__);

    CComPtr< IDiaEnumSymbols > symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            static_cast<enum SymTagEnum>(symTag),
            toOleStr(name),
            nameCmpFlags,
            &symbols);
    if (FAILED(hres))
        throw Exception("Get list of children", hres);

    python::list childList;

    CComPtr< IDiaSymbol > child;
    ULONG celt;
    while ( SUCCEEDED(symbols->Next(1, &child, &celt)) && (celt == 1) )
        childList.append( Symbol(child) );

    return childList;
}

////////////////////////////////////////////////////////////////////////////////

ULONGLONG Symbol::getSize()
{
    throwIfNull(__FUNCTION__);

    ULONGLONG retValue;
    HRESULT hres = m_symbol->get_length(&retValue);
    if (FAILED(hres))
        throw Exception("Get length", hres);

    return retValue;
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getSymTag()
{
    throwIfNull(__FUNCTION__);

    DWORD retValue;
    HRESULT hres = m_symbol->get_symTag(&retValue);
    if (FAILED(hres))
        throw Exception("Get symbol type", hres);

    return retValue;
}

////////////////////////////////////////////////////////////////////////////////

python::object Symbol::getChildByName(const std::string &_name)
{
    throwIfNull(__FUNCTION__);

    CComPtr< IDiaEnumSymbols > symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            SymTagNull,
            toOleStr(_name),
            nsCaseSensitive,
            &symbols);
    if (FAILED(hres))
        throw Exception("Get child by name", hres);

    LONG count;
    hres = symbols->get_Count(&count);
    if (FAILED(hres))
        throw Exception("Get count of children", hres);

    if (count != 1)
        throw Exception("Query unique child", S_FALSE);

    CComPtr< IDiaSymbol > child;
    hres = symbols->Item(0, &child);
    if (FAILED(hres))
        throw Exception("Build child object", hres);

    return python::object( Symbol(child) );
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getChildCount()
{
    throwIfNull(__FUNCTION__);

    CComPtr< IDiaEnumSymbols > symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            SymTagNull,
            NULL,
            nsCaseSensitive,
            &symbols);
    if (FAILED(hres))
        throw Exception("Get child count", hres);

    LONG count;
    hres = symbols->get_Count(&count);
    if (FAILED(hres))
        throw Exception("Get count of count", hres);

    return count;
}

////////////////////////////////////////////////////////////////////////////////

python::object Symbol::getChildByIndex(ULONG _index)
{
    throwIfNull(__FUNCTION__);

    CComPtr< IDiaEnumSymbols > symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            SymTagNull,
            NULL,
            nsCaseSensitive,
            &symbols);
    if (FAILED(hres))
        throw Exception("Get child by index", hres);

    LONG count;
    hres = symbols->get_Count(&count);
    if (FAILED(hres))
        throw Exception("Get count of children", hres);

    if (LONG(_index) >= count)
        throw Exception("Check child index", S_FALSE);

    CComPtr< IDiaSymbol > child;
    hres = symbols->Item(_index, &child);
    if (FAILED(hres))
        throw Exception("Build child object", hres);

    return python::object( Symbol(child) );
}

////////////////////////////////////////////////////////////////////////////////

GlobalScope::GlobalScope(
    __inout CComPtr< IDiaDataSource > &_scope,
    __inout CComPtr< IDiaSession > &_session,
    __inout CComPtr< IDiaSymbol > &_globalScope
)   : Symbol(_globalScope)
    , m_source( _scope.Detach() )
    , m_session( _session.Detach() )
{
}

////////////////////////////////////////////////////////////////////////////////

python::object GlobalScope::openPdb(const std::string &filePath)
{
    CComPtr< IDiaDataSource > _scope;

    HRESULT hres = 
        _scope.CoCreateInstance(__uuidof(DiaSource), NULL, CLSCTX_INPROC_SERVER);
    if ( FAILED(hres) )
        throw Exception("Create scope instance", hres);

    hres = _scope->loadDataFromPdb( toOleStr(filePath) );
    if ( FAILED(hres) )
        throw Exception("Load pdb file", hres);

    CComPtr< IDiaSession > _session;
    hres = _scope->openSession(&_session);
    if ( FAILED(hres) )
        throw Exception("Open session for querying symbols", hres);

    CComPtr< IDiaSymbol > _globalScope;
    hres = _session->get_globalScope(&_globalScope);
    if ( FAILED(hres) )
        throw Exception("Retrieves a reference to the global scope", hres);

    return python::object(GlobalScope(_scope, _session, _globalScope));
}

////////////////////////////////////////////////////////////////////////////////

}
