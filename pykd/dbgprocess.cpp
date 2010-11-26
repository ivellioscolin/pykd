#include "stdafx.h"

#include "dbgprocess.h"
#include "dbgext.h"
#include "dbgexcept.h"

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
getThreadList()
{
    HRESULT         hres;  
    PULONG          ids = NULL;
    ULONG           i;
    ULONG           oldThreadId = 0;

    try {

        ULONG   threadCount;
        hres = dbgExt->system->GetNumberThreads( &threadCount );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSystemObjects::GetNumberThreads failed" );
            
        ids = new ULONG[threadCount];            
        hres = dbgExt->system->GetThreadIdsByIndex( 0, threadCount, ids, NULL );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSystemObjects::GetThreadIdsByIndex failed" );
        
        hres = dbgExt->system->GetCurrentThreadId( &oldThreadId );
        
        boost::python::list     threadList;
        
        for ( i = 0; i < threadCount; ++i )
        {
            dbgExt->system->SetCurrentThreadId( ids[i] );

            ULONG64   threadOffset;                
            hres = dbgExt->system->GetCurrentThreadDataOffset( &threadOffset );
            
            if ( FAILED( hres ) )
                throw DbgException( "IDebugSystemObjects::GetCurrentThreadDataOffset failed" );
                
            threadList.append( threadOffset );            
        }
        
        if ( ids )
            delete[] ids;
        
        return threadList;  
    }
  	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}
	
	if ( oldThreadId )
        dbgExt->system->SetCurrentThreadId( oldThreadId );
        
    if ( ids )
        delete[] ids;        
	
	return  boost::python::list();
}

/////////////////////////////////////////////////////////////////////////////////
