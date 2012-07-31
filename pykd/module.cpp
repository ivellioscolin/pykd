#include "stdafx.h"
#include "dbgengine.h"
#include "module.h"
#include "dbgexcept.h"

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

        m_symScope = loadSymbolFile( m_symfile );

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

std::string Module::print()
{
    std::stringstream   sstr;

    sstr << "Module: " << m_name <<  std::endl;
    sstr << "Start: " << std::hex << m_base << " End: " << getEnd() << " Size: " << m_size << std::endl;
    sstr << "Image: " << m_imageName << std::endl;
    sstr << "Symnol: " << m_symfile << std::endl;
    //sstr << "Timestamp: " << m_timeDataStamp << std::endl;
    //sstr << "Check Sum: " << m_checkSum << std::endl;

    return sstr.str();
}

/////////////////////////////////////////////////////////////////////////////////////

}; // end of namespace pykd

















//#include "module.h"
//#include "dbgclient.h"
//#include "dbgmem.h"
//
//namespace pykd {
//
/////////////////////////////////////////////////////////////////////////////////////
//
//ModulePtr Module::loadModuleByName( const std::string  &moduleName ) {
//    return g_dbgClient->loadModuleByName( moduleName );  
//};
//
//ModulePtr Module::loadModuleByOffset( ULONG64  offset ) {
//    return g_dbgClient->loadModuleByOffset( offset );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//Module::Module( IDebugClient4 *client, SynSymbolsPtr synSymbols, const std::string& moduleName ) 
//    : DbgObject( client )
//    , m_synSymbols(synSymbols)
//{
//    HRESULT    hres;
//
//    m_name = moduleName;
//
//    hres = m_symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &m_base );
//    if ( FAILED( hres ) )
//        throw DbgException( "IDebugSymbol::GetModuleByModuleName  failed" ); 
//
//    DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };
//    hres = m_symbols->GetModuleParameters( 1, &m_base, 0, &moduleParam );
//    if ( FAILED( hres ) )
//         throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );    
//
//    m_size = moduleParam.Size;
//    m_timeDataStamp = moduleParam.TimeDateStamp;
//    m_checkSum = moduleParam.Checksum;
//
//    char imageName[0x100];
//
//    hres = m_symbols->GetModuleNameString( 
//        DEBUG_MODNAME_IMAGE,
//        DEBUG_ANY_ID,
//        m_base,
//        imageName,
//        sizeof( imageName ),
//        NULL );
//
//    if ( FAILED( hres ) )
//        throw DbgException( "IDebugSymbol::GetModuleNameString failed" );
//
//    m_imageName = std::string( imageName );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//Module::Module( IDebugClient4 *client, SynSymbolsPtr synSymbols, ULONG64 offset ) 
//    : DbgObject( client )
//    , m_synSymbols(synSymbols)
//{
//    HRESULT     hres;
//
//    offset = addr64( offset );
//
//    ULONG       moduleIndex;
//    hres = m_symbols->GetModuleByOffset( offset, 0, &moduleIndex, &m_base );
//    if ( FAILED( hres ) )
//        throw DbgException( "IDebugSymbol::GetModuleByOffset failed" );
//
//    char  moduleName[0x100];
//
//    hres = m_symbols->GetModuleNameString( 
//        DEBUG_MODNAME_MODULE,
//        moduleIndex,
//        0,
//        moduleName,
//        sizeof( moduleName ),
//        NULL );
//
//    if ( FAILED( hres ) )
//        throw DbgException( "IDebugSymbol::GetModuleNameString failed" );
//
//    m_name = std::string( moduleName );
//
//    char imageName[0x100];
//
//    hres = m_symbols->GetModuleNameString( 
//        DEBUG_MODNAME_IMAGE,
//        DEBUG_ANY_ID,
//        m_base,
//        imageName,
//        sizeof( imageName ),
//        NULL );
//
//    if ( FAILED( hres ) )
//        throw DbgException( "IDebugSymbol::GetModuleNameString failed" );
//
//    m_imageName = std::string( imageName );
//
//    DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };
//    hres = m_symbols->GetModuleParameters( 1, &m_base, 0, &moduleParam );
//    if ( FAILED( hres ) )
//         throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );    
//
//    m_size = moduleParam.Size;
//    m_timeDataStamp = moduleParam.TimeDateStamp;
//    m_checkSum = moduleParam.Checksum;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//std::string
//Module::getPdbName()
//{
//    HRESULT         hres;
//
//    IMAGEHLP_MODULEW64      moduleInfo = {};
//
//    hres = m_advanced->GetSymbolInformation(
//        DEBUG_SYMINFO_IMAGEHLP_MODULEW64,
//        m_base,
//        0,
//        &moduleInfo,
//        sizeof(moduleInfo),
//        NULL,
//        NULL,
//        0,
//        NULL );
//
//    if ( FAILED( hres ) )
//        throw DbgException( "IDebugAdvanced2::GetSymbolInformation failed" );
//
//    if (!*moduleInfo.LoadedPdbName)
//    {
//        reloadSymbolsImpl();
//
//        hres = m_advanced->GetSymbolInformation(
//            DEBUG_SYMINFO_IMAGEHLP_MODULEW64,
//            m_base,
//            0,
//            &moduleInfo,
//            sizeof(moduleInfo),
//            NULL,
//            NULL,
//            0,
//            NULL );
//
//        if ( FAILED( hres ) )
//            throw DbgException( "IDebugAdvanced2::GetSymbolInformation failed" );
//    }
//
//    char  pdbName[ 256 ];
//    WideCharToMultiByte( CP_ACP, 0, moduleInfo.LoadedPdbName, 256, pdbName, 256, NULL, NULL );
//
//    return std::string( pdbName );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//void
//Module::reloadSymbolsImpl()
//{
//    HRESULT     hres;
//
//    std::string  param = "/f ";
//    param += m_imageName;
//
//    hres = m_symbols->Reload( param.c_str() );
//    if ( FAILED( hres ) )
//        throw DbgException("IDebugSymbols::Reload failed" );
//}
//
//
/////////////////////////////////////////////////////////////////////////////////////
//
//void
//Module::reloadSymbols()
//{
//    reloadSymbolsImpl();
//
//    m_dia.reset();
//    m_dia = pyDia::GlobalScope::loadPdb( getPdbName() );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//TypedVarPtr
//Module::getTypedVarByTypeName( const std::string &typeName, ULONG64 addr )
//{
//    return TypedVar::getTypedVar( m_client, getTypeByName(typeName), VarDataMemory::factory(m_dataSpaces, addr) );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//TypedVarPtr
//Module::getTypedVarByType( const TypeInfoPtr &typeInfo, ULONG64 addr )
//{
//   return TypedVar::getTypedVar( m_client, typeInfo, VarDataMemory::factory(m_dataSpaces, addr) );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//ULONG64 
//Module::getSymbolSize( const std::string &symName )
//{
//    try {
//
//        pyDia::SymbolPtr  symVar = getDia()->getChildByName( symName );
//
//        if ( symVar->getSymTag() == SymTagData )
//            return symVar->getSize();
//
//    } catch( const SymbolException& )
//    {
//    }
//
//    return getTypeByName(symName)->getSize();
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//TypedVarPtr
//Module::getTypedVarByName( const std::string &symName )
//{
//    HRESULT     hres;
//
//    pyDia::SymbolPtr  symVar = getDia()->getChildByName( symName );
//
//    std::string     fullName = m_name;
//    fullName += '!';
//    fullName += symName;
//
//    ULONG64   offset;
//
//    hres = m_symbols->GetOffsetByName( fullName.c_str(), &offset );
//
//    TypeInfoPtr typeInfo = TypeInfo::getTypeInfo( symVar->getType() );
//
//    if ( FAILED( hres ) )
//    {
//        if ( LocIsConstant == symVar->getLocType() )
//            return TypedVar::getTypedVar( m_client, typeInfo, VarDataConst::factory(m_control, symVar) );
//        throw DbgException("IDebugSymbols::GetOffsetByName failed" );
//    }
//
//    return TypedVar::getTypedVar( m_client, typeInfo, VarDataMemory::factory(m_dataSpaces, offset) );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//TypedVarPtr
//Module::getTypedVarByAddr( ULONG64 addr )
//{
//    HRESULT     hres;
//
//    addr = addr64(addr);
//
//    if ( addr < m_base || addr > getEnd() )
//        throw DbgException( "address is out of the module space" );
//
//    char    nameBuf[0x100];
//
//    hres = 
//        m_symbols->GetNameByOffset(
//            addr,
//            nameBuf,
//            sizeof(nameBuf),
//            NULL,
//            NULL );
//
//    std::string     fullName( nameBuf );
//
//    size_t          symPos = fullName.find ( '!' ) + 1;
//
//    std::string     symbolName;
//    symbolName.assign( fullName, symPos, fullName.length() - symPos );
//
//    if ( FAILED(hres) )
//        throw DbgException( "failed IDebugSymbols::GetNameByOffset" );
//
//    return getTypedVarByName( symbolName );
//
//    //LONG displacement;
//    //pyDia::SymbolPtr diaSym = 
//    //    getDia()->findByRvaImpl((ULONG)(addr - m_base), SymTagData, displacement);
//    //if (displacement)
//    //    throw DbgException( "not exactly match by RVA" );
//
//    //return TypedVar::getTypedVar( m_client, TypeInfo::getTypeInfo( diaSym->getType() ), addr ); 
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//ULONG Module::getRvaByName(const std::string &symName)
//{
//    HRESULT     hres;
//    ULONG64     offset;
//
//    hres = m_symbols->GetOffsetByName( symName.c_str(), &offset );
//    if ( SUCCEEDED(hres) )
//        return (ULONG)(offset - m_base);
//
//    return (ULONG)m_synSymbols->getRvaByName(m_timeDataStamp, m_checkSum, symName);
//
//    //try {
//    //    pyDia::SymbolPtr sym = getDia()->getChildByName( symName );
//    //    return sym->getRva();
//    //} 
//    //catch (const pyDia::Exception &) {
//    //}
//    //return (ULONG)m_synSymbols->getRvaByName(m_timeDataStamp, m_checkSumm, symName);
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//TypedVarPtr Module::containingRecordByName( ULONG64 address, const std::string &typeName, const std::string &fieldName )
//{
//    address = addr64(address); 
//
//    TypeInfoPtr     typeInfo = getTypeByName( typeName );
//
//    VarDataPtr varData = 
//        VarDataMemory::factory( 
//            m_dataSpaces,
//            address - typeInfo->getFieldOffsetByNameRecirsive( fieldName )
//        );
//
//    return TypedVar::getTypedVar( m_client, typeInfo, varData );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//TypedVarPtr Module::containingRecordByType( ULONG64 address, const TypeInfoPtr &typeInfo, const std::string &fieldName )
//{
//    address = addr64(address); 
//
//    VarDataPtr varData = 
//        VarDataMemory::factory( 
//            m_dataSpaces,
//            address - typeInfo->getFieldOffsetByNameRecirsive( fieldName )
//        );
//
//    return TypedVar::getTypedVar( m_client, typeInfo, varData );
//}
//
//
/////////////////////////////////////////////////////////////////////////////////////
//
//python::list Module::getTypedVarListByTypeName( ULONG64 listHeadAddress, const std::string  &typeName, const std::string &listEntryName )
//{
//    return getTypedVarListByType( listHeadAddress, getTypeByName( typeName ), listEntryName );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//python::list Module::getTypedVarListByType( ULONG64 listHeadAddress, const TypeInfoPtr &typeInfo, const std::string &listEntryName )
//{
//    python::list    lst;
//
//    listHeadAddress = addr64( listHeadAddress );
//
//    ULONG64                 entryAddress = 0;
//
//    TypeInfoPtr             fieldTypeInfo = typeInfo->getField( listEntryName );
//
//    if ( fieldTypeInfo->getName() == ( typeInfo->getName() + "*" ) )
//    {
//        for( entryAddress = ptrPtr( listHeadAddress, m_dataSpaces ); addr64(entryAddress) != listHeadAddress && entryAddress != NULL; entryAddress = ptrPtr( entryAddress + typeInfo->getFieldOffsetByNameRecirsive(listEntryName) ) )
//            lst.append( getTypedVarByType( typeInfo, entryAddress ) );
//    }
//    else
//    {
//        for( entryAddress = ptrPtr( listHeadAddress, m_dataSpaces ); addr64(entryAddress) != listHeadAddress && entryAddress != NULL; entryAddress = ptrPtr( entryAddress ) )
//            lst.append( containingRecordByType( entryAddress, typeInfo, listEntryName ) );
//    }
//
//    return lst;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//python::list Module::getTypedVarArrayByTypeName( ULONG64 addr, const std::string  &typeName, ULONG number )
//{
//    return getTypedVarArrayByType( addr, getTypeByName( typeName ), number );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//python::list Module::getTypedVarArrayByType( ULONG64 address, const TypeInfoPtr &typeInfo, ULONG number )
//{
//    address = addr64(address); 
//       
//    python::list     lst;
//    
//    for( ULONG i = 0; i < number; ++i )
//        lst.append( getTypedVarByType( typeInfo, address + i * typeInfo->getSize() ) );
//   
//    return lst;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//std::string Module::print()
//{
//    std::stringstream   sstr;
//
//    sstr << "Module: " << m_name <<  std::endl;
//    sstr << "Start: " << std::hex << m_base << " End: " << getEnd() << " Size: " << m_size << std::endl;
//    sstr << "Image: " << m_imageName << std::endl;
//    sstr << "Pdb: " << getPdbName() << std::endl;
//    sstr << "Timestamp: " << m_timeDataStamp << std::endl;
//    sstr << "Check Sum: " << m_checkSum << std::endl;
//
//    return sstr.str();
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//}; // end of namespace pykd
//
