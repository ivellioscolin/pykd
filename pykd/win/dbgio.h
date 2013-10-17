#pragma once

#include <dbgeng.h>
#include "dbgengine.h"
#include "dbgexcept.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

class DbgOut  {

public:

    void
    write( const std::wstring  &str ) {
        dprint( str );
    }

};

///////////////////////////////////////////////////////////////////////////////

class DbgErr {

public:

    void
    write( const std::wstring  &str )
    {
        eprint( str );
    }
};

///////////////////////////////////////////////////////////////////////////////


class DbgIn {

public:

    std::string
    readline() {
        return dreadline();
    }

    std::string
    encoding() {
        return "ascii";
    }

};

///////////////////////////////////////////////////////////////////////////////

class OutputReader : public IDebugOutputCallbacks, private boost::noncopyable {

public:

    explicit OutputReader( IDebugClient4 *client ) 
    {
        HRESULT   hres;

        m_client = client;

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

    CComPtr<IDebugClient4>              m_client;
};

///////////////////////////////////////////////////////////////////////////////

} // end pykd namespace
