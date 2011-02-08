#include "stdafx.h"

#include "dbgext.h"
#include "dbgmodule.h"
#include "dbgexcept.h"
#include "dbgmem.h"
#include "dbgsym.h"
#include "dbgcallback.h"

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

boost::python::object
findModule( ULONG64 addr )
{
    HRESULT         hres;
    
    addr = addr64( addr );
      
    try {
    
        ULONG     moduleIndex;
        ULONG64   moduleBase;            
        hres = dbgExt->symbols->GetModuleByOffset( addr, 0, &moduleIndex, &moduleBase );    
        
        if ( FAILED( hres ) )
        {
            return boost::python::object();   
        }       
        
        DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };
        hres = dbgExt->symbols->GetModuleParameters( 1, &moduleBase, 0, &moduleParam );
        if ( FAILED( hres ) )
             throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );      
             
        char   moduleName[0x100];             
             
        hres = 
            dbgExt->symbols->GetModuleNames(  
                moduleIndex,
                0,
                NULL,
                0,
                NULL,
                moduleName,
                sizeof( moduleName ),
                NULL,
                NULL,
                0,
                NULL );
                
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbol::GetModuleNames  failed" );                 
             
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

dbgModuleClass::dbgModuleClass( const std::string &name, ULONG64 base, ULONG size ) :
    m_name( name ),
    m_base( base ),
    m_end( base + size )
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

    return 0;    
}

/////////////////////////////////////////////////////////////////////////////////

void
dbgModuleClass::getImagePath()
{
    HRESULT         hres;
    PWSTR           pathBuffer = NULL;
         
    try {
    
        ULONG       pathSize = 0;
        hres = dbgExt->symbols3->GetSymbolPathWide( NULL, 0, &pathSize );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbol3::GetImagePathWide  failed" );
            
        pathBuffer = new WCHAR [ pathSize ];
        
        hres = dbgExt->symbols3->GetSymbolPathWide( pathBuffer, pathSize, NULL );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbol3::GetImagePathWide  failed" );
            
        std::wstring   symPath( pathBuffer, pathSize );
        
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

    if ( pathBuffer )
        delete[] pathBuffer;
}

/////////////////////////////////////////////////////////////////////////////////