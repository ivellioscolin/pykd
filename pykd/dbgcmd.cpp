#include "stdafx.h"

#include <engextcpp.hpp>

#include "dbgcmd.h"
#include "dbgexcept.h"

///////////////////////////////////////////////////////////////////////////////

// класс для перехвата вывода в отладчик

class OutputReader : public IDebugOutputCallbacks {

public:

	OutputReader( IDebugClient  *debugClient ) 
	{
		HRESULT   hres;

		try {

			m_debugClient = debugClient;
			m_debugClient->AddRef();

			hres = m_debugClient->GetOutputCallbacks( &m_previousCallback );
			if ( FAILED( hres ) )
			{
				throw hres;
			}

			hres = m_debugClient->SetOutputCallbacks( this );
			if ( FAILED( hres ) )
			{
				throw hres;
			}

		} catch( ... )
		{
			m_debugClient->Release();
			m_debugClient = NULL;
		}
	}

	~OutputReader() 
	{
		if ( m_debugClient )
		{
			m_debugClient->SetOutputCallbacks( m_previousCallback );
			m_debugClient->Release();
		}
	}

	const std::string&
	Line() const {
		return  m_readLine;
	}

private:

	 // IUnknown.
    STDMETHOD(QueryInterface)(
        __in REFIID InterfaceId,
		__out PVOID* Interface ) {
			return E_NOINTERFACE;
	}

	STDMETHOD_(ULONG, AddRef)() {
		return 1L;
	}


	STDMETHOD_(ULONG, Release)() {
		return 0L;
	}

   STDMETHOD(Output)(
        __in ULONG Mask,
        __in PCSTR Text )
   {
		if ( Mask == DEBUG_OUTPUT_NORMAL )
		{
			m_readLine += std::string( Text );
		}

	   return S_OK;
   }

private:

	std::string               m_readLine;

	IDebugClient             *m_debugClient;

	IDebugOutputCallbacks    *m_previousCallback;
};


///////////////////////////////////////////////////////////////////////////////

std::string
dbgCommand( const std::string &command )
{
    HRESULT     hres;

    try {
    
        OutputReader    outReader(  g_Ext->m_Client );
    
        hres = g_Ext->m_Control->Execute( DEBUG_OUTCTL_THIS_CLIENT, command.c_str(), 0 );
        if ( FAILED( hres ) )
            throw  DbgException( "IDebugControl::Execute  failed" ); 
        
        return std::string( outReader.Line() );
    }    
	catch( std::exception  &e )
	{
		g_Ext->Out( "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		g_Ext->Out( "pykd unexpected error\n" );
	}	
	
	return "error"; 
}	

///////////////////////////////////////////////////////////////////////////////