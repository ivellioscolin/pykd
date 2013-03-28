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
        throw DbgException( "IDebugSymbol::GetModuleByModuleName  failed" ); 

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
        throw DbgException( "IDebugSymbol::GetModuleByOffset failed" );

    return base;
}

///////////////////////////////////////////////////////////////////////////////////

ULONG64 findModuleBySymbol( const std::string &symbolName )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG64     base;

    hres = g_dbgEng->symbols->GetSymbolModule( ( std::string("!") + symbolName ).c_str(), &base );
    if ( FAILED( hres ) )
    {
        std::stringstream   sstr;
        sstr << "failed to find module for symbol: " << symbolName;
        throw SymbolException( sstr.str() );
    }

    return base;
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
        throw DbgException( "IDebugSymbol::GetModuleNameString failed" );

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
        throw DbgException( "IDebugSymbol::GetModuleNameString failed" );

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
         throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );    

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
        throw DbgException( "IDebugAdvanced2::GetSymbolInformation failed" );

    if (!*moduleInfo.LoadedPdbName)
    {
        std::wstringstream sstr;
        sstr << L"/f \"" << moduleInfo.ImageName << L"\"";

        hres = g_dbgEng->symbols->ReloadWide( sstr.str().c_str() );
        if ( FAILED( hres ) )
            throw DbgException("IDebugSymbols::Reload failed" );

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
            throw DbgException( "IDebugAdvanced2::GetSymbolInformation failed" );
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
         throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );

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
         throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );    

    return moduleParam.Checksum;
}

}

ULONG getModuleCheckSum( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    return getModuleCheckSumImpl( baseOffset );
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
         throw DbgException( "IDebugSymbol2::GetModuleVersionInformation  failed" ); 

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
         throw DbgException( "IDebugSymbol2::GetModuleVersionInformation  failed" ); 

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
         throw DbgException( "IDebugSymbol2::GetModuleVersionInformation  failed" );

    ULONG productNameLength = 0;

    std::stringstream  sstr;
    sstr << "\\StringFileInfo\\" << std::hex 
            << std::setw(4) << std::setfill('0') <<  codePages[0].wLanguage 
            << std::setw(4) << std::setfill('0') << codePages[0].wCodePage 
            << "\\" << value;

    ULONG  valueLength;

    g_dbgEng->symbols->GetModuleVersionInformation( 
        DEBUG_ANY_ID,
        baseOffset,
        sstr.str().c_str(),
        NULL,
        0,
        &valueLength );

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
