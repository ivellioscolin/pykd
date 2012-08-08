
#include "stdafx.h"
#include "dia/diawrapper.h"
#include "win/utils.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

const std::string DiaException::descPrefix("pyDia: ");

std::string DiaException::makeFullDesc(const std::string &desc, HRESULT hres)
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

#define callSymbol(method) \
    callSymbolT( &IDiaSymbol::##method, #method)

////////////////////////////////////////////////////////////////////////////////

SymbolSessionPtr  loadSymbolFile(const std::string &filePath, ULONGLONG loadBase )
{
    HRESULT hres;
    DiaDataSourcePtr dataSource;

    hres = dataSource.CoCreateInstance(__uuidof(DiaSource), NULL, CLSCTX_INPROC_SERVER);

    if ( S_OK != hres )
        throw DiaException("Call ::CoCreateInstance", hres);

    hres = dataSource->loadDataFromPdb( toWStr(filePath) );
    if ( S_OK != hres )
        throw DiaException("Call IDiaDataSource::loadDataFromPdb", hres);

    DiaSessionPtr _session;
    hres = dataSource->openSession(&_session);
    if ( S_OK != hres )
        throw DiaException("Call IDiaDataSource::openSession", hres);

    hres = _session->put_loadAddress(loadBase);
    if (S_OK != hres)
        throw DiaException("Call IDiaSession::put_loadAddress", hres);

    DiaSymbolPtr _globalScope;
    hres = _session->get_globalScope(&_globalScope);
    if ( S_OK != hres )
        throw DiaException("Call IDiaSymbol::get_globalScope", hres);

    return SymbolSessionPtr( new DiaSession( _session, _globalScope ) );
}

//////////////////////////////////////////////////////////////////////////////////

std::string getBasicTypeName( ULONG basicType )
{
    for ( size_t i = 0; i < DiaSymbol::cntBasicTypeName; ++i )
    {
        if ( basicType == DiaSymbol::basicTypeName[i].first )
            return std::string( DiaSymbol::basicTypeName[i].second );
    }

    std::stringstream   sstr;

    sstr << "faild to find basic type with index %d" << basicType;

    throw DiaException( sstr.str() );
}

////////////////////////////////////////////////////////////////////////////////

DiaSymbol::DiaSymbol(__inout DiaSymbolPtr &_symbol )
{
    m_symbol = _symbol;
    m_symbol->get_machineType(&m_machineType);
}

//////////////////////////////////////////////////////////////////////////////////

DiaSymbol::DiaSymbol( IDiaSymbol *_symbol )
{
    m_symbol = _symbol;
    m_symbol->get_machineType(&m_machineType);
}

//////////////////////////////////////////////////////////////////////////////////

SymbolPtrList  DiaSymbol::findChildren(
        ULONG symTag,
        const std::string &name,
        bool caseSensitive
    )
{
    DiaEnumSymbolsPtr symbols;
    HRESULT hres;

    if ( name.empty() )
    {
        hres = m_symbol->findChildren(
            static_cast<enum ::SymTagEnum>(symTag),
                NULL,
                caseSensitive ? nsCaseSensitive : nsCaseInsensitive,
                &symbols);

    }
    else
    {
        hres = m_symbol->findChildren(
            static_cast<enum ::SymTagEnum>(symTag),
                toWStr(name),
                 caseSensitive ? nsCaseSensitive : nsCaseInsensitive,
                &symbols);
    }

    if (S_OK != hres)
        throw DiaException("Call IDiaSymbol::findChildren", hres);

    SymbolPtrList childList;

    DiaSymbolPtr child;
    ULONG celt;
    while ( SUCCEEDED(symbols->Next(1, &child, &celt)) && (celt == 1) )
    {
        childList.push_back( SymbolPtr( new DiaSymbol(child) ) );
        child = NULL;
    }

    return childList;
}

//////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getBaseType()
{
    return callSymbol(get_baseType);
}

//////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getBitPosition()
{
    return callSymbol(get_bitPosition);
}

//////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getChildCount(ULONG symTag)
{
    DiaEnumSymbolsPtr symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            static_cast<enum ::SymTagEnum>(symTag),
            NULL,
            nsCaseSensitive,
            &symbols);
    if (S_OK != hres)
        throw DiaException("Call IDiaSymbol::findChildren", hres);

    LONG count;
    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::get_Count", hres);

    return count;
}

