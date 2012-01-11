#pragma once

#include <string>

#include "dbgobj.h"
#include "diawrapper.h"
#include "typeinfo.h"
#include "typedvar.h"
#include "synsymbol.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class Module :  public intBase, private DbgObject {

public:
    
    Module( IDebugClient4 *client, SynSymbolsPtr synSymbols, const std::string& moduleName );

    Module( IDebugClient4 *client, SynSymbolsPtr synSymbols, ULONG64 offset );

    std::string  getName() {
        return m_name;
    }

    std::string getImageName() {
        return m_imageName;
    }

    ULONG64  getBase() const {
        return m_base;
    }

    ULONG64 getEnd() const {
        return m_base + m_size;
    }

    ULONG  getSize() const {
        return m_size;
    }

    std::string
    getPdbName();

    void
    reloadSymbols();
   
    ULONG64
    getSymbol( const std::string &symbolname ) {
        return m_base + getRvaByName(symbolname);
    }

    ULONG
    getSymbolRva( const std::string &symbolname ) {
        return getRvaByName(symbolname);
    }

    TypeInfoPtr getTypeByName( const std::string &typeName ) {
        return TypeInfo::getTypeInfo( boost::static_pointer_cast<pyDia::Symbol>( getDia() ), typeName);
    }

    TypedVarPtr getTypedVarByTypeName( const std::string &typeName, ULONG64 addr );

    TypedVarPtr getTypedVarByType( const TypeInfoPtr  &typeInfo, ULONG64 addr );

    TypedVarPtr getTypedVarByAddr( ULONG64 addr );

    TypedVarPtr getTypedVarByName( const std::string &symName );

    TypedVarPtr containingRecordByName( ULONG64 addr, const std::string &typeName, const std::string &fieldName );

    TypedVarPtr containingRecordByType( ULONG64 addr, const TypeInfoPtr &typeInfo, const std::string &fieldName );

    python::list getTypedVarListByTypeName( ULONG64 listHeadAddres, const std::string  &typeName, const std::string &listEntryName );

    python::list getTypedVarListByType( ULONG64 listHeadAddres, const TypeInfoPtr &typeInfo, const std::string &listEntryName );

    python::list getTypedVarArrayByTypeName( ULONG64 addr, const std::string  &typeName, ULONG number );

    python::list getTypedVarArrayByType( ULONG64 addr, const TypeInfoPtr &typeInfo, ULONG number );

    pyDia::GlobalScopePtr& getDia() {
        if (!m_dia)
            m_dia = pyDia::GlobalScope::loadPdb( getPdbName() );
        return m_dia;
    }

private:

    ULONG getRvaByName(const std::string &symName);

    BaseTypeVariant getValue() {
        return BaseTypeVariant(m_base);
    }


    std::string             m_name;
    std::string             m_imageName;
    ULONG64                 m_base;
    ULONG                   m_size;

    pyDia::GlobalScopePtr   m_dia;

    ULONG                   m_timeDataStamp;
    ULONG                   m_checkSumm;
    SynSymbolsPtr           m_synSymbols;
};

///////////////////////////////////////////////////////////////////////////////////

Module loadModuleByName( const std::string  &moduleName ) ;

Module loadModuleByOffset( ULONG64  offset );

///////////////////////////////////////////////////////////////////////////////////

}; // end pykd namespace


