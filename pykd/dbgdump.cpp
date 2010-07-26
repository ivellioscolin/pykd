#include "stdafx.h"

#include "dbgext.h"
#include "dbgdump.h"
#include "dbgexcept.h"
#include "dbgsession.h"
#include "dbgsystem.h"

/////////////////////////////////////////////////////////////////////////////////

std::string
dbgLoadDump( const std::string &fileName )
{
    HRESULT     hres;
    
    wchar_t     *fileNameW = NULL;

    try {
    
        fileNameW = new wchar_t [ fileName.size()+ 1 ];
    
        MultiByteToWideChar(
            CP_ACP,            
            0,
            fileName.c_str(),
            fileName.size() + 1,
            fileNameW,
            fileName.size() + 1 );
    
        hres = dbgExt->client4->OpenDumpFileWide( fileNameW, NULL );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugClient4::OpenDumpFileWide failed" );
            
        hres = dbgExt->control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
        if ( FAILED( hres ) )
            throw DbgException( "IDebugControl::WaitForEvent failed" );   
            
        dbgSessionStarted = true;                    
        
        if ( fileNameW )
            delete[] fileNameW;
            
        return "loaded ok";                              
    }
 	catch( std::exception& )
	{
	    //g_Ext->Out( "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		//g_Ext->Out( "pykd unexpected error\n" );
	}	    

	std::string  result = "failed to open dump ";
	result += fileName;
	
    if ( fileNameW )
        delete[] fileNameW;	
	
    return result;
}

///////////////////////////////////////////////////////////////////////////////// 