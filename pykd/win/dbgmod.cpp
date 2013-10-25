// 
// Work with modules
// 

///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbgeng.h"

#include <iomanip>

///////////////////////////////////////////////////////////////////////////////

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

ULONG64 findModuleBase( const std::string &moduleName )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG64     base;

    hres = g_dbgEng->symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &base );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleByModuleName", hres );

    return base;
}

///////////////////////////////////////////////////////////////////////////////////

ULONG64 findModuleBase( ULONG64 offset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG64     base;
    ULONG       moduleIndex;

    hres = g_dbgEng->symbols->GetModuleByOffset( offset, 0, &moduleIndex, &base );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleByOffset", hres );

    return base;
}

///////////////////////////////////////////////////////////////////////////////////

ULONG64 findModuleBySymbol( const std::string &symbolName )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG64     base;

    std::string  str = "!";
    str += symbolName;

    hres = g_dbgEng->symbols->GetSymbolModule( str.c_str(), &base );
    if ( SUCCEEDED( hres ) )
        return base;

    DEBUG_VALUE  debugValue = {};
    ULONG        remainderIndex = 0;

    hres = 
        g_dbgEng->control->Evaluate( 
            symbolName.c_str(), 
            DEBUG_VALUE_INT64,
            &debugValue,
            &remainderIndex );

    if ( SUCCEEDED( hres ) )
    {
        ULONG64     base;
        ULONG       moduleIndex;

        hres = g_dbgEng->symbols->GetModuleByOffset( debugValue.I64, 0, &moduleIndex, &base );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbol::GetModuleByOffset", hres );

        return  base;
    }

    std::stringstream   sstr;
    sstr << "failed to find module for symbol: " << symbolName;
    throw SymbolException( sstr.str() );
}

///////////////////////////////////////////////////////////////////////////////////

namespace {

std::string getModuleNameImpl( ULONG64 baseOffset )
{
    HRESULT  hres;

    char  moduleName[0x100];

    hres = g_dbgEng->symbols->GetModuleNameString( 
        DEBUG_MODNAME_MODULE,
        DEBUG_ANY_ID,
        baseOffset,
        moduleName,
        sizeof( moduleName ),
        NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleNameString", hres );

    return std::string( moduleName );
}

}

std::string getModuleName( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    return getModuleNameImpl( baseOffset );
}

///////////////////////////////////////////////////////////////////////////////////

std::string getModuleImageName( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;
    char  imageName[0x100];

    hres = g_dbgEng->symbols->GetModuleNameString( 
        DEBUG_MODNAME_IMAGE,
        DEBUG_ANY_ID,
        baseOffset,
        imageName,
        sizeof( imageName ),
        NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleNameString", hres );

    return std::string( imageName );
}

///////////////////////////////////////////////////////////////////////////////////

namespace {

ULONG getModuleSizeImpl( ULONG64 baseOffset )
{
    HRESULT  hres;
    DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };

    hres = g_dbgEng->symbols->GetModuleParameters( 1, &baseOffset, 0, &moduleParam );
    if ( FAILED( hres ) )
         throw DbgException( "IDebugSymbol::GetModuleParameters", hres );

    return moduleParam.Size;
}

}

ULONG getModuleSize( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    return getModuleSizeImpl( baseOffset );
}

///////////////////////////////////////////////////////////////////////////////////

std::string getModuleSymbolFileName( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;
    IMAGEHLP_MODULEW64   moduleInfo = {};

    hres = g_dbgEng->advanced->GetSymbolInformation(
        DEBUG_SYMINFO_IMAGEHLP_MODULEW64,
        baseOffset,
        0,
        &moduleInfo,
        sizeof(moduleInfo),
        NULL,
        NULL,
        0,
        NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugAdvanced2::GetSymbolInformation", hres );

    if (!*moduleInfo.LoadedPdbName)
    {
        std::wstringstream sstr;
        sstr << L"/f \"" << moduleInfo.ImageName << L"\"";

        hres = g_dbgEng->symbols->ReloadWide( sstr.str().c_str() );
        if ( FAILED( hres ) )
            throw DbgException("IDebugSymbols::Reload", hres );

        hres = g_dbgEng->advanced->GetSymbolInformation(
            DEBUG_SYMINFO_IMAGEHLP_MODULEW64,
            baseOffset,
            0,
            &moduleInfo,
            sizeof(moduleInfo),
            NULL,
            NULL,
            0,
            NULL );

        if ( FAILED( hres ) )
            throw DbgException( "IDebugAdvanced2::GetSymbolInformation", hres );
    }

    char  pdbName[ 256 ];
    WideCharToMultiByte( CP_ACP, 0, moduleInfo.LoadedPdbName, 256, pdbName, 256, NULL, NULL );

    return std::string( pdbName );
}

///////////////////////////////////////////////////////////////////////////////////

namespace{

ULONG getModuleTimeStampImpl( ULONG64 baseOffset )
{
    HRESULT  hres;
    DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };

    hres = g_dbgEng->symbols->GetModuleParameters( 1, &baseOffset, 0, &moduleParam );
    if ( FAILED( hres ) )
         throw DbgException( "IDebugSymbol::GetModuleParameters", hres );

    return moduleParam.TimeDateStamp;
}

}

ULONG getModuleTimeStamp( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    return getModuleTimeStampImpl( baseOffset );
}

///////////////////////////////////////////////////////////////////////////////////

namespace {

ULONG getModuleCheckSumImpl( ULONG64 baseOffset )
{
    HRESULT  hres;
    DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };

