#pragma once

#include <string>

#include "dbgobj.h"
#include "diawrapper.h"
#include "typeinfo.h"
#include "typedvar.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class Module : private DbgObject {

public:
    
    Module( IDebugClient4 *client, const std::string& moduleName );

    Module( IDebugClient4 *client, ULONG64 offset );

    std::string  getName() {
        return m_name;
    }

    std::string getImageName() {
        return m_imageName;
    }

    ULONG64  getBase() {
        return m_base;
    }

    ULONG64 getEnd() {
        return m_base + m_size;
    }

    ULONG  getSize() {
        return m_size;
    }

    std::string
    getPdbName();

    void
    reloadSymbols();
   
    ULONG64
    getSymbol( const std::string &symbolname ) {
        pyDia::SymbolPtr   sym = getDia()->getChildByName( symbolname );

        return m_base + sym->getRva();
    }

    ULONG
    getSymbolRva( const std::string &symbolname ) {
        pyDia::SymbolPtr   sym = getDia()->getChildByName( symbolname );

        return sym->getRva();
    }

    TypeInfoPtr getTypeByName( const std::string &typeName ) {
        return TypeInfo::getTypeInfo( boost::static_pointer_cast<pyDia::Symbol>( getDia() ), typeName);
    }

    TypedVarPtr getTypedVarByTypeName( const std::string &typeName, ULONG64 addr );

    TypedVarPtr getTypedVarByType( const TypeInfoPtr  &typeInfo, ULONG64 addr );

    TypedVarPtr getTypedVarByAddr( ULONG64 addr );

    TypedVarPtr getTypedVarByName( const std::string &symName );

private:

    pyDia::GlobalScopePtr& getDia() {
        if (!m_dia)
            m_dia = pyDia::GlobalScope::loadPdb( getPdbName() );
        return m_dia;
    }


    std::string             m_name;
    std::string             m_imageName;
    ULONG64                 m_base;
    ULONG                   m_size;
    pyDia::GlobalScopePtr   m_dia;
};

///////////////////////////////////////////////////////////////////////////////////

Module loadModuleByName( const std::string  &moduleName ) ;

Module loadModuleByOffset( ULONG64  offset );

///////////////////////////////////////////////////////////////////////////////////

}; // end pykd namespace


