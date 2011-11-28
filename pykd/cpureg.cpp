#include "stdafx.h"

#include "cpureg.h"
#include "dbgclient.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

python::object DebugClient::getRegByIndex( ULONG index )
{
    HRESULT         hres;

    DEBUG_VALUE    debugValue;            
    hres = m_registers->GetValue( index, &debugValue );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegister::GetValue  failed" );
        
    switch( debugValue.Type )
    {
    case DEBUG_VALUE_INT8:
        return boost::python::long_( debugValue.I8 );
        break;
        
    case DEBUG_VALUE_INT16:
        return boost::python::long_( debugValue.I16 );
        break;
        
    case DEBUG_VALUE_INT32:
        return boost::python::long_( debugValue.I32 );
        break;
        
    case DEBUG_VALUE_INT64:
        return boost::python::long_(debugValue.I64 );
        break;
   }

   throw DbgException( "Invalid register value" );  
}

python::object getRegByIndex( ULONG index )
{
    return g_dbgClient->getRegByIndex( index );  
}

///////////////////////////////////////////////////////////////////////////////////


python::object DebugClient::getRegByName( const std::wstring &regName )
{
    ULONG       registerIndex = 0;
    HRESULT     hres;

    hres = m_registers->GetIndexByNameWide( regName.c_str(), &registerIndex );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegister2::GetIndexByNameWide  failed" );

    return getRegByIndex( registerIndex );
}        

python::object getRegByName( const std::wstring &regName )
{
    return g_dbgClient->getRegByName( regName );
}

///////////////////////////////////////////////////////////////////////////////////

} // end namespace pykd