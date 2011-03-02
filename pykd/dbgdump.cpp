#include "stdafx.h"

#include <vector>

#include "dbgext.h"
#include "dbgdump.h"
#include "dbgexcept.h"
#include "dbgeventcb.h"
#include "dbgsession.h"
#include "dbgsystem.h"

/////////////////////////////////////////////////////////////////////////////////

std::string
dbgLoadDump( const std::string &fileName )
{
    HRESULT     hres;
    
    try {
    
	std::vector<wchar_t> fileNameW( fileName.size()+ 1 );
    
        MultiByteToWideChar(
            CP_ACP,            
            0,
            fileName.c_str(),
            (ULONG)fileName.size() + 1,
            &fileNameW[0],
            (ULONG)fileName.size() + 1 );
    
        hres = dbgExt->client4->OpenDumpFileWide( &fileNameW[0], NULL );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugClient4::OpenDumpFileWide failed" );
            
        hres = dbgExt->control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
        if ( FAILED( hres ) )
            throw DbgException( "IDebugControl::WaitForEvent failed" );   

        setDbgSessionStarted();

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
	
    return result;
}

///////////////////////////////////////////////////////////////////////////////// 