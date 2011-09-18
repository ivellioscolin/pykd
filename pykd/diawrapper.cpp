
#include "stdafx.h"

#include <vector>
#include <memory>

#include "diawrapper.h"
#include "utils.h"

namespace pyDia {

////////////////////////////////////////////////////////////////////////////////

PyObject *Exception::diaExceptTypeObject =  NULL;

const std::string Exception::descPrefix("pyDia: ");

////////////////////////////////////////////////////////////////////////////////

const char *Symbol::symTagName[SymTagMax] = {
    "Null",
    "Exe",
    "Compiland",
    "CompilandDetails",
    "CompilandEnv",
    "Function",
    "Block",
    "Data",
    "Annotation",
    "Label",
    "PublicSymbol",
    "UDT",
    "Enum",
    "FunctionType",
    "PointerType",
    "ArrayType",
    "BaseType",
    "Typedef",
    "BaseClass",
    "Friend",
    "FunctionArgType",
    "FuncDebugStart",
    "FuncDebugEnd",
    "UsingNamespace",
    "VTableShape",
    "VTable",
    "Custom",
    "Thunk",
    "CustomType",
    "ManagedType",
    "Dimension"
};

const char *Symbol::locTypeName[LocTypeMax] = {
    "Null",
    "Static",
    "TLS",
    "RegRel",
    "ThisRel",
    "Enregistered",
    "BitField",
    "Slot",
    "IlRel",
    "InMetaData",
    "Constant"
};

////////////////////////////////////////////////////////////////////////////////

std::string Exception::makeFullDesc(const std::string &desc, HRESULT hres)
{
    std::stringstream sstream;
    sstream << descPrefix << desc << " failed" << std::endl;
    sstream << "Return value is 0x" << std::hex << hres;

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
        sstream << ": " << std::endl;
        sstream << errMessage;
        LocalFree(errMessage);
    }
    else
    {
        sstream << std::endl;
    }

    return sstream.str();
}

////////////////////////////////////////////////////////////////////////////////

void Exception::exceptionTranslate( const Exception &e )
{
    boost::python::object pyExcept(e);

    PyErr_SetObject( diaExceptTypeObject, pyExcept.ptr() );
}

////////////////////////////////////////////////////////////////////////////////

python::list Symbol::findChildrenImpl(
    ULONG symTag,
    const std::string &name,
    DWORD nameCmpFlags
)
{
    throwIfNull(__FUNCTION__);

    DiaEnumSymbolsPtr symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            static_cast<enum SymTagEnum>(symTag),
            toWStr(name),
            nameCmpFlags,
            &symbols);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::findChildren", hres);

    python::list childList;

    DiaSymbolPtr child;
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
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::get_length", hres);

    return retValue;
}

////////////////////////////////////////////////////////////////////////////////

std::string Symbol::getName()
{
    throwIfNull(__FUNCTION__);

    autoBstr bstrName;
    HRESULT hres = m_symbol->get_name(&bstrName);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::get_name", hres);

    return bstrName.asStr();
}

////////////////////////////////////////////////////////////////////////////////

python::object Symbol::getType()
{
    throwIfNull(__FUNCTION__);

    DiaSymbolPtr _type;
    HRESULT hres = m_symbol->get_type(&_type);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::get_type", hres);

    return python::object( Symbol(_type) );
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getSymTag()
{
    throwIfNull(__FUNCTION__);

    DWORD retValue;
    HRESULT hres = m_symbol->get_symTag(&retValue);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::get_symTag", hres);

    return retValue;
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getRva()
{
    throwIfNull(__FUNCTION__);

    DWORD retValue;
    HRESULT hres = m_symbol->get_relativeVirtualAddress(&retValue);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::get_relativeVirtualAddress", hres);

    return retValue;
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getLocType()
{
    throwIfNull(__FUNCTION__);

    DWORD retValue;
    HRESULT hres = m_symbol->get_locationType(&retValue);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::get_locationType", hres);

    return retValue;
}

////////////////////////////////////////////////////////////////////////////////

python::object Symbol::getValue()
{
    throwIfNull(__FUNCTION__);

    VARIANT variant = { VT_EMPTY };
    HRESULT hres = m_symbol->get_value(&variant);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::get_value", hres);

    switch (variant.vt)
    {
    case VT_I1:
    case VT_UI1:
        return python::object( static_cast<ULONG>(variant.bVal) );

    case VT_BOOL:
        return python::object( static_cast<bool>(!!variant.iVal) );

    case VT_I2:
    case VT_UI2:
        return python::object( static_cast<ULONG>(variant.iVal) );

    case VT_I4:
    case VT_UI4:
    case VT_INT:
    case VT_UINT:
    case VT_ERROR:
    case VT_HRESULT:
        return python::object( variant.lVal );

    case VT_I8:
    case VT_UI8:
        return python::object( variant.llVal );

    case VT_R4:
        return python::object( variant.fltVal );

    case VT_R8:
        return python::object( variant.dblVal );

    case VT_BSTR:
        return python::object( autoBstr::asStr(variant.bstrVal).c_str() );

    }
    throw Exception("Unknown value type");
}

////////////////////////////////////////////////////////////////////////////////

python::object Symbol::getChildByName(const std::string &_name)
{
    throwIfNull(__FUNCTION__);

    DiaEnumSymbolsPtr symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            SymTagNull,
            toWStr(_name),
            nsCaseSensitive,
            &symbols);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::findChildren", hres);

    LONG count;
    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw Exception("Call IDiaEnumSymbols::get_Count", hres);

    if (!count)
        throw Exception(_name + " not found");

    if (count != 1)
        throw Exception(_name + " is not unique");

    DiaSymbolPtr child;
    hres = symbols->Item(0, &child);
    if (S_OK != hres)
        throw Exception("Call IDiaEnumSymbols::Item", hres);

    return python::object( Symbol(child) );
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getChildCount()
{
    throwIfNull(__FUNCTION__);

    DiaEnumSymbolsPtr symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            SymTagNull,
            NULL,
            nsCaseSensitive,
            &symbols);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::findChildren", hres);

    LONG count;
    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw Exception("Call IDiaEnumSymbols::get_Count", hres);

    return count;
}

////////////////////////////////////////////////////////////////////////////////

python::object Symbol::getChildByIndex(ULONG _index)
{
    throwIfNull(__FUNCTION__);

    DiaEnumSymbolsPtr symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            SymTagNull,
            NULL,
            nsCaseSensitive,
            &symbols);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::findChildren", hres);

    LONG count;
    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw Exception("Call IDiaEnumSymbols::get_Count", hres);

    if (LONG(_index) >= count)
        throw Exception("Attempt to access non-existing element by index");

    DiaSymbolPtr child;
    hres = symbols->Item(_index, &child);
    if (S_OK != hres)
        throw Exception("Call IDiaEnumSymbols::Item", hres);

    return python::object( Symbol(child) );
}

////////////////////////////////////////////////////////////////////////////////

std::string Symbol::print()
{
    std::stringstream sstream;
    if (m_symbol)
    {
        DWORD dwValue;
        autoBstr bstrValue;
        VARIANT vtValue = { VT_EMPTY };

        sstream << "symTag: ";
        HRESULT hres = m_symbol->get_symTag(&dwValue);
        if ((S_OK == hres) && dwValue < _countof(symTagName))
            sstream << symTagName[dwValue];
        else
            sstream << "<unknown>";
        sstream << ", ";

        hres = m_symbol->get_name(&bstrValue);
        if (S_OK == hres)
            sstream << "\"" << bstrValue.asStr().c_str() << "\"";
        else
            sstream << "<no-name>";
        bstrValue.free();

        hres = m_symbol->get_locationType(&dwValue);
        if ((S_OK == hres) && dwValue < _countof(locTypeName))
        {
            sstream << std::endl;
            sstream << "Location: " << locTypeName[dwValue];
            if (dwValue == LocIsStatic)
            {
                hres = m_symbol->get_relativeVirtualAddress(&dwValue);
                if (S_OK == hres)
                    sstream << ", RVA: 0x" << std::hex << dwValue;
            }
        }

        hres = m_symbol->get_value(&vtValue);
        if (S_OK == hres)
        {
            switch (vtValue.vt)
            {
            case VT_I1:
            case VT_UI1:
                sstream << std::endl << "Value is ";
                sstream << std::hex << "0x" << vtValue.bVal;
                break;

            case VT_BOOL:
                sstream << std::endl << "Value is ";
                sstream << vtValue.iVal ? "True" : "False";
                break;

            case VT_I2:
            case VT_UI2:
                sstream << std::endl << "Value is ";
                sstream << std::hex << "0x" << vtValue.iVal;
                break;

            case VT_I4:
            case VT_UI4:
            case VT_INT:
            case VT_UINT:
            case VT_ERROR:
            case VT_HRESULT:
                sstream << std::endl << "Value is ";
                sstream << std::hex << "0x" << vtValue.lVal;
                break;

            case VT_I8:
            case VT_UI8:
                sstream << std::endl << "Value is ";
                sstream << std::hex << "0x" << vtValue.llVal;
                break;

            case VT_R4:
                sstream << std::endl << "Value is ";
                sstream << vtValue.fltVal;
                break;

            case VT_R8:
                sstream << std::endl << "Value is ";
                sstream << vtValue.dblVal;
                break;

            case VT_BSTR:
                sstream << std::endl << "Value is ";
                sstream << "\"" << autoBstr::asStr(vtValue.bstrVal).c_str() << "\"";
                break;
            }
        }
    }
    return sstream.str();
}

////////////////////////////////////////////////////////////////////////////////

GlobalScope::GlobalScope(
    __inout DiaDataSourcePtr &_scope,
    __inout DiaSessionPtr &_session,
    __inout DiaSymbolPtr &_globalScope
)   : Symbol(_globalScope)
    , m_source( _scope.Detach() )
    , m_session( _session.Detach() )
{
}

////////////////////////////////////////////////////////////////////////////////

python::object GlobalScope::openPdb(const std::string &filePath)
{
    DiaDataSourcePtr _scope;

    HRESULT hres = 
        _scope.CoCreateInstance(__uuidof(DiaSource), NULL, CLSCTX_INPROC_SERVER);
    if ( S_OK != hres )
        throw Exception("Call ::CoCreateInstance", hres);

    hres = _scope->loadDataFromPdb( toWStr(filePath) );
    if ( S_OK != hres )
        throw Exception("Call IDiaDataSource::loadDataFromPdb", hres);

    DiaSessionPtr _session;
    hres = _scope->openSession(&_session);
    if ( S_OK != hres )
        throw Exception("Call IDiaDataSource::openSession", hres);

    DiaSymbolPtr _globalScope;
    hres = _session->get_globalScope(&_globalScope);
    if ( S_OK != hres )
        throw Exception("Call IDiaSymbol::get_globalScope", hres);

    return python::object(GlobalScope(_scope, _session, _globalScope));
}

////////////////////////////////////////////////////////////////////////////////

}
