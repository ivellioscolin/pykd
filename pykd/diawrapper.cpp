
#include "stdafx.h"

#include "diawrapper.h"
#include "utils.h"

namespace pyDia {

////////////////////////////////////////////////////////////////////////////////

PyObject *Exception::diaExceptTypeObject =  NULL;

const std::string Exception::descPrefix("pyDia: ");

////////////////////////////////////////////////////////////////////////////////

#define callSymbol(method) \
    callSymbolT( &IDiaSymbol::##method, __FUNCTION__, #method)

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

std::list< Symbol > Symbol::findChildrenImpl(
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

    std::list< Symbol > childList;

    DiaSymbolPtr child;
    ULONG celt;
    while ( SUCCEEDED(symbols->Next(1, &child, &celt)) && (celt == 1) )
        childList.push_back( Symbol(child, m_machineType) );

    return childList;
}

////////////////////////////////////////////////////////////////////////////////

ULONGLONG Symbol::getSize()
{
    return callSymbol(get_length);
}

////////////////////////////////////////////////////////////////////////////////

std::string Symbol::getName()
{
    autoBstr retValue( callSymbol(get_name) );
    return retValue.asStr();
}

////////////////////////////////////////////////////////////////////////////////

Symbol Symbol::getType()
{
    return Symbol( callSymbol(get_type), m_machineType );
}

////////////////////////////////////////////////////////////////////////////////

Symbol Symbol::getIndexType()
{
    return Symbol( callSymbol(get_arrayIndexType), m_machineType );
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getSymTag()
{
    return callSymbol(get_symTag);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getRva()
{
    return callSymbol(get_relativeVirtualAddress);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getLocType()
{
    return callSymbol(get_locationType);
}

////////////////////////////////////////////////////////////////////////////////

LONG Symbol::getOffset()
{
    return callSymbol(get_offset);
}

////////////////////////////////////////////////////////////////////////////////
void Symbol::getValueImpl(IDiaSymbol *_symbol, VARIANT &vtValue)
{
    if (!_symbol)
        throw Exception(std::string(__FUNCTION__) + " failed, DIA object is not initialized");

    HRESULT hres = _symbol->get_value(&vtValue);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::get_value", hres);
}

////////////////////////////////////////////////////////////////////////////////

python::object Symbol::getValue()
{
    VARIANT vtValue = { VT_EMPTY };
    getValueImpl(m_symbol, vtValue);
    switch (vtValue.vt)
    {
    case VT_I1:
    case VT_UI1:
        return python::object( static_cast<ULONG>(vtValue.bVal) );

    case VT_BOOL:
        return python::object( static_cast<bool>(!!vtValue.iVal) );

    case VT_I2:
    case VT_UI2:
        return python::object( static_cast<ULONG>(vtValue.iVal) );

    case VT_I4:
    case VT_UI4:
    case VT_INT:
    case VT_UINT:
    case VT_ERROR:
    case VT_HRESULT:
        return python::object( vtValue.lVal );

    case VT_I8:
    case VT_UI8:
        return python::object( float(vtValue.llVal) );

    case VT_R4:
        return python::object( double(vtValue.fltVal) );

    case VT_R8:
        return python::object( vtValue.dblVal );

    case VT_BSTR:
        return python::object( autoBstr::asStr(vtValue.bstrVal).c_str() );

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
    return callSymbol(get_baseType);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getBitPosition()
{
    return callSymbol(get_bitPosition);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getIndexId()
{
    return callSymbol(get_symIndexId);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getUdtKind()
{
    return callSymbol(get_udtKind);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getRegisterId()
{
    return callSymbol(get_registerId);
}

////////////////////////////////////////////////////////////////////////////////

Symbol Symbol::getChildByName(const std::string &_name)
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

    return Symbol(child, m_machineType);
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

Symbol Symbol::getChildByIndex(ULONG _index)
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
    {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        boost::python::throw_error_already_set();
    }

    DiaSymbolPtr child;
    hres = symbols->Item(_index, &child);
    if (S_OK != hres)
        throw Exception("Call IDiaEnumSymbols::Item", hres);

    return Symbol(child, m_machineType);
}

////////////////////////////////////////////////////////////////////////////////

std::string Symbol::print()
{
    return printImpl(m_symbol, m_machineType);
}

////////////////////////////////////////////////////////////////////////////////

std::string Symbol::printImpl(IDiaSymbol *_symbol, DWORD machineType, ULONG indent /*= 0*/)
{
    std::stringstream sstream;
    for (ULONG i =0; i < indent; ++i)
        sstream << " ";
    if (_symbol)
    {
        DWORD dwValue;
        autoBstr bstrValue;
        VARIANT vtValue = { VT_EMPTY };
        bool bValue;
        LONG lValue;
        ULONGLONG ullValue;
        HRESULT hres;

        DWORD locType = LocIsNull;
        hres = _symbol->get_locationType(&locType);
        bool bLocation = (S_OK == hres);
        if (bLocation)
        {
            hres = _symbol->get_offset(&lValue);

            const bool bNegOffset = lValue < 0;
            lValue = bNegOffset ? -1 * lValue : lValue;

            switch (locType)
            {
            case LocIsBitField:
            case LocIsThisRel:
                assert(S_OK == hres);
                sstream << (bNegOffset ? "-" : "+") << "0x" << std::hex << lValue;
                if (LocIsBitField == locType)
                {
                    hres = _symbol->get_bitPosition(&dwValue);
                    if (S_OK == hres)
                        sstream << ", Bit position: " << dwValue;
                }
                break;

            case LocIsEnregistered:
            case LocIsRegRel:
                hres = _symbol->get_registerId(&dwValue);
                if (S_OK == hres)
                {
                    const char *regName = NULL;
                    if (IMAGE_FILE_MACHINE_I386 == machineType)
                    {
                        for (ULONG i = 0; i < cntI386RegName; ++i)
                        {
                            if (dwValue == i386RegName[i].first)
                            {
                                regName = i386RegName[i].second;
                                break;
                            }
                        }
                    }
                    else if (IMAGE_FILE_MACHINE_AMD64 == machineType)
                    {
                        for (ULONG i = 0; i < cntI386RegName; ++i)
                        {
                            if (dwValue == i386RegName[i].first)
                            {
                                regName = i386RegName[i].second;
                                break;
                            }
                        }
                    }
                    if (!regName)
                    {
                        sstream << locTypeName[locType].second;
                    }
                    else
                    {
                        if (LocIsEnregistered == locType)
                        {
                            sstream << regName;
                        }
                        else
                        {
                            sstream << "[" << regName;
                            sstream << (bNegOffset ? "-" : "+") << "0x" << std::hex << lValue;
                            sstream << "]";
                        }
                    }
                }
                else
                {
                    sstream << locTypeName[locType].second;
                }
                break;

            default:
                if (S_OK == _symbol->get_relativeVirtualAddress(&dwValue))
                    sstream << "RVA:0x" << std::hex << dwValue;
                else if (locType < _countof(locTypeName))
                    sstream << "Location: " << locTypeName[locType].second;
                if (S_OK == hres)
                {
                    sstream << ", Offset: ";
                    sstream << (bNegOffset ? "-" : "+") << "0x" << std::hex << lValue;
                }
                break;
            }
        }

        hres = _symbol->get_symTag(&dwValue);
        if ((S_OK == hres) && dwValue < _countof(symTagName))
        {
            if (bLocation)
                sstream << ", ";

            sstream << symTagName[dwValue].second;
            if (SymTagUDT == symTagName[dwValue].first)
            {
                hres = _symbol->get_udtKind(&dwValue);
                if ((S_OK == hres) && (dwValue < cntUdtKindName))
                    sstream << ": " << udtKindName[dwValue].second;
            }
        }
        else
        {
            sstream << "!invalid symTag!";
        }
        sstream << ", ";

        hres = _symbol->get_name(&bstrValue);
        if (S_OK == hres)
            sstream << "\"" << bstrValue.asStr().c_str() << "\"";
        else
            sstream << "<no-name>";
        bstrValue.free();

        hres = _symbol->get_length(&ullValue);
        if (S_OK == hres)
            sstream << ", Length: 0x" << std::hex << ullValue;

        bValue = false;
        try
        {
            getValueImpl(_symbol, vtValue);
            bValue = true;
        }
        catch (const Exception &except)
        {
            DBG_UNREFERENCED_PARAMETER(except);
        }
        if (bValue)
        {
            switch (vtValue.vt)
            {
            case VT_I1:
            case VT_UI1:
                sstream << ", Value: ";
                sstream << "0x" << std::hex << vtValue.bVal;
                break;

            case VT_BOOL:
                sstream << ", Value: ";
                sstream << vtValue.iVal ? "True" : "False";
                break;

            case VT_I2:
            case VT_UI2:
                sstream << ", Value: ";
                sstream << "0x" << std::hex << vtValue.iVal;
                break;

            case VT_I4:
            case VT_UI4:
            case VT_INT:
            case VT_UINT:
            case VT_ERROR:
            case VT_HRESULT:
                sstream << ", Value: ";
                sstream << "0x" << std::hex << vtValue.lVal;
                break;

            case VT_I8:
            case VT_UI8:
                sstream << ", Value: ";
                sstream << "0x" << std::hex << vtValue.llVal;
                break;

            case VT_R4:
                sstream << ", Value: ";
                sstream << vtValue.fltVal;
                break;

            case VT_R8:
                sstream << ", Value: ";
                sstream << vtValue.dblVal;
                break;

            case VT_BSTR:
                sstream << ", Value: ";
                sstream << "\"" << autoBstr::asStr(vtValue.bstrVal).c_str() << "\"";
                break;
            }
        }

        hres = _symbol->get_baseType(&dwValue);
        if (SUCCEEDED(hres) && btNoType != dwValue)
        {
            for (ULONG i = 0; i < cntBasicTypeName; ++i)
            {
                if (basicTypeName[i].first == dwValue)
                {
                    sstream << ", Basic type: " << basicTypeName[i].second;
                    break;
                }
            }
        }

        DWORD dwThisSymbol = 0;
        hres = _symbol->get_symIndexId(&dwThisSymbol);
        assert(S_OK == hres);
        if (S_OK == hres)
        {
            DiaSymbolPtr pType;
            hres = _symbol->get_type(&pType);
            if (S_OK == hres)
            {
                DWORD dwTypeSymbol;
                hres = pType->get_symIndexId(&dwTypeSymbol);
                if ((S_OK == hres) && (dwTypeSymbol != dwThisSymbol))
                {
                    sstream << std::endl;
                    for (ULONG i =0; i < indent; ++i)
                        sstream << " ";
                    sstream << "Type: " << std::endl;
                    sstream << printImpl(pType, machineType, indent + 1).c_str();
                }
            }
        }

        DiaEnumSymbolsPtr symbols;
        hres = 
            _symbol->findChildren(
                SymTagNull,
                NULL,
                nsCaseSensitive,
                &symbols);
        if (S_OK == hres)
        {
            if (indent <= 2)
            {
                DiaSymbolPtr child;
                ULONG celt;
                while ( SUCCEEDED(symbols->Next(1, &child, &celt)) && (celt == 1) )
                {
                    sstream << std::endl << printImpl(child, machineType, indent + 1).c_str();
                    child.Release();
                }
            }
            else
            {
                lValue = 0;
                symbols->get_Count(&lValue);
                if (lValue)
                {
                    sstream << std::endl;
                    for (ULONG i =0; i < indent+1; ++i)
                        sstream << " ";
                    sstream << "<...>";
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
)   : Symbol(_globalScope, CV_CFL_80386)
    , m_source( _scope.Detach() )
    , m_session( _session.Detach() )
{
    m_symbol->get_machineType(&m_machineType);
}

////////////////////////////////////////////////////////////////////////////////

GlobalScope GlobalScope::openPdb(const std::string &filePath)
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

    return GlobalScope(_scope, _session, _globalScope);
}

////////////////////////////////////////////////////////////////////////////////

}