    hres = g_dbgEng->symbols->GetModuleParameters( 1, &baseOffset, 0, &moduleParam );
    if ( FAILED( hres ) )
         throw DbgException( "IDebugSymbol::GetModuleParameters", hres );

    return moduleParam.Checksum;
}

}

ULONG getModuleCheckSum( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    return getModuleCheckSumImpl( baseOffset );
}

///////////////////////////////////////////////////////////////////////////////

namespace  {

ULONG getModuleFlags(ULONG64 baseOffset)
{
    HRESULT hres;
    DEBUG_MODULE_PARAMETERS moduleParam = { 0 };

    hres = g_dbgEng->symbols->GetModuleParameters( 1, &baseOffset, 0, &moduleParam );
    if ( FAILED( hres ) )
         throw DbgException( "IDebugSymbol::GetModuleParameters", hres );

    return moduleParam.Flags;
}

}

bool isModuleUnloaded( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );
    return !!(getModuleFlags(baseOffset) & DEBUG_MODULE_UNLOADED);
}

bool isModuleUserMode( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );
    return !!(getModuleFlags(baseOffset) & DEBUG_MODULE_USER_MODE);
}

///////////////////////////////////////////////////////////////////////////////

void getModuleFileVersion( ULONG64 baseOffset, USHORT &majorHigh, USHORT &majorLow, USHORT &minorHigh, USHORT &minorLow )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    VS_FIXEDFILEINFO  fileInfo={};

    HRESULT  hres;

    hres = g_dbgEng->symbols->GetModuleVersionInformation( 
        DEBUG_ANY_ID,
        baseOffset,
        "\\",
        (PVOID)&fileInfo,
        sizeof(fileInfo),
        NULL );

    if ( FAILED( hres ) )
         throw DbgException( "IDebugSymbol2::GetModuleVersionInformation", hres ); 

    majorHigh = HIWORD(fileInfo.dwFileVersionMS);
    majorLow = LOWORD(fileInfo.dwFileVersionMS); 
    minorHigh =  HIWORD(fileInfo.dwFileVersionLS);
    minorLow = LOWORD(fileInfo.dwFileVersionLS);
}

///////////////////////////////////////////////////////////////////////////////

std::string getModuleVersionInfo( ULONG64 baseOffset, const std::string &value )
{
    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    };

    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;

    ULONG codePagesSize = 0;

    hres = g_dbgEng->symbols->GetModuleVersionInformation( 
        DEBUG_ANY_ID,
        baseOffset,
        "\\VarFileInfo\\Translation",
        NULL,
        0,
        &codePagesSize );

    if ( FAILED( hres ) )
         throw DbgException( "IDebugSymbol2::GetModuleVersionInformation", hres ); 

    size_t codePageNum = codePagesSize / sizeof(LANGANDCODEPAGE);

    std::vector<LANGANDCODEPAGE> codePages(codePageNum);

    hres = g_dbgEng->symbols->GetModuleVersionInformation( 
        DEBUG_ANY_ID,
        baseOffset,
        "\\VarFileInfo\\Translation",
        &codePages[0],
        codePagesSize,
        NULL );

    if ( FAILED( hres ) )
         throw DbgException( "IDebugSymbol2::GetModuleVersionInformation", hres );

    ULONG productNameLength = 0;

    std::stringstream  sstr;
    sstr << "\\StringFileInfo\\" << std::hex 
            << std::setw(4) << std::setfill('0') <<  codePages[0].wLanguage 
            << std::setw(4) << std::setfill('0') << codePages[0].wCodePage 
            << "\\" << value;

    ULONG  valueLength = 0;

    g_dbgEng->symbols->GetModuleVersionInformation( 
        DEBUG_ANY_ID,
        baseOffset,
        sstr.str().c_str(),
        NULL,
        0,
        &valueLength );

    if (!valueLength)
        return "";

    std::vector<char>  valueStr(valueLength);

    hres = g_dbgEng->symbols->GetModuleVersionInformation( 
        DEBUG_ANY_ID,
        baseOffset,
        sstr.str().c_str(),
        &valueStr[0],
        valueLength,
        NULL );

    if ( hres == S_OK )
        return std::string( &valueStr[0] );

   return "";
}

///////////////////////////////////////////////////////////////////////////////

}   // namespace pykd

///////////////////////////////////////////////////////////////////////////////
