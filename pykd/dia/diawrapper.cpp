
#include "stdafx.h"
#include "dbghelp.h"
#include "dia/diawrapper.h"
#include "win/utils.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

static const struct DiaRegToRegRelativeAmd64 : DiaRegToRegRelativeBase
{
    typedef std::map<ULONG, ULONG> Base;
    DiaRegToRegRelativeAmd64();
} g_DiaRegToRegRelativeAmd64;

DiaRegToRegRelativeAmd64::DiaRegToRegRelativeAmd64()
{
    (*this)[CV_AMD64_RIP] = rriInstructionPointer;
    (*this)[CV_AMD64_RBP] = rriStackFrame;
    (*this)[CV_AMD64_RSP] = rriStackPointer;
}

///////////////////////////////////////////////////////////////////////////////

static const struct DiaRegToRegRelativeI386 : DiaRegToRegRelativeBase
{
    typedef std::map<ULONG, ULONG> Base;
    DiaRegToRegRelativeI386();
} g_DiaRegToRegRelativeI386;

DiaRegToRegRelativeI386::DiaRegToRegRelativeI386()
{
    (*this)[CV_REG_EIP] = rriInstructionPointer;
    (*this)[CV_REG_EBP] = rriStackFrame;
    (*this)[CV_REG_ESP] = rriStackPointer;
}

///////////////////////////////////////////////////////////////////////////////

const std::string DiaException::descPrefix("pyDia: ");

std::string DiaException::makeFullDesc(const std::string &desc, HRESULT hres, IDiaSymbol *symbol /*= NULL*/)
{
    std::stringstream sstream;
    sstream << descPrefix << desc << " failed" << std::endl;
    if (symbol)
    {
        BSTR bstrName = NULL;
        HRESULT locRes = symbol->get_undecoratedName(&bstrName);
        if (S_OK == locRes && bstrName)
        {
            autoBstr freeBstr(bstrName);
            sstream << "Symbol name: \"" << autoBstr::asStr(bstrName) << "\"";
        }
        else
        {
            locRes = symbol->get_name(&bstrName);
            if (S_OK == locRes && bstrName)
            {
                autoBstr freeBstr(bstrName);
                sstream << "Symbol name: " << autoBstr::asStr(bstrName);
            }
            else
            {
                sstream << "Symbol: ";
            }
        }

        DWORD dwValue;
        locRes = symbol->get_relativeVirtualAddress(&dwValue);
        if (S_OK == locRes)
        {
            sstream << ", RVA= 0x" << std::hex << dwValue;
        }

        locRes = symbol->get_symTag(&dwValue);
        if (S_OK == locRes)
        {
            sstream << ", tag= " << std::dec << dwValue;
        }

        locRes = symbol->get_locationType(&dwValue);
        if (S_OK == locRes)
        {
            sstream << ", location: " << std::dec << dwValue;
        }

        sstream << std::endl;
    }
    sstream << "Return value is 0x" << std::hex << hres;

    switch (hres)
    {
#define _CASE_DIA_ERROR(x)  case E_PDB_##x: sstream << ": E_PDB_" #x << std::endl; break

    _CASE_DIA_ERROR(USAGE);
    _CASE_DIA_ERROR(OUT_OF_MEMORY);
    _CASE_DIA_ERROR(FILE_SYSTEM);
    _CASE_DIA_ERROR(NOT_FOUND);
    _CASE_DIA_ERROR(INVALID_SIG);
    _CASE_DIA_ERROR(INVALID_AGE);
    _CASE_DIA_ERROR(PRECOMP_REQUIRED);
    _CASE_DIA_ERROR(OUT_OF_TI);
    _CASE_DIA_ERROR(NOT_IMPLEMENTED);
    _CASE_DIA_ERROR(V1_PDB);
    _CASE_DIA_ERROR(FORMAT);
    _CASE_DIA_ERROR(LIMIT);
    _CASE_DIA_ERROR(CORRUPT);
    _CASE_DIA_ERROR(TI16);
    _CASE_DIA_ERROR(ACCESS_DENIED);
    _CASE_DIA_ERROR(ILLEGAL_TYPE_EDIT);
    _CASE_DIA_ERROR(INVALID_EXECUTABLE);
    _CASE_DIA_ERROR(DBG_NOT_FOUND);
    _CASE_DIA_ERROR(NO_DEBUG_INFO);
    _CASE_DIA_ERROR(INVALID_EXE_TIMESTAMP);
    _CASE_DIA_ERROR(RESERVED);
    _CASE_DIA_ERROR(DEBUG_INFO_NOT_IN_PDB);
    _CASE_DIA_ERROR(SYMSRV_BAD_CACHE_PATH);
    _CASE_DIA_ERROR(SYMSRV_CACHE_FULL);

#undef _CASE_DIA_ERROR
    default:
        {
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
        }
    }

    return sstream.str();
}

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

    return SymbolPtr( new DiaSymbol(DiaSymbolPtr(_symbol), machineType) );
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
                (caseSensitive ? nsCaseSensitive : nsCaseInsensitive) | nsfUndecoratedName | nsfRegularExpression,
                &symbols);

    }
    else
    {
        hres = m_symbol->findChildren(
            static_cast<enum ::SymTagEnum>(symTag),
                toWStr(name),
                (caseSensitive ? nsCaseSensitive : nsCaseInsensitive) | nsfUndecoratedName | nsfRegularExpression,
                &symbols);
    }

    if (S_OK != hres)
        throw DiaException("Call IDiaSymbol::findChildren", hres);

    SymbolPtrList childList;

    DiaSymbolPtr child;
    ULONG celt;
    while ( SUCCEEDED(symbols->Next(1, &child, &celt)) && (celt == 1) )
    {
        childList.push_back( SymbolPtr( new DiaSymbol(child, m_machineType) ) );
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
            nsfCaseSensitive | nsfUndecoratedName,
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
            nsfCaseSensitive | nsfUndecoratedName,
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

    return SymbolPtr( new DiaSymbol(child, m_machineType) );
}

