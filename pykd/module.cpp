#include "stdafx.h"
#include "dbgengine.h"
#include "module.h"
#include "dbgexcept.h"
#include "vardata.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

ModulePtr Module::loadModuleByName( const std::string  &moduleName ) {
    return ModulePtr( new Module( moduleName ) );
};

ModulePtr Module::loadModuleByOffset( ULONG64  offset ) {
    return ModulePtr( new Module( offset ) );
}

/////////////////////////////////////////////////////////////////////////////////////

Module::Module(const std::string &moduleName )
{
    m_base = findModuleBase( moduleName );
    m_name = moduleName;
    m_symfile = getModuleSymbolFileName( m_base );
    m_imageName = getModuleImageName( m_base );
    m_timeDataStamp = getModuleTimeStamp( m_base );
    m_checkSum = getModuleCheckSum( m_base );
}

/////////////////////////////////////////////////////////////////////////////////////

Module::Module(ULONG64 offset )
{
    m_base = findModuleBase( addr64(offset) );
    m_name = getModuleName( m_base );
    m_symfile = getModuleSymbolFileName( m_base );
    m_imageName = getModuleImageName( m_base );
    m_timeDataStamp = getModuleTimeStamp( m_base );
    m_checkSum = getModuleCheckSum( m_base );
}

/////////////////////////////////////////////////////////////////////////////////////

SymbolPtr& Module::getSymScope()
{
    do {

        if ( m_symScope )
            break;

        if ( m_symfile.empty() )
            break;

        m_symScope = loadSymbolFile( m_symfile, m_base );

    } while( false );

    if ( !m_symScope )
        throw SymbolException( "failed to find symbol file" );

    return m_symScope;
}

/////////////////////////////////////////////////////////////////////////////////////

void Module::reloadSymbols()
{
    m_symfile = getModuleSymbolFileName( m_base );
    m_symScope.reset();
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG Module::getRvaByName(const std::string &symName)
{
    SymbolPtr  &symScope = getSymScope();
    SymbolPtr  child = symScope->getChildByName( symName );
    return child->getRva();
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG64 
Module::getSymbolSize( const std::string &symName )
{
    try {

        SymbolPtr  &symScope = getSymScope();

        SymbolPtr  child = symScope->getChildByName( symName );

        if ( child->getSymTag() == SymTagData )
            return child->getSize();

    } catch( const SymbolException& )
    {
    }

    return getTypeByName(symName)->getSize();
}

/////////////////////////////////////////////////////////////////////////////////////

std::string Module::print()
{
    std::stringstream   sstr;

    sstr << "Module: " << m_name <<  std::endl;
    sstr << "Start: " << std::hex << m_base << " End: " << getEnd() << " Size: " << m_size << std::endl;
    sstr << "Image: " << m_imageName << std::endl;
    sstr << "Symnol: " << m_symfile << std::endl;
    sstr << "Timestamp: " << m_timeDataStamp << std::endl;
    sstr << "Check Sum: " << m_checkSum << std::endl;

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr 
Module::getTypedVarByAddr( ULONG64 offset )
{
    offset = addr64(offset);

    if ( offset < m_base || offset > getEnd() )
        throw DbgException( "address is out of the module space" );

    std::string  symName = getSymbolByOffset( offset );

    return getTypedVarByName( symName );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr
Module::getTypedVarByName( const std::string &symName )
{
    SymbolPtr  symVar = getSymScope()->getChildByName( symName );

    TypeInfoPtr typeInfo = TypeInfo::getTypeInfo( symVar->getType() );

    if ( LocIsConstant != symVar->getLocType() )
    {
        ULONG64  offset = getSymbol( symName );
        return TypedVar::getTypedVar(typeInfo, VarDataMemory::factory(offset) );
    }

    return TypedVar::getTypedVar( typeInfo, VarDataConst::factory(symVar) );

    


    //HRESULT     hres;

    //pyDia::SymbolPtr  symVar = getSymScope()->getChildByName( symName );

    //std::string     fullName = m_name;
    //fullName += '!';
    //fullName += symName;

    //ULONG64   offset;

    //hres = m_symbols->GetOffsetByName( fullName.c_str(), &offset );

    //TypeInfoPtr typeInfo = TypeInfo::getTypeInfo( symVar->getType() );

    //if ( FAILED( hres ) )
    //{
    //    if ( LocIsConstant == symVar->getLocType() )
    //        return TypedVar::getTypedVar( m_client, typeInfo, VarDataConst::factory(m_control, symVar) );
    //    throw DbgException("IDebugSymbols::GetOffsetByName failed" );
    //}

    //return TypedVar::getTypedVar( m_client, typeInfo, VarDataMemory::factory(m_dataSpaces, offset) );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr 
Module::getTypedVarByTypeName( const std::string &typeName, ULONG64 offset )
{
    TypeInfoPtr typeInfo = getTypeByName( typeName );

    return TypedVar::getTypedVar(typeInfo, VarDataMemory::factory(offset) );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr Module::containingRecordByName( ULONG64 offset, const std::string &typeName, const std::string &fieldName )
{
    TypeInfoPtr     typeInfo = getTypeByName( typeName );

    return containingRecordByType( offset, typeInfo, fieldName );
}

///////////////////////////////////////////////////////////////////////////////////

python::list Module::getTypedVarArrayByTypeName( ULONG64 offset, const std::string  &typeName, ULONG number )
{
    return getTypedVarArrayByType( offset, getTypeByName( typeName ), number );
}

///////////////////////////////////////////////////////////////////////////////////

python::list Module::getTypedVarListByTypeName( ULONG64 listHeadAddress, const std::string  &typeName, const std::string &listEntryName )
{
    return getTypedVarListByType( listHeadAddress, getTypeByName( typeName ), listEntryName );
}

///////////////////////////////////////////////////////////////////////////////////

}; // end of namespace pykd


