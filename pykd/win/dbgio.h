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

    void flush() {
    }

    std::string
    encoding() {
        return "ascii";
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

    void flush() {
    }

    std::string
    encoding() {
        return "ascii";
    }
};

///////////////////////////////////////////////////////////////////////////////


class DbgIn {

public:

    std::wstring
    readline() {
        return dreadline();
    }

    std::string
    encoding() {
        return "ascii";
    }

};

///////////////////////////////////////////////////////////////////////////////

class OutputReader : public IDebugOutputCallbacksWide, private boost::noncopyable {

public:

    explicit OutputReader( IDebugClient4 *client ) 
    {
        HRESULT   hres;

        m_client = client;

        hres = m_client->GetOutputCallbacksWide( &m_previousCallback );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugClient::GetOutputCallbacks failed" );

        hres = m_client->SetOutputCallbacksWide( this );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugClient::GetOutputCallbacks failed" );
    }

    ~OutputReader() 
    {
        m_client->SetOutputCallbacksWide( m_previousCallback );
    }

    const std::wstring&
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
        __in PCWSTR Text )
   {
        if ( Mask == DEBUG_OUTPUT_NORMAL )
        {
            m_readLine += std::wstring( Text );
        }
       return S_OK;
   }

private:

    std::wstring                        m_readLine;

    CComPtr<IDebugOutputCallbacksWide>  m_previousCallback;

    CComQIPtr<IDebugClient5>            m_client;
};

///////////////////////////////////////////////////////////////////////////////

} // end pykd namespace