////////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSymbol::getChildByIndex(ULONG symTag, ULONG _index )
{
    DiaEnumSymbolsPtr symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            static_cast<enum ::SymTagEnum>(symTag),
            NULL,
            nsCaseSensitive,
            &symbols);
    if (S_OK != hres)
        throw DiaException("Call IDiaSymbol::findChildren", hres);

    LONG count;
    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::get_Count", hres);

    if (LONG(_index) >= count)
    {
        throw PyException( PyExc_IndexError, "Index out of range");
    }

    DiaSymbolPtr child;
    hres = symbols->Item(_index, &child);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::Item", hres);

    return SymbolPtr( new DiaSymbol(child) );
}

////////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSymbol::getChildByName(const std::string &_name)
{
    DiaEnumSymbolsPtr symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            ::SymTagNull,
            toWStr(_name),
            nsCaseSensitive,
            &symbols);

    if (S_OK != hres)
        throw DiaException("Call IDiaSymbol::findChildren", hres);

    LONG count;
    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::get_Count", hres);

    if (!count)
        throw DiaException(_name + " not found");

    if (count != 1)
        throw DiaException(_name + " is not unique");

    DiaSymbolPtr child;
    hres = symbols->Item(0, &child);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::Item", hres);

    return SymbolPtr( new DiaSymbol(child) );
}

