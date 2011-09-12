#pragma once

//#include <string>
//#include <dbgeng.h>
//
//#include "dbgext.h"
//
///////////////////////////////////////////////////////////////////////////////////
//
//class dbgPrint {
//
//public:
//
//	static void dprint( const boost::python::object& obj, bool dml = false );
//	
//	static void dprintln( const boost::python::object& obj, bool dml = false );
//
//};
//
/////////////////////////////////////////////////////////////////////////////////
//
//// класс для перехвата вывода в отладчик
//
//class OutputReader : public IDebugOutputCallbacks {
//
//public:
//
//	OutputReader( IDebugClient  *debugClient ) 
//	{
//		HRESULT   hres;
//
//		try {
//
//			m_debugClient = debugClient;
//			m_debugClient->AddRef();
//
//			hres = m_debugClient->GetOutputCallbacks( &m_previousCallback );
//			if ( FAILED( hres ) )
//			{
//				throw hres;
//			}
//
//			hres = m_debugClient->SetOutputCallbacks( this );
//			if ( FAILED( hres ) )
//			{
//				throw hres;
//			}
//
//		} catch( ... )
//		{
//			m_debugClient->Release();
//			m_debugClient = NULL;
//		}
//	}
//
//	~OutputReader() 
//	{
//		if ( m_debugClient )
//		{
//			m_debugClient->SetOutputCallbacks( m_previousCallback );
//			m_debugClient->Release();
//		}
//	}
//
//	const std::string&
//	Line() const {
//		return  m_readLine;
//	}
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
//   STDMETHOD(Output)(
//        __in ULONG Mask,
//        __in PCSTR Text )
//   {
//		if ( Mask == DEBUG_OUTPUT_NORMAL )
//		{
//			m_readLine += std::string( Text );
//		}
//
//	   return S_OK;
//   }
//
//private:
//
//	std::string               m_readLine;
//
//	IDebugClient             *m_debugClient;
//
//	IDebugOutputCallbacks    *m_previousCallback;
//};
//
/////////////////////////////////////////////////////////////////////////////////
//
//class InputReader : public IDebugInputCallbacks {
//
//public:
//
//    InputReader( IDebugClient  *debugClient ) 
//    {
//    	HRESULT   hres;
//
//		try {
//
//			m_debugClient = debugClient;
//			m_debugClient->AddRef();
//
//			hres = m_debugClient->GetInputCallbacks( &m_previousCallback );
//			if ( FAILED( hres ) )
//			{
//				throw hres;
//			}
//
//			hres = m_debugClient->SetInputCallbacks( this );
//			if ( FAILED( hres ) )
//			{
//				throw hres;
//			}
//
//		} catch( ... )
//		{
//			m_debugClient->Release();
//			m_debugClient = NULL;
//		}    
//    }
//    
//	~InputReader() 
//	{
//		if ( m_debugClient )
//		{
//			m_debugClient->SetInputCallbacks( m_previousCallback );
//			m_debugClient->Release();
//		}
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
//            return S_OK;
//    }        
//        
//private:
//
//	IDebugClient            *m_debugClient;
//
//    IDebugInputCallbacks    *m_previousCallback;           
//
//};
//
//
///////////////////////////////////////////////////////////////////////////////////
//
//class dbgOut {
//
//public:
//
//    void
//    write( const boost::python::object  &str ) {
//        dbgPrint::dprint( str );
//    }         
//    
//};
//
///////////////////////////////////////////////////////////////////////////////////
//
//class dbgIn {
//
//public:
//
//    std::string
//    readline() {
//    
//        char        str[100];
//        ULONG       inputSize;
//        
//        OutputReader        outputReader( dbgExt->client );
//    
//        dbgExt->control->Input( str, sizeof(str), &inputSize );
//    
//        return std::string( str );
//    }    
//
//};
//
///////////////////////////////////////////////////////////////////////////////////
//
