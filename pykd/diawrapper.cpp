
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

#define _DEF_SYM_TAG_VAL(x) Symbol::ValueNameEntry(SymTag##x, #x)
const Symbol::ValueNameEntry Symbol::symTagName[SymTagMax] = {
    _DEF_SYM_TAG_VAL(Null),
    _DEF_SYM_TAG_VAL(Exe),
    _DEF_SYM_TAG_VAL(Compiland),
    _DEF_SYM_TAG_VAL(CompilandDetails),
    _DEF_SYM_TAG_VAL(CompilandEnv),
    _DEF_SYM_TAG_VAL(Function),
    _DEF_SYM_TAG_VAL(Block),
    _DEF_SYM_TAG_VAL(Data),
    _DEF_SYM_TAG_VAL(Annotation),
    _DEF_SYM_TAG_VAL(Label),
    _DEF_SYM_TAG_VAL(PublicSymbol),
    _DEF_SYM_TAG_VAL(UDT),
    _DEF_SYM_TAG_VAL(Enum),
    _DEF_SYM_TAG_VAL(FunctionType),
    _DEF_SYM_TAG_VAL(PointerType),
    _DEF_SYM_TAG_VAL(ArrayType),
    _DEF_SYM_TAG_VAL(BaseType),
    _DEF_SYM_TAG_VAL(Typedef),
    _DEF_SYM_TAG_VAL(BaseClass),
    _DEF_SYM_TAG_VAL(Friend),
    _DEF_SYM_TAG_VAL(FunctionArgType),
    _DEF_SYM_TAG_VAL(FuncDebugStart),
    _DEF_SYM_TAG_VAL(FuncDebugEnd),
    _DEF_SYM_TAG_VAL(UsingNamespace),
    _DEF_SYM_TAG_VAL(VTableShape),
    _DEF_SYM_TAG_VAL(VTable),
    _DEF_SYM_TAG_VAL(Custom),
    _DEF_SYM_TAG_VAL(Thunk),
    _DEF_SYM_TAG_VAL(CustomType),
    _DEF_SYM_TAG_VAL(ManagedType),
    _DEF_SYM_TAG_VAL(Dimension)
};
#undef _DEF_SYM_TAG_VAL

#define _DEF_LOC_TYPE(x)    Symbol::ValueNameEntry(LocIs##x, #x)
const Symbol::ValueNameEntry Symbol::locTypeName[LocTypeMax] = {
    _DEF_LOC_TYPE(Null),
    _DEF_LOC_TYPE(Static),
    _DEF_LOC_TYPE(TLS),
    _DEF_LOC_TYPE(RegRel),
    _DEF_LOC_TYPE(ThisRel),
    _DEF_LOC_TYPE(Enregistered),
    _DEF_LOC_TYPE(BitField),
    _DEF_LOC_TYPE(Slot),
    _DEF_LOC_TYPE(IlRel),
    Symbol::ValueNameEntry(LocInMetaData, "InMetaData"),
    _DEF_LOC_TYPE(Constant)
};
#undef _DEF_LOC_TYPE

#define _DEF_BASIC_TYPE(x)  Symbol::ValueNameEntry(bt##x, #x)
const Symbol::ValueNameEntry Symbol::basicTypeName[] = {
    _DEF_BASIC_TYPE(NoType),
    _DEF_BASIC_TYPE(Void),
    _DEF_BASIC_TYPE(Char),
    _DEF_BASIC_TYPE(WChar),
    _DEF_BASIC_TYPE(Int),
    _DEF_BASIC_TYPE(UInt),
    _DEF_BASIC_TYPE(Float),
    _DEF_BASIC_TYPE(BCD),
    _DEF_BASIC_TYPE(Bool),
    _DEF_BASIC_TYPE(Long),
    _DEF_BASIC_TYPE(ULong),
    _DEF_BASIC_TYPE(Currency),
    _DEF_BASIC_TYPE(Date),
    _DEF_BASIC_TYPE(Variant),
    _DEF_BASIC_TYPE(Complex),
    _DEF_BASIC_TYPE(Bit),
    _DEF_BASIC_TYPE(BSTR),
    _DEF_BASIC_TYPE(Hresult)
};
#undef _DEF_BASIC_TYPE

const size_t Symbol::cntBasicTypeName = _countof(Symbol::basicTypeName);

////////////////////////////////////////////////////////////////////////////////

#define callSymbol(retType, method)                         \
do {                                                        \
    throwIfNull(__FUNCTION__);                              \
    retType retValue;                                       \
    HRESULT hres = m_symbol->##method(&retValue);           \
    if (FAILED(hres))                                       \
        throw Exception("Call IDiaSymbol::" #method, hres); \
    return retValue;                                        \
} while(false)

#define callSymbolDword(method) callSymbol(DWORD, method)
#define callSymbolUlonglong(method) callSymbol(ULONGLONG, method)

#define callSymbolObjectFromSymbol(method)                  \
do {                                                        \
    throwIfNull(__FUNCTION__);                              \
    DiaSymbolPtr retSymbol;                                 \
    HRESULT hres = m_symbol->##method(&retSymbol);          \
    if (S_OK != hres)                                       \
        throw Exception("Call IDiaSymbol::" #method, hres); \
    return python::object( Symbol(retSymbol) );             \
} while(false)

#define callSymbolStr(method)                               \
do {                                                        \
    throwIfNull(__FUNCTION__);                              \
    autoBstr bstrName;                                      \
    HRESULT hres = m_symbol->##method(&bstrName);           \
    if (S_OK != hres)                                       \
        throw Exception("Call IDiaSymbol" #method, hres);   \
    return bstrName.asStr();                                \
} while(false)

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
    callSymbolUlonglong(get_length);
}

////////////////////////////////////////////////////////////////////////////////

std::string Symbol::getName()
{
    callSymbolStr(get_name);
}

////////////////////////////////////////////////////////////////////////////////

python::object Symbol::getType()
{
    callSymbolObjectFromSymbol(get_type);
}

////////////////////////////////////////////////////////////////////////////////

python::object Symbol::getIndexType()
{
    callSymbolObjectFromSymbol(get_arrayIndexType);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getSymTag()
{
    callSymbolDword(get_symTag);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getRva()
{
    callSymbolDword(get_relativeVirtualAddress);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getLocType()
{
    callSymbolDword(get_locationType);
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

bool Symbol::isBasicType()
{
    throwIfNull(__FUNCTION__);

    DWORD baseType = btNoType;
    return 
        SUCCEEDED( m_symbol->get_baseType(&baseType) ) && 
        (btNoType != baseType);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getBaseType()
{
    callSymbolDword(get_baseType);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getBitPosition()
{
    callSymbolDword(get_bitPosition);
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
            sstream << symTagName[dwValue].second;
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
            sstream << "Location: " << locTypeName[dwValue].second;
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

        hres = m_symbol->get_baseType(&dwValue);
        if (SUCCEEDED(hres) && btNoType != dwValue)
        {
            for (ULONG i = 0; i < cntBasicTypeName; ++i)
            {
                if (basicTypeName[i].first == dwValue)
                {
                    sstream << std::endl;
                    sstream << "Basic type is " << basicTypeName[i].second;
                    break;
                }
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