////////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSymbol::getChildByName(const std::string &name )
{
    // ищем прямое совпадение
    DiaEnumSymbolsPtr symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            ::SymTagNull,
            toWStr(name),
            nsfCaseSensitive | nsfUndecoratedName,
            &symbols);

    LONG count;
    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::get_Count", hres);

    if (count >0 )
    {
        DiaSymbolPtr child;
        hres = symbols->Item(0, &child);
        if (S_OK != hres)
            throw DiaException("Call IDiaEnumSymbols::Item", hres);

        return SymbolPtr( new DiaSymbol(child, m_machineType) );
    }

    // _имя
    std::string underscoreName;
    underscoreName += '_';
    underscoreName += name;
    symbols = 0;

    hres = 
        m_symbol->findChildren(
            ::SymTagNull,
            toWStr(underscoreName),
            nsfCaseSensitive | nsfUndecoratedName,
            &symbols);

    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::get_Count", hres);

    if (count >0 )
    {
        DiaSymbolPtr child;
        hres = symbols->Item(0, &child);
        if (S_OK != hres)
            throw DiaException("Call IDiaEnumSymbols::Item", hres);

        return SymbolPtr( new DiaSymbol(child, m_machineType) );
    }
    
    // _имя@парам
    std::string     pattern = "_";
    pattern += name;
    pattern += "@*";
    symbols = 0;

    hres = 
        m_symbol->findChildren(
            ::SymTagNull,
            toWStr(pattern),
            nsfRegularExpression | nsfCaseSensitive | nsfUndecoratedName,
            &symbols);

    if (S_OK != hres)
        throw DiaException("Call IDiaSymbol::findChildren", hres);

    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::get_Count", hres);

    if (count == 0)
         throw DiaException( name + " not found");

    if (count >0 )
    {
        DiaSymbolPtr child;
        hres = symbols->Item(0, &child);
        if (S_OK != hres)
            throw DiaException("Call IDiaEnumSymbols::Item", hres);

        return SymbolPtr( new DiaSymbol(child, m_machineType) );
    }
    
    throw DiaException(name + " is not found");
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
        return getRegRealativeIdImpl(g_DiaRegToRegRelativeAmd64);
    case IMAGE_FILE_MACHINE_I386:
        return getRegRealativeIdImpl(g_DiaRegToRegRelativeI386);
    }
    throw DiaException("Unsupported machine type");
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getRegRealativeIdImpl(const DiaRegToRegRelativeBase &DiaRegToRegRelative)
{
    DiaRegToRegRelativeBase::const_iterator it = 
        DiaRegToRegRelative.find(callSymbol(get_registerId));

    if (it == DiaRegToRegRelative.end())
        throw DiaException("Cannot convert DAI register ID to relative register ID");

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

std::string DiaSymbol::getName()
{
    HRESULT hres;
    BSTR bstrName = NULL;

    ULONG symTag;
    hres = m_symbol->get_symTag( &symTag );

    if ( FAILED( hres ) )
        throw DiaException("Call IDiaSymbol::get_symTag", hres);
      
    if( symTag == SymTagData || symTag == SymTagFunction || symTag == SymTagPublicSymbol )
    {
        hres = m_symbol->get_undecoratedNameEx( UNDNAME_NAME_ONLY, &bstrName);
        if ( FAILED( hres ) )
            throw DiaException("Call IDiaSymbol::get_undecoratedNameEx", hres);

        std::string  retStr = autoBstr( bstrName ).asStr();

        if ( !retStr.empty() )
        {
            boost::cmatch  matchResult;

            if ( boost::regex_match( retStr.c_str(), matchResult, stdcallMatch ) )
                return std::string( matchResult[1].first, matchResult[1].second );

            if ( boost::regex_match( retStr.c_str(), matchResult, fastcallMatch ) )
                return std::string( matchResult[1].first, matchResult[1].second );
    
            return retStr; 
        }
    }

    bstrName = callSymbol(get_name);

    return autoBstr( bstrName ).asStr();
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
    DiaSymbolPtr diaSymbol(callSymbol(get_virtualBaseTableType));
    SymbolPtr baseTableType = SymbolPtr( new DiaSymbol( diaSymbol, m_machineType ) );

    return (ULONG)baseTableType->getType()->getSize();
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
    if ( !pdisplacement && displacement)
        throw DiaException("Call IDiaSession::findSymbolByRVAEx failed to find suymbol" );

    if (pdisplacement)
        *pdisplacement = displacement;

    return SymbolPtr( new DiaSymbol(child, m_globalSymbol->getMachineType() ) );
}

///////////////////////////////////////////////////////////////////////////////

void DiaSession::getSourceLine( ULONG64 offset, std::string &fileName, ULONG &lineNo, LONG &displacement )
{
    DiaEnumLineNumbersPtr  lines;

    HRESULT hres = m_session->findLinesByVA( offset, 1, &lines );
    if (S_OK != hres)
        throw DiaException("failed to find source line");

    DiaLineNumberPtr  sourceLine;
    hres = lines->Item( 0, &sourceLine );
    if (S_OK != hres)
        throw DiaException("failed to find source line");

    DiaSourceFilePtr  sourceFile;
    hres = sourceLine->get_sourceFile( &sourceFile );
    if (S_OK != hres)
        throw DiaException("failed to find source line");
    
    autoBstr  fileNameBstr;
    hres = sourceFile->get_fileName ( &fileNameBstr );
    if (S_OK != hres)
        throw DiaException("failed to find source line");
    fileName = fileNameBstr.asStr();

    hres = sourceLine->get_lineNumber( &lineNo );
    if (S_OK != hres)
        throw DiaException("failed to find source line");

    ULONGLONG  va;
    hres = sourceLine->get_virtualAddress ( &va );
    if (S_OK != hres)
        throw DiaException("failed to find source line");

    displacement = (LONG)( (LONGLONG)offset - (LONGLONG)va );
}

///////////////////////////////////////////////////////////////////////////////

}; // pykd nemaspace end

