
#include "stdafx.h"

#include <dbghelp.h>

#include "dia/diasymbol.h"
#include "win/utils.h"

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

#define callSymbol(method) \
    callSymbolT( &IDiaSymbol::##method, #method)

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

DiaSymbol::DiaSymbol(__inout DiaSymbolPtr &_symbol, DWORD machineType )
    : m_symbol(_symbol), m_machineType(machineType)
{
}

//////////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSymbol::fromGlobalScope( IDiaSymbol *_symbol )
{
    DWORD machineType;
    HRESULT hres = _symbol->get_machineType(&machineType);
    if (S_OK != hres)
        throw DiaException("IDiaSymbol::get_machineType", hres);
    if (!machineType)
        machineType = IMAGE_FILE_MACHINE_I386;

    std::auto_ptr< DiaSymbol > globalScope( new DiaSymbol(DiaSymbolPtr(_symbol), machineType) );
    globalScope->m_publicSymbols.reset( new DiaPublicSymbolCache( _symbol ) );
    return SymbolPtr( globalScope.release() );
}

//////////////////////////////////////////////////////////////////////////////////

DiaSymbol::SelectedChilds DiaSymbol::selectChildren(
    ULONG symtag,
    LPCOLESTR name,
    DWORD compareFlags
)
{
    BOOST_ASSERT(symtag < SymTagMax);

    const bool findAll = 
        ( !name || !*name || (name[0] == L'*' && name[1] == L'\0') );

    DiaEnumSymbolsPtr symbols;

    HRESULT hres = 
        m_symbol->findChildren(
            static_cast<enum ::SymTagEnum>(symtag),
            ( findAll ? NULL : name ),
            ( findAll ? nsNone : compareFlags),
            &symbols);

    if (S_OK != hres)
        throw DiaException("IDiaSymbol::findChildren", hres);

    LONG count = 0;
    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw DiaException("IDiaEnumSymbols::get_Count", hres);

    return SelectedChilds(symbols, count);
}

//////////////////////////////////////////////////////////////////////////////////

SymbolPtrList DiaSymbol::findChildren(ULONG symTag, const std::string &name)
{
    DiaEnumSymbolsPtr symbols = 
        selectChildren(symTag, toWStr(name), nsRegularExpression).first;

    SymbolPtrList childList;
    DiaSymbolPtr child;
    ULONG celt = 0;
    while ( SUCCEEDED(symbols->Next(1, &child, &celt)) && (celt == 1) )
    {
        childList.push_back( SymbolPtr( new DiaSymbol(child, m_machineType) ) );
        child.Release();
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
    return selectChildren(symTag).second;
}

////////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSymbol::getChildByIndex(ULONG symTag, ULONG _index )
{
    SelectedChilds selected = selectChildren(symTag);

    if (LONG(_index) >= selected.second)
    {
        throw PyException( PyExc_IndexError, "Index out of range");
    }

    DiaSymbolPtr child;
    HRESULT hres = selected.first->Item(_index, &child);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::Item", hres);

    return SymbolPtr( new DiaSymbol(child, m_machineType) );
}

////////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSymbol::getChildByName(const std::string &name)
{
    SelectedChilds selected = selectChildren(::SymTagNull, toWStr(name), nsCaseSensitive);

    if (selected.second > 0)
        return getChildBySelected(selected, name);

    if (m_publicSymbols)
    {
        DiaSymbolPtr publicSymbol = m_publicSymbols->lookup(name);
        if (publicSymbol)
            return SymbolPtr( new DiaSymbol(publicSymbol, m_machineType) );
    }

    // FIXME: c++ decoration is not supported

    // _имя
    std::string underscoreName;
    underscoreName += '_';
    underscoreName += name;

    selected = 
        selectChildren(
            ::SymTagNull,
            toWStr(underscoreName),
            nsfCaseSensitive | nsfUndecoratedName);

    if (selected.second > 0)
        return getChildBySelected(selected, name);

    // _имя@парам
    std::string pattern = "_";
    pattern += name;
    pattern += "@*";

    selected = 
        selectChildren(
            ::SymTagNull,
            toWStr(pattern),
            nsfRegularExpression | nsfCaseSensitive | nsfUndecoratedName);

    if (selected.second > 0)
        return getChildBySelected(selected, name);

    throw DiaException(name + " is not found");
}

//////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSymbol::getChildBySelected(const SelectedChilds &selected, const std::string &name)
{
    if (selected.second == 0)
        throw SymbolException(name + " is not found");

    DiaSymbolPtr child;
    HRESULT hres = selected.first->Item(0, &child);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::Item", hres);

    return SymbolPtr( new DiaSymbol(child, m_machineType) );
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

ULONG DiaSymbol::getRegRealativeId()
{
    switch (m_machineType)
    {
    case IMAGE_FILE_MACHINE_AMD64:
        return getRegRealativeIdImpl(regToRegRelativeAmd64);
    case IMAGE_FILE_MACHINE_I386:
        return getRegRealativeIdImpl(regToRegRelativeI386);
    }
    throw DiaException("Unsupported machine type");
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getRegRealativeIdImpl(const DiaRegToRegRelativeBase &regToRegRelative)
{
    DiaRegToRegRelativeBase::const_iterator it = 
        regToRegRelative.find(callSymbol(get_registerId));

    if (it == regToRegRelative.end())
        throw DiaException("Cannot convert DIA register ID to relative register ID");

    return it->second;
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getIndexId()
{
    return callSymbol(get_symIndexId);
}

////////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSymbol::getIndexType()
{
    DiaSymbolPtr diaSymbol(callSymbol(get_arrayIndexType));
    return SymbolPtr( new DiaSymbol(diaSymbol, m_machineType) );
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getLocType()
{
    return callSymbol(get_locationType);
}

//////////////////////////////////////////////////////////////////////////////

static const  boost::regex  stdcallMatch("^_(\\w+)(@\\d+)?$");
static const  boost::regex  fastcallMatch("^@(\\w+)(@\\d+)?$");

//////////////////////////////////////////////////////////////////////////////

std::string DiaSymbol::getName()
{
    std::string name = autoBstr( callSymbol(get_name) ).asStr();

    ULONG symTag;
    HRESULT hres = m_symbol->get_symTag( &symTag );
    if ( FAILED( hres ) )
        throw DiaException("Call IDiaSymbol::get_symTag", hres);

    if ( symTag != SymTagData && symTag != SymTagFunction && symTag != SymTagPublicSymbol )
        return name;

    std::string undecoratedName;
    {
        BSTR bstrUndecoratedName = NULL;
        hres = m_symbol->get_undecoratedNameEx( UNDNAME_NAME_ONLY, &bstrUndecoratedName);
        if (S_OK == hres)
            undecoratedName = autoBstr( bstrUndecoratedName ).asStr();
    }
    if ( undecoratedName.empty() )
        return name;

    // c++ decoration is not supported
    boost::cmatch  matchResult;

    if ( boost::regex_match( undecoratedName.c_str(), matchResult, stdcallMatch ) )
        return std::string( matchResult[1].first, matchResult[1].second );

    if ( boost::regex_match( undecoratedName.c_str(), matchResult, fastcallMatch ) )
        return std::string( matchResult[1].first, matchResult[1].second );

    return undecoratedName;
}

///////////////////////////////////////////////////////////////////////////////

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
    DiaSymbolPtr diaSymbol(callSymbol(get_type));
    return SymbolPtr( new DiaSymbol( diaSymbol, m_machineType ) );
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getUdtKind()
{
    return callSymbol(get_udtKind);
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
        btv = !!vtValue.boolVal;
        break;

    case VT_I2:
        btv = (LONG)vtValue.iVal;
        break;

    case VT_UI2:
        btv = (ULONG)vtValue.uiVal;
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
    DiaSymbolPtr diaSymbol(callSymbol(get_virtualBaseTableType));
    SymbolPtr baseTableType = SymbolPtr( new DiaSymbol( diaSymbol, m_machineType ) );

    return (ULONG)baseTableType->getType()->getSize();
}

std::string DiaSymbol::getBuildDescription() const 
{
    std::stringstream sstr;

    if (m_publicSymbols)
    {
        sstr << "Public symbols cache build time: ";
        sstr << std::dec << m_publicSymbols->getBuildTimeInSeconds();
        sstr << " sec";
    }

    return sstr.str();
}

//////////////////////////////////////////////////////////////////////////////

bool DiaSymbol::isBasicType()
{
    DWORD baseType = btNoType;
    return 
        SUCCEEDED( m_symbol->get_baseType(&baseType) ) && 
        (btNoType != baseType);
}

//////////////////////////////////////////////////////////////////////////////

bool DiaSymbol::isConstant()
{
    HRESULT  hres;
    BOOL  retBool = FALSE;

    hres = m_symbol->get_constType( &retBool );
    if ( FAILED( hres ) )
        throw DiaException("Call IDiaSymbol::get_constType", hres, m_symbol);

    return !!retBool;
}

//////////////////////////////////////////////////////////////////////////////

bool DiaSymbol::isIndirectVirtualBaseClass()
{
    return !!callSymbol(get_indirectVirtualBaseClass);
}

//////////////////////////////////////////////////////////////////////////////

bool DiaSymbol::isVirtualBaseClass()
{
    return !!callSymbol(get_virtualBaseClass);
}

//////////////////////////////////////////////////////////////////////////////

}; // pykd namespace end

