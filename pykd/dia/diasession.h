
#pragma once

#include "dia\diadecls.h"
#include "dia\diasymbol.h"

namespace pykd {

////////////////////////////////////////////////////////////////////////////

class DiaSession : public SymbolSession
{
public:

    DiaSession( IDiaSession* session, IDiaSymbol *globalScope, const std::string symbolFile, LONGLONG loadSeconds );

    virtual SymbolPtr getSymbolScope() {
        return m_globalSymbol;
    }

    virtual SymbolPtr findByRva( ULONG rva, ULONG symTag = SymTagNull, LONG* displacement = NULL );

    virtual void getSourceLine( ULONG64 offset, std::string &fileName, ULONG &lineNo, LONG &displacement );

    virtual std::string getSymbolFileName() {
        return m_symbolFileName;
    }

    virtual std::string getBuildDescription() const;

private:

    ULONG findRvaByName( const std::string &name );

    DiaSymbolPtr    m_globalScope;
    SymbolPtr       m_globalSymbol;
    DiaSessionPtr   m_session;
    std::string     m_symbolFileName;
    LONGLONG        m_loadSeconds;
};

////////////////////////////////////////////////////////////////////////////

} // end pykd namespace

