#pragma once

#include "dbgobj.h"
#include <vector>

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

void dprint( const std::wstring &str, bool dml = false );

void dprintln( const std::wstring &str, bool dml = false );

void eprint( const std::wstring &str );

void eprintln( const std::wstring &str );

/////////////////////////////////////////////////////////////////////////////////

class DbgOut : private DbgObject {

public:

    DbgOut( IDebugClient4 *client ) : DbgObject( client )
    {}

    void
    write( const std::wstring  &str );    
};

/////////////////////////////////////////////////////////////////////////////////

class DbgIn : private DbgObject {

public:

    DbgIn( IDebugClient4 *client ) : DbgObject( client )
    {}

    std::string
    readline();
};

/////////////////////////////////////////////////////////////////////////////////

// класс для перехвата вывода в отладчик

class OutputReader :  private DbgObject,  public IDebugOutputCallbacks, private boost::noncopyable {

public:

    explicit OutputReader( IDebugClient4 *client ) : DbgObject( client )
	{
		HRESULT   hres;

        hres = m_client->GetOutputCallbacks( &m_previousCallback );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugClient::GetOutputCallbacks failed" );

		hres = m_client->SetOutputCallbacks( this );
		if ( FAILED( hres ) )
            throw DbgException( "IDebugClient::GetOutputCallbacks failed" );

	}

	~OutputReader() 
	{
	    m_client->SetOutputCallbacks( m_previousCallback );
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

	std::string                         m_readLine;

    CComPtr<IDebugOutputCallbacks>      m_previousCallback;
};

///////////////////////////////////////////////////////////////////////////////////
//
//class InputReader : public IDebugInputCallbacks,  private DbgObject,  private boost::noncopyable {
//
//public:
//
//    explicit InputReader( IDebugClient4  *debugClient ) : DbgObject( debugClient )
//    {
//    	HRESULT   hres;
//
//		hres = m_client->GetInputCallbacks( &m_previousCallback );
//		if ( FAILED( hres ) )
//            throw DbgException( "IDebugClient::GetInputCallbacks failed" );
//    
//        hres = m_client->SetInputCallbacks( this );
//    	if ( FAILED( hres ) )
//            throw DbgException( "IDebugClient::SetInputCallbacks failed" );
//    }
//    
//	~InputReader() 
//	{
//		if ( m_previousCallback )
//			m_client->SetInputCallbacks( m_previousCallback );
//	}
//    
//
//private:
//
//	 // IUnknown.
//    STDMETHOD(QueryInterface)(
//        __in REFIID InterfaceId,
//		__out PVOID* Interface ) {
//			return E_NOINTERFACE;
//	}
//
//	STDMETHOD_(ULONG, AddRef)() {
//		return 1L;
//	}
//
//
//	STDMETHOD_(ULONG, Release)() {
//		return 0L;
//	}
//
//    STDMETHOD( EndInput )() {
//        return S_OK;
//    }        
//    
//    STDMETHOD( StartInput )(
//        IN ULONG  BufferSize ) {
//
//            std::vector<char>   buf(BufferSize);
//
//            ULONG  inputSize;
//
//            m_control->Input( &buf[0], BufferSize, &inputSize );
//
//            m_control->ReturnInput( &buf[0] );
//
//            return S_OK;
//    }        
//        
//private:
//
//    CComPtr<IDebugInputCallbacks>   m_previousCallback;           
//
//};

/////////////////////////////////////////////////////////////////////////////////

}; 