
#pragma once

#include "dia\diadecls.h"
#include "dia\diasymbol.h"

namespace pykd {

////////////////////////////////////////////////////////////////////////////

class DiaSession : public SymbolSession
{
public:

    DiaSession( IDiaSession* session, IDiaSymbol *globalScope, const std::string symbolFile ) :
        m_globalScope( globalScope ),
        m_globalSymbol( DiaSymbol::fromGlobalScope( globalScope ) ),
        m_session( session ),
        m_symbolFileName( symbolFile )
        {}

    virtual SymbolPtr getSymbolScope() {
        return m_globalSymbol;
    }

    virtual SymbolPtr findByRva( ULONG rva, ULONG symTag = SymTagNull, LONG* displacement = NULL );

    virtual void getSourceLine( ULONG64 offset, std::string &fileName, ULONG &lineNo, LONG &displacement );

    virtual std::string getSymbolFileName() {
        return m_symbolFileName;
    }

private:

    ULONG findRvaByName( const std::string &name );

    DiaSymbolPtr    m_globalScope;
    SymbolPtr       m_globalSymbol;
    DiaSessionPtr   m_session;
    std::string     m_symbolFileName;

};

////////////////////////////////////////////////////////////////////////////

} // end pykd namespace