//////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getCount()
{
    return callSymbol(get_count);
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getDataKind()
{
    return callSymbol(get_dataKind);
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getLocType()
{
    return callSymbol(get_locationType);
}

//////////////////////////////////////////////////////////////////////////////

std::string DiaSymbol::getName()
{
    autoBstr retValue( callSymbol(get_name) );
    return retValue.asStr();
}
////////////////////////////////////////////////////////////////////////////////

LONG DiaSymbol::getOffset()
{
    return callSymbol(get_offset);
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getRva()
{
    return callSymbol(get_relativeVirtualAddress);
}

////////////////////////////////////////////////////////////////////////////////

ULONGLONG DiaSymbol::getSize()
{
    return callSymbol(get_length);
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getSymTag()
{
    return callSymbol(get_symTag);
}

////////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSymbol::getType()
{
    return SymbolPtr( new DiaSymbol( callSymbol(get_type) ) );
}

////////////////////////////////////////////////////////////////////////////////

ULONGLONG DiaSymbol::getVa()
{
    return callSymbol(get_virtualAddress);
}

////////////////////////////////////////////////////////////////////////////////

void DiaSymbol::getValue( BaseTypeVariant &btv )
{
    CComVariant  vtValue;
    HRESULT hres = m_symbol->get_value(&vtValue);
    if (S_OK != hres)
        throw DiaException("Call IDiaSymbol::get_value", hres);

    switch (vtValue.vt)
    {
    case VT_I1:
        btv = (LONG)vtValue.bVal;
        break;

    case VT_UI1:
        btv = (ULONG)vtValue.bVal;
        break;

    case VT_BOOL:
        btv = !!vtValue.iVal;
        break;

    case VT_I2:
        btv = (LONG)vtValue.iVal;
        break;

    case VT_UI2:
        btv = (ULONG)vtValue.iVal;
        break;

    case VT_I4:
    case VT_INT:
        btv = (LONG)vtValue.lVal;
        break;

    case VT_UI4:
    case VT_UINT:
    case VT_ERROR:
    case VT_HRESULT:
        btv = (ULONG)vtValue.lVal;
        break;

    case VT_I8:
        btv = (ULONG64)vtValue.llVal;
        break;

    case VT_UI8:
        btv = (LONG64)vtValue.llVal;
        break;

    //case VT_R4:
    //    btv = vtValue.fltVal;
    //    break;

    //case VT_R8:
    //    fillDataBuff(vtValue.dblVal);
    //    break;

    default:
        throw DbgException( "Unsupported const value" );
    }
}

//////////////////////////////////////////////////////////////////////////////

int DiaSymbol::getVirtualBasePointerOffset()
{
    return callSymbol(get_virtualBasePointerOffset);
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getVirtualBaseDispIndex()
{
    return callSymbol(get_virtualBaseDispIndex);
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getVirtualBaseDispSize()
{
   SymbolPtr   baseTableType = SymbolPtr( new DiaSymbol( callSymbol(get_virtualBaseTableType) ) );

   return (ULONG)baseTableType->getType()->getSize();
}

//////////////////////////////////////////////////////////////////////////////

bool DiaSymbol::isVirtualBaseClass()
{
    return !!callSymbol(get_virtualBaseClass);
}

//////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSession::findByRva( ULONG rva, ULONG symTag, LONG* pdisplacement )
{
    DiaSymbolPtr child;
    LONG displacement;

    HRESULT hres = 
        m_session->findSymbolByRVAEx(
            rva,
            static_cast<enum ::SymTagEnum>(symTag),
            &child,
            &displacement);

    if (S_OK != hres)
        throw DiaException("Call IDiaSession::findSymbolByRVAEx", hres);
    if (!child)
        throw DiaException("Call IDiaSession::findSymbolByRVAEx", E_UNEXPECTED);
    if ( pdisplacement == NULL && displacement != 0 )
        throw DiaException("Call IDiaSession::findSymbolByRVAEx failed to find suymbol" );

    return SymbolPtr( new DiaSymbol(child) );
}

//////////////////////////////////////////////////////////////////////////////

}; // pykd nemaspace end




































//
//#include "diawrapper.h"
//#include "diacallback.h"
//#include "utils.h"
//
//namespace pyDia {
//
//////////////////////////////////////////////////////////////////////////////////
//
////PyObject *Exception::diaExceptTypeObject =  NULL;
//
//const std::string Exception::descPrefix("pyDia: ");
//
//////////////////////////////////////////////////////////////////////////////////
//
//#define callSymbol(method) \
//    callSymbolT( &IDiaSymbol::##method, #method)
//
//////////////////////////////////////////////////////////////////////////////////
//
//std::string Exception::makeFullDesc(const std::string &desc, HRESULT hres)
//{
//    std::stringstream sstream;
//    sstream << descPrefix << desc << " failed" << std::endl;
//    sstream << "Return value is 0x" << std::hex << hres;
//
//    PCHAR errMessage = NULL;
//    FormatMessageA(
//        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//        NULL,
//        hres,
//        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//        (PCHAR)&errMessage,
//        0,
//        NULL);
//    if (errMessage)
//    {
//        sstream << ": " << std::endl;
//        sstream << errMessage;
//        LocalFree(errMessage);
//    }
//    else
//    {
//        sstream << std::endl;
//    }
//
//    return sstream.str();
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//SymbolPtrList Symbol::findChildrenImpl(
//    ULONG symTag,
//    const std::string &name,
//    DWORD nameCmpFlags
//)
//{
//    DiaEnumSymbolsPtr symbols;
//    HRESULT hres;
//
//    if ( name.empty() )
//    {
//        hres = m_symbol->findChildren(
//                static_cast<enum SymTagEnum>(symTag),
//                NULL,
//                nameCmpFlags,
//                &symbols);
//
//    }
//    else
//    {
//        hres = m_symbol->findChildren(
//                static_cast<enum SymTagEnum>(symTag),
//                toWStr(name),
//                nameCmpFlags,
//                &symbols);
//    }
//
//    if (S_OK != hres)
//        throw Exception("Call IDiaSymbol::findChildren", hres);
//
//    SymbolPtrList childList;
//
//    DiaSymbolPtr child;
//    ULONG celt;
//    while ( SUCCEEDED(symbols->Next(1, &child, &celt)) && (celt == 1) )
//        childList.push_back( SymbolPtr( new Symbol(child, m_machineType) ) );
//
//    return childList;
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONGLONG Symbol::getSize()
//{
//    return callSymbol(get_length);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//std::string Symbol::getName()
//{
//    autoBstr retValue( callSymbol(get_name) );
//    return retValue.asStr();
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//std::string Symbol::getUndecoratedName()
//{
//    autoBstr retValue( callSymbol(get_undecoratedName) );
//    return retValue.asStr();
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//SymbolPtr Symbol::getType()
//{
//    return SymbolPtr( new Symbol(callSymbol(get_type), m_machineType) );
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//SymbolPtr Symbol::getIndexType()
//{
//    return SymbolPtr( new Symbol(callSymbol(get_arrayIndexType), m_machineType) );
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONG Symbol::getSymTag()
//{
//    return callSymbol(get_symTag);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONG Symbol::getRva()
//{
//    return callSymbol(get_relativeVirtualAddress);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONGLONG Symbol::getVa()
//{
//    return callSymbol(get_virtualAddress);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONG Symbol::getLocType()
//{
//    return callSymbol(get_locationType);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//LONG Symbol::getOffset()
//{
//    return callSymbol(get_offset);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONG Symbol::getCount()
//{
//    return callSymbol(get_count);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//int Symbol::getVirtualBasePointerOffset()
//{
//    return callSymbol(get_virtualBasePointerOffset);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONG Symbol::getVirtualBaseDispIndex()
//{
//    return callSymbol(get_virtualBaseDispIndex);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONG Symbol::getVirtualBaseDispSize()
//{
//   SymbolPtr   baseTableType = SymbolPtr( new Symbol( callSymbol(get_virtualBaseTableType), m_machineType ) );
//
//   return (ULONG)baseTableType->getType()->getSize();
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONG Symbol::getSection()
//{
//    return callSymbol(get_targetSection);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//void Symbol::getValueImpl(IDiaSymbol *_symbol, VARIANT &vtValue)
//{
//    HRESULT hres = _symbol->get_value(&vtValue);
//    if (S_OK != hres)
//        throw Exception("Call IDiaSymbol::get_value", hres);
//}
//
//void Symbol::getValue( VARIANT &vtValue)
//{
//    HRESULT hres = m_symbol->get_value(&vtValue);
//    if (S_OK != hres)
//        throw Exception("Call IDiaSymbol::get_value", hres);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//python::object Symbol::getValue()
//{
//    VARIANT vtValue = { VT_EMPTY };
//    getValueImpl(m_symbol, vtValue);
//    switch (vtValue.vt)
//    {
//    case VT_I1:
//    case VT_UI1:
//        return python::object( static_cast<ULONG>(vtValue.bVal) );
//
//    case VT_BOOL:
//        return python::object( static_cast<bool>(!!vtValue.iVal) );
//
//    case VT_I2:
//    case VT_UI2:
//        return python::object( static_cast<ULONG>(vtValue.iVal) );
//
//    case VT_I4:
//    case VT_UI4:
//    case VT_INT:
//    case VT_UINT:
//    case VT_ERROR:
//    case VT_HRESULT:
//        return python::object( vtValue.lVal );
//
//    case VT_I8:
//    case VT_UI8:
//        return python::object( vtValue.llVal );
//
//    case VT_R4:
//        return python::object( double(vtValue.fltVal) );
//
//    case VT_R8:
//        return python::object( vtValue.dblVal );
//
//    case VT_BSTR:
//        return python::object( autoBstr::asStr(vtValue.bstrVal).c_str() );
//
//    }
//    throw Exception("Unknown value type");
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//bool Symbol::isBasicType()
//{
//    DWORD baseType = btNoType;
//    return 
//        SUCCEEDED( m_symbol->get_baseType(&baseType) ) && 
//        (btNoType != baseType);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//bool Symbol::isVirtualBaseClass()
//{
//    return !!callSymbol(get_virtualBaseClass);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//bool Symbol::isIndirectVirtualBaseClass()
//{
//    return !!callSymbol(get_indirectVirtualBaseClass);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONG Symbol::getBaseType()
//{
//    return callSymbol(get_baseType);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONG Symbol::getBitPosition()
//{
//    return callSymbol(get_bitPosition);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONG Symbol::getIndexId()
//{
//    return callSymbol(get_symIndexId);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONG Symbol::getUdtKind()
//{
//    return callSymbol(get_udtKind);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONG Symbol::getDataKind()
//{
//    return callSymbol(get_dataKind);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONG Symbol::getRegisterId()
//{
//    return callSymbol(get_registerId);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//bool Symbol::isConstant()
//{
//    return !!callSymbol(get_constType); 
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//SymbolPtr Symbol::getChildByName(const std::string &_name)
//{
//    DiaEnumSymbolsPtr symbols;
//    HRESULT hres = 
//        m_symbol->findChildren(
//            SymTagNull,
//            toWStr(_name),
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
//    if (!count)
//        throw Exception(_name + " not found");
//
//    if (count != 1)
//        throw Exception(_name + " is not unique");
//
//    DiaSymbolPtr child;
//    hres = symbols->Item(0, &child);
//    if (S_OK != hres)
//        throw Exception("Call IDiaEnumSymbols::Item", hres);
//
//    return SymbolPtr( new Symbol(child, m_machineType) );
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//std::string Symbol::print()
//{
//    return printImpl(m_symbol, m_machineType);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//bool Symbol::eq(Symbol &rhs)
//{
//    return getIndexId() == rhs.getIndexId();
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//GlobalScope::GlobalScope(
//    __inout DiaDataSourcePtr &dataSource,
//    __inout DiaSessionPtr &_session,
//    __inout DiaSymbolPtr &_globalScope
//)   : Symbol(_globalScope, CV_CFL_80386)
//    , m_dataSource( dataSource.Detach() )
//    , m_session( _session.Detach() )
//{
//    m_symbol->get_machineType(&m_machineType);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//GlobalScopePtr GlobalScope::loadPdb(const std::string &filePath)
//{
//    class CLoaderFromPdb : public IScopeDataLoader {
//    public:
//        CLoaderFromPdb(const std::string &filePath) : m_filePath(filePath) {}
//
//        virtual void loadData(IDiaDataSource *dataSource) override {
//            HRESULT hres = dataSource->loadDataFromPdb( toWStr(m_filePath) );
//            if ( S_OK != hres )
//                throw Exception("Call IDiaDataSource::loadDataFromPdb", hres);
//        }
//
//    private:
//        const std::string &m_filePath;
//    } loaderFromPdb(filePath);
//
//    return loadImpl(loaderFromPdb);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//GlobalScopePtr GlobalScope::loadExe(const std::string &filePath, PCSTR searchPath /*= NULL*/)
//{
//    if (!searchPath)
//        searchPath = "SRV**\\\\symbols\\symbols";
//
//    class CLoaderForExe : public IScopeDataLoader {
//    public:
//        CLoaderForExe(const std::string &filePath, PCSTR searchPath)
//            : m_filePath(filePath), m_searchPath(searchPath)
//        {
//        }
//
//        virtual void loadData(IDiaDataSource *dataSource) override {
//            LoadCallback loadCallback;
//            HRESULT hres = 
//                dataSource->loadDataForExe( toWStr(m_filePath), toWStr(m_searchPath), &loadCallback );
//            if ( S_OK != hres )
//                throw Exception("Call IDiaDataSource::loadDataForExe", hres);
//        }
//
//    private:
//        const std::string &m_filePath;
//        const std::string m_searchPath;
//    } loaderForExe(filePath, searchPath);
//
//    return loadImpl(loaderForExe);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//GlobalScopePtr GlobalScope::loadImpl(IScopeDataLoader &ScopeDataLoader)
//{
//    DiaDataSourcePtr dataSource;
//
//    HRESULT hres = 
//        dataSource.CoCreateInstance(__uuidof(DiaSource), NULL, CLSCTX_INPROC_SERVER);
//    if ( S_OK != hres )
//        throw Exception("Call ::CoCreateInstance", hres);
//
//    ScopeDataLoader.loadData(dataSource);
//
//    DiaSessionPtr _session;
//    hres = dataSource->openSession(&_session);
//    if ( S_OK != hres )
//        throw Exception("Call IDiaDataSource::openSession", hres);
//
//    DiaSymbolPtr _globalScope;
//    hres = _session->get_globalScope(&_globalScope);
//    if ( S_OK != hres )
//        throw Exception("Call IDiaSymbol::get_globalScope", hres);
//
//    return GlobalScopePtr( new GlobalScope(dataSource, _session, _globalScope) );
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//SymbolPtr GlobalScope::findByRvaImpl(
//    __in ULONG rva,
//    __in ULONG symTag,
//    __out LONG &displacement
//)
//{
//    DiaSymbolPtr child;
//    HRESULT hres = 
//        m_session->findSymbolByRVAEx(
//            rva,
//            static_cast<enum SymTagEnum>(symTag),
//            &child,
//            &displacement);
//    if (S_OK != hres)
//        throw Exception("Call IDiaSession::findSymbolByRVAEx", hres);
//    if (!child)
//        throw Exception("Call IDiaSession::findSymbolByRVAEx", E_UNEXPECTED);
//
//    return SymbolPtr( new Symbol(child, m_machineType) );
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//SymbolPtr GlobalScope::findByVaImpl(
//    __in ULONGLONG va,
//    __in ULONG symTag,
//    __out LONG &displacement
//)
//{
//    DiaSymbolPtr child;
//    HRESULT hres = 
//        m_session->findSymbolByVAEx(
//            va,
//            static_cast<enum SymTagEnum>(symTag),
//            &child,
//            &displacement);
//    if (S_OK != hres)
//        throw Exception("Call IDiaSession::findSymbolByVAEx", hres);
//    if (!child)
//        throw Exception("Call IDiaSession::findSymbolByVAEx", E_UNEXPECTED);
//
//    return SymbolPtr( new Symbol(child, m_machineType) );
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//SymbolPtr GlobalScope::getSymbolById(ULONG symId)
//{
//    DiaSymbolPtr _symbol;
//    HRESULT hres = m_session->symbolById(symId, &_symbol);
//    if (S_OK != hres)
//        throw Exception("Call IDiaSession::findSymbolByRVAEx", hres);
//    if (!_symbol)
//        throw Exception("Call IDiaSession::findSymbolByRVAEx", E_UNEXPECTED);
//
//    return SymbolPtr( new Symbol(_symbol, m_machineType) );
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//ULONGLONG GlobalScope::getLoadAddress()
//{
//    ULONGLONG loadAddress;
//    HRESULT hres = m_session->get_loadAddress(&loadAddress);
//    if (S_OK != hres)
//        throw Exception("Call IDiaSession::get_loadAddress", hres);
//    return loadAddress;
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//void GlobalScope::setLoadAddress(ULONGLONG loadAddress)
//{
//    HRESULT hres = m_session->put_loadAddress(loadAddress);
//    if (S_OK != hres)
//        throw Exception("Call IDiaSession::put_loadAddress", hres);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//}
