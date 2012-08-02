#pragma once

#include "intbase.h"
#include "symengine.h"
#include "typeinfo.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class Module;
typedef boost::shared_ptr<Module>  ModulePtr;

///////////////////////////////////////////////////////////////////////////////////

class Module : public intBase {

public:

    static
    ModulePtr loadModuleByName( const std::string &name );

    static
    ModulePtr loadModuleByOffset( ULONG64 offset );

public:

    Module(const std::string &name );

    Module(ULONG64 offset );

    std::string  getName() {
        return m_name;
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

    std::string getSymFile() const {
        return m_symfile;
    }

    std::string getImageName() const {
        return m_imageName;
    }

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

    ULONG getCheckSum() const {
        return m_checkSum;
    }

    ULONG getTimeDataStamp() const {
        return m_timeDataStamp;
    }

    TypeInfoPtr getTypeByName( const std::string &typeName ) {
        return TypeInfo::getTypeInfo( boost::static_pointer_cast<Symbol>( getSymScope() ), typeName);
    }

    ULONG64 getSymbolSize( const std::string &symName );

    std::string print();

private:

    SymbolPtr& getSymScope();

    BaseTypeVariant getValue() {
        return BaseTypeVariant(m_base);
    }

    ULONG getRvaByName(const std::string &symName);

    std::string             m_name;
    std::string             m_imageName;
    std::string             m_symfile;
    ULONG64                 m_base;
    ULONG                   m_size;
    ULONG                   m_timeDataStamp;
    ULONG                   m_checkSum;

    SymbolPtr               m_symScope;
};

///////////////////////////////////////////////////////////////////////////////////

}; // end pykd namespace






//#include <string>
//
//#include "dbgobj.h"
//#include "diawrapper.h"
//#include "typeinfo.h"
//#include "typedvar.h"
//#include "synsymbol.h"
//
//namespace pykd {
//
/////////////////////////////////////////////////////////////////////////////////////
//
//class Module;
//typedef boost::shared_ptr<Module>  ModulePtr;
//
/////////////////////////////////////////////////////////////////////////////////////
//
//class Module :  public intBase, private DbgObject {
//
//public:
//
//    static
//    ModulePtr loadModuleByName( const std::string &name );
//
//    static
//    ModulePtr loadModuleByOffset( ULONG64 offset );
//
//public:
//    
//    Module( IDebugClient4 *client, SynSymbolsPtr synSymbols, const std::string& moduleName );
//
//    Module( IDebugClient4 *client, SynSymbolsPtr synSymbols, ULONG64 offset );
//
//    std::string  getName() {
//        return m_name;
//    }
//
//    std::string getImageName() {
//        return m_imageName;
//    }
//
//    ULONG64  getBase() const {
//        return m_base;
//    }
//
//    ULONG64 getEnd() const {
//        return m_base + m_size;
//    }
//
//    ULONG  getSize() const {
//        return m_size;
//    }
//
//    std::string
//    getPdbName();
//
//    void
//    reloadSymbols();
//
//    ULONG64
//    getSymbol( const std::string &symbolname ) {
//        return m_base + getRvaByName(symbolname);
//    }
//
//    ULONG
//    getSymbolRva( const std::string &symbolname ) {
//        return getRvaByName(symbolname);
//    }
//
//    TypeInfoPtr getTypeByName( const std::string &typeName ) {
//        return TypeInfo::getTypeInfo( boost::static_pointer_cast<pyDia::Symbol>( getDia() ), typeName);
//    }
//
//    TypedVarPtr getTypedVarByTypeName( const std::string &typeName, ULONG64 addr );
//
//    TypedVarPtr getTypedVarByType( const TypeInfoPtr  &typeInfo, ULONG64 addr );
//
//    TypedVarPtr getTypedVarByAddr( ULONG64 addr );
//
//    TypedVarPtr getTypedVarByName( const std::string &symName );
//
//    TypedVarPtr containingRecordByName( ULONG64 addr, const std::string &typeName, const std::string &fieldName );
//
//    TypedVarPtr containingRecordByType( ULONG64 addr, const TypeInfoPtr &typeInfo, const std::string &fieldName );
//
//    python::list getTypedVarListByTypeName( ULONG64 listHeadAddres, const std::string  &typeName, const std::string &listEntryName );
//
//    python::list getTypedVarListByType( ULONG64 listHeadAddres, const TypeInfoPtr &typeInfo, const std::string &listEntryName );
//
//    python::list getTypedVarArrayByTypeName( ULONG64 addr, const std::string  &typeName, ULONG number );
//
//    python::list getTypedVarArrayByType( ULONG64 addr, const TypeInfoPtr &typeInfo, ULONG number );
//
//    ULONG64 getSymbolSize( const std::string &symName );
//
//    pyDia::GlobalScopePtr& getDia() {
//        if (!m_dia)
//        {
//            m_dia = pyDia::GlobalScope::loadPdb( getPdbName() );
//            if ( m_dia )
//            {
//                m_dia->setLoadAddress( m_base ); 
//            }
//        }    
//
//        return m_dia;
//    }
//
//    ULONG getCheckSum() const {
//        return m_checkSum;
//    }
//
//    ULONG getTimeDataStamp() const {
//        return m_timeDataStamp;
//    }
//
//    std::string print();
//
//private:
//
//    void
//    reloadSymbolsImpl();
//
//
//    ULONG getRvaByName(const std::string &symName);
//
//    BaseTypeVariant getValue() {
//        return BaseTypeVariant(m_base);
//    }
//
//
//    std::string             m_name;
//    std::string             m_imageName;
//    ULONG64                 m_base;
//    ULONG                   m_size;
//
//    pyDia::GlobalScopePtr   m_dia;
//
//    ULONG                   m_timeDataStamp;
//    ULONG                   m_checkSum;
//    SynSymbolsPtr           m_synSymbols;
//};
//
/////////////////////////////////////////////////////////////////////////////////////
//
//}; // end pykd namespace


