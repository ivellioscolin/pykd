#pragma once

#include "dbgext.h"
#include "dbgeventcb.h"

///////////////////////////////////////////////////////////////////////////////

class dbgClient {

public:

    dbgClient() 
    {
        IDebugClient4     *client = NULL;
        DebugCreate( __uuidof(IDebugClient4), (void **)&client );
    
        m_ext = new DbgExt( client );
        
        client->Release();
    }
    
    ~dbgClient() 
    {
        removeEventsMgr();
            
        delete m_ext;
    }
    
    void startEventsMgr() {
 
         m_callbacks = new DbgEventCallbacksManager( m_ext->client );    
    }
    
    void removeEventsMgr() {
    
        if ( m_callbacks )
        {
            delete m_callbacks;
            m_callbacks = NULL;
        }
    }
    
private:

    DbgExt                          *m_ext;
    DbgEventCallbacksManager        *m_callbacks;  
};

extern dbgClient    g_dbgClient;

///////////////////////////////////////////////////////////////////////////////