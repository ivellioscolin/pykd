#include "stdafx.h"

#include <boost/format.hpp>

#include "dbgext.h"
#include "dbgmem.h"
#include "dbgmodule.h"
#include "dbgexcept.h"
#include "dbgsym.h"
#include "dbgio.h"
#include "dbgsynsym.h"

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadModule( const std::string &moduleName )
{
    HRESULT         hres;
   
    try {
        ULONG64    moduleBase;
        hres = dbgExt->symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &moduleBase );
        if ( FAILED( hres ) )
             return boost::python::object();
        
        DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };
        hres = dbgExt->symbols->GetModuleParameters( 1, &moduleBase, 0, &moduleParam );
        if ( FAILED( hres ) )
             throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );      
       
             
        return boost::python::object( dbgModuleClass( moduleName, moduleBase, moduleParam.Size ) );            
        
    }
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	 
	
	return boost::python::object();
}

/////////////////////////////////////////////////////////////////////////////////

void queryModuleParams(
    __in ULONG64 addr,
    __out std::string &name,
    __out ULONG64 &base,
    __out ULONG &size
)
{
    addr = addr64( addr );

    ULONG moduleIndex;
    HRESULT hres = 
        dbgExt->symbols->GetModuleByOffset( addr, 0, &moduleIndex, &base);
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleByOffset failed" );

    DEBUG_MODULE_PARAMETERS moduleParam = { 0 };
    hres = dbgExt->symbols->GetModuleParameters( 1, &base, 0, &moduleParam );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleParameters failed" );
    size = moduleParam.Size;

    ULONG moduleNameChars = 0;
    dbgExt->symbols->GetModuleNames(
        moduleIndex,
        0,
        NULL,
        0,
        NULL,
        NULL,
        0,
        &moduleNameChars,
        NULL,
        0,
        NULL );
    name.resize(moduleNameChars + 1);
    hres = dbgExt->symbols->GetModuleNames(
        moduleIndex,
        0,
        NULL,
        0,
        NULL,
        &name[0],
        (ULONG)name.size(),
        NULL,
        NULL,
        0,
        NULL );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleNames  failed" );
}

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
findModule( ULONG64 addr )
{
    try {
        ULONG64 moduleBase;
        ULONG moduleSize;
        std::string moduleName;
        queryModuleParams(addr, moduleName, moduleBase, moduleSize);
        return 
            boost::python::object(
                dbgModuleClass( moduleName, moduleBase, moduleSize )
            );
    }
    catch( std::exception  &e )
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
    }
    catch(...)
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
    }

    return boost::python::object();    
}

/////////////////////////////////////////////////////////////////////////////////

dbgModuleClass::dbgModuleClass( const std::string &name, ULONG64 base, ULONG size ) :
    m_name( name ),
    m_base( addr64(base) ),
    m_end( addr64(base) + size )
{
    reloadSymbols();

    std::string         pattern = name + "!*";
    ULONG64             enumHandle = 0;
    
    HRESULT   hres = dbgExt->symbols->StartSymbolMatch( pattern.c_str(), &enumHandle );
    
    while( SUCCEEDED( hres ) )
    {
        char            nameBuf[0x100];
        ULONG64         offset = 0;
    
        hres = 
            dbgExt->symbols->GetNextSymbolMatch(
                enumHandle,
                nameBuf,
                sizeof( nameBuf ),
                NULL,
                &offset );

        if ( FAILED( hres ) )
            break;

        std::string   symbolName( nameBuf );

        symbolName.erase( 0, name.size() + 1 );

        m_offsets.insert( std::make_pair( symbolName, offset ) );
    }

    if ( enumHandle )
        dbgExt->symbols->EndSymbolMatch( enumHandle );

    memset( &m_debugInfo, 0, sizeof( m_debugInfo ) );

    hres = dbgExt->advanced2->GetSymbolInformation(
        DEBUG_SYMINFO_IMAGEHLP_MODULEW64,
        base,
        0,
        &m_debugInfo,
        sizeof( m_debugInfo ),
        NULL,
        NULL,
        0,
        NULL );

    if ( SUCCEEDED( hres ) )
        getImagePath();
}

/////////////////////////////////////////////////////////////////////////////////

void
dbgModuleClass::reloadSymbols()
{
    HRESULT         hres;
     
    try {
        static const char *szReloadParam = "/f "; //"/f /s ";
        std::string   reloadParam = szReloadParam;
        reloadParam += m_name;

        {
            // try reload module by entered name, "silent mode"
            OutputReader outputReader( dbgExt->client );
            hres = dbgExt->symbols->Reload( reloadParam.c_str() );
        }
        if ( FAILED( hres ) )
        {
            // failed => try reload symbols by image file name
            char szImageName[MAX_PATH/2];
            HRESULT hres2 = dbgExt->symbols2->GetModuleNameString(
                DEBUG_MODNAME_IMAGE,
                DEBUG_ANY_ID,
                m_base,
                szImageName,
                _countof(szImageName),
                NULL);
            if (SUCCEEDED(hres2))
            {
                PCSTR szImageFileName = strrchr(szImageName, '\\');
                if (!szImageFileName)
                    szImageFileName = szImageName;
                else
                    ++szImageFileName;

                reloadParam = szReloadParam;
                reloadParam += szImageFileName;
                hres = dbgExt->symbols->Reload( reloadParam.c_str() );
            }
        }

        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbol::Reload  failed" );
    }
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	 
	
}

/////////////////////////////////////////////////////////////////////////////////

ULONG64
dbgModuleClass::getOffset( const std::string  &symName )
{
    OffsetMap::iterator  offset = m_offsets.find( symName ); 
    if ( offset != m_offsets.end() )
    {
        return offset->second;
    }
    ModuleInfo moduleInfo(m_debugInfo);
    return ::getSyntheticSymbol(moduleInfo, symName);
}

/////////////////////////////////////////////////////////////////////////////////

bool dbgModuleClass::addSyntheticSymbol(
    ULONG64 offset,
    ULONG size,
    const std::string &symName
)
{
    ModuleInfo moduleInfo(m_debugInfo);
    return ::addSyntheticSymbolForModule(offset, size, symName, moduleInfo);
}

/////////////////////////////////////////////////////////////////////////////////

void dbgModuleClass::delAllSyntheticSymbols()
{
    ModuleInfo moduleInfo(m_debugInfo);
    ::delAllSyntheticSymbolsForModule(moduleInfo);
}

/////////////////////////////////////////////////////////////////////////////////

ULONG dbgModuleClass::delSyntheticSymbol(
    ULONG64 offset
)
{
    ModuleInfo moduleInfo(m_debugInfo);
    return ::delSyntheticSymbolForModule(offset, moduleInfo);
}

/////////////////////////////////////////////////////////////////////////////////

ULONG dbgModuleClass::delSyntheticSymbolsMask( const std::string &symName )
{
    return ::delSyntheticSymbolsMask(m_name, symName);
}

/////////////////////////////////////////////////////////////////////////////////

void
dbgModuleClass::getImagePath()
{
    HRESULT         hres;
         
    try {
    
        ULONG       pathSize = 0;
        hres = dbgExt->symbols3->GetSymbolPathWide( NULL, 0, &pathSize );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbol3::GetImagePathWide  failed" );
            
	    std::vector<WCHAR> pathBuffer(pathSize);
        
        hres = dbgExt->symbols3->GetSymbolPathWide( &pathBuffer[0], pathSize, NULL );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbol3::GetImagePathWide  failed" );
            
        std::wstring   symPath( &pathBuffer[0], pathSize );
        
        std::wstring   altName =  m_debugInfo.CVData;
        altName = altName.substr( 0, altName.find_last_of(L".") );
        
        std::wstring   imageName = m_debugInfo.LoadedImageName;
        altName += imageName.substr( imageName.find_last_of(L".") );        
        
        for ( size_t  offset = 0; offset < symPath.length(); )
        {
            size_t  newOffset = symPath.find( L";", offset );
            std::wstring    subPath = symPath.substr( offset, newOffset - offset );
            
            std::wstringstream   sstr;
            
            sstr << subPath  << L"\\" << m_debugInfo.LoadedImageName << L"\\" << std::hex << 
                m_debugInfo.TimeDateStamp << m_debugInfo.ImageSize << L"\\" << 
                m_debugInfo.LoadedImageName;
                
            if( (_waccess( sstr.str().c_str(), 0 )) != -1 )
            {
                m_imageFullName = sstr.str();
                break;
            }   
            
            
            std::wstringstream   altstr;
            
            altstr << subPath << L"\\" << altName << L"\\" << std::hex << 
                m_debugInfo.TimeDateStamp << m_debugInfo.ImageSize << L"\\" << 
                altName;
                
            if( (_waccess( altstr.str().c_str(), 0 )) != -1 )
            {
                m_imageFullName = altstr.str();
                break;
            }               
            
            if ( newOffset == std::wstring::npos )
                break;
            
            offset = newOffset + 1;
        }       
          
    }
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	
}

std::string
dbgModuleClass::print() const 
{
	try
	{
		const char * format_string(dbgExt->control->IsPointer64Bit() == S_OK ?
			"%1$016x %2$016x %3$20s %4$20s" : "%1$08x %2$08x %3$20s %4$20s");
		boost::format fmt(format_string);
		std::vector<char> v(MAX_PATH);
		::WideCharToMultiByte(
            CP_ACP,
            0,
            m_imageFullName.c_str(),
            -1,
            &v[0],
            (ULONG)v.size(),
            0,
            0);
		std::string fullname(&v[0]);
		fmt % m_base % (m_end - m_base) % m_name % fullname;
				return fmt.str();
	}
	catch (std::exception & e)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch (...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////////