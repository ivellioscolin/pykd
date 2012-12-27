#include "stdafx.h"
#include "dbgengine.h"
#include "module.h"
#include "dbgexcept.h"
#include "vardata.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

Module::SymbolSessionCache  Module::m_symSessionCache;

///////////////////////////////////////////////////////////////////////////////////

ModulePtr Module::loadModuleByName( const std::string  &moduleName ) 
{
    return ModulePtr( new Module( moduleName ) );
};

/////////////////////////////////////////////////////////////////////////////////////

ModulePtr Module::loadModuleByOffset( ULONG64  offset ) 
{
    return  ModulePtr( new Module( offset ) );
}

/////////////////////////////////////////////////////////////////////////////////////

Module::Module(const std::string &moduleName )
{
    m_base = findModuleBase( moduleName );
    m_name = moduleName;
    m_imageName = getModuleImageName( m_base );
    m_timeDataStamp = getModuleTimeStamp( m_base );
    m_checkSum = getModuleCheckSum( m_base );
    m_size = getModuleSize( m_base );
}

/////////////////////////////////////////////////////////////////////////////////////

Module::Module(ULONG64 offset )
{
    m_base = findModuleBase( addr64(offset) );
    m_name = getModuleName( m_base );
    m_imageName = getModuleImageName( m_base );
    m_timeDataStamp = getModuleTimeStamp( m_base );
    m_checkSum = getModuleCheckSum( m_base );
    m_size = getModuleSize( m_base );
}

/////////////////////////////////////////////////////////////////////////////////////

SymbolSessionPtr& Module::getSymSession()
{
    if (m_symSession)
        return m_symSession;

    SymbolMapKey cacheKey = { m_name, m_size, m_timeDataStamp, m_checkSum };
    SymbolSessionCache::iterator found = m_symSessionCache.find( cacheKey );
    if ( found != m_symSessionCache.end() )
    {
        m_symSession = found->second;
        if ( !m_symSession )
            throw SymbolException( "failed to load symbol file" );

        return m_symSession;
    }

    try
    {
        m_symSession = loadSymbolFile( m_base, m_imageName, m_symfile);
    }
    catch(const SymbolException &)
    {
    }
    if (m_symSession)
    {
        m_symSessionCache.insert( std::make_pair( cacheKey, m_symSession ) );
        return m_symSession;
    }

    // TODO: read image file path and load using IDiaReadExeAtOffsetCallback

    m_symfile = getModuleSymbolFileName(m_base);
    if (!m_symfile.empty() )
    {
        try
        {
            m_symSession = loadSymbolFile(m_symfile, m_base);
        }
        catch(const SymbolException&)
        {
        }

        if (m_symSession)
        {
            m_symSessionCache.insert( std::make_pair( cacheKey, m_symSession ) );
            return m_symSession;
        }
    }

    try
    {
        m_symSession = loadSymbolFromExports(m_base);
        m_symfile = "export symbols";
    }
    catch(const SymbolException&)
    {
    }

    if (m_symSession)
    {
        m_symSessionCache.insert( std::make_pair( cacheKey, m_symSession ) );
        return m_symSession;
    }

    m_symSessionCache.insert( std::make_pair( cacheKey, SymbolSessionPtr() ) );

    throw SymbolException( "failed to load symbol file" );
}

/////////////////////////////////////////////////////////////////////////////////////

SymbolPtr Module::getSymScope()
{
    return getSymSession()->getSymbolScope();
}

/////////////////////////////////////////////////////////////////////////////////////

void Module::reloadSymbols()
{
    SymbolMapKey cacheKey = { m_name, m_size, m_timeDataStamp, m_checkSum };
    m_symSessionCache.erase( cacheKey );

    m_symSession.reset();
    getSymSession();
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG Module::getRvaByName(const std::string &symName)
{
    SymbolPtr  sym = getSymScope()->getChildByName( symName );
    return sym->getRva();
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

    prepareSymbolFile();

    sstr << "Module: " << m_name <<  std::endl;
    sstr << "Start: " << std::hex << m_base << " End: " << getEnd() << " Size: " << m_size << std::endl;
    sstr << "Image: " << m_imageName << std::endl;
    sstr << "Symbols: " << m_symfile << std::endl;
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

    SymbolPtr symVar = getSymSession()->findByRva( (ULONG)(offset - m_base ) );

    TypeInfoPtr typeInfo = TypeInfo::getTypeInfo( symVar->getType() );

    return TypedVar::getTypedVar(typeInfo, VarDataMemory::factory(offset) );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr
Module::getTypedVarByName( const std::string &symName )
{
    SymbolPtr  symVar = getSymScope()->getChildByName( symName );

    TypeInfoPtr typeInfo = TypeInfo::getTypeInfo( symVar->getType() );

    if ( LocIsConstant != symVar->getLocType() )
    {
        ULONG64  offset = getSymbolOffset( symName );
        return TypedVar::getTypedVar(typeInfo, VarDataMemory::factory(offset) );
    }

    return TypedVar::getTypedVar( typeInfo, VarDataConst::factory(symVar) );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr 
Module::getTypedVarByTypeName( const std::string &typeName, ULONG64 offset )
{
    offset = addr64(offset);

    TypeInfoPtr typeInfo = getTypeByName( typeName );

    return TypedVar::getTypedVar(typeInfo, VarDataMemory::factory(offset) );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr Module::containingRecordByName( ULONG64 offset, const std::string &typeName, const std::string &fieldName )
{
    TypeInfoPtr     typeInfo = getTypeByName( typeName );

    return containingRecordByType( offset, typeInfo, fieldName );
}

///////////////////////////////////////////////////////////////////////////////

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

SymbolPtr Module::getSymbolByVa( ULONG64 offset, ULONG symTag, LONG* displacment )
{
    offset = addr64(offset);

    if ( offset < m_base || offset > getEnd() )
        throw DbgException( "address is out of the module space" );

   return getSymSession()->findByRva( (ULONG)(offset - m_base ), symTag, displacment );
}
///////////////////////////////////////////////////////////////////////////////////

std::string Module::getSymbolNameByVa( ULONG64 offset, bool showDisplacement )
{
    offset = addr64(offset);

    if ( offset < m_base || offset > getEnd() )
        throw DbgException( "address is out of the module space" );

    LONG displacement = 0;

    SymbolPtr  sym =  getSymSession()->findByRva( (ULONG)(offset - m_base ), SymTagNull, &displacement );

    std::stringstream  sstr;

    sstr << sym->getName();

    if ( showDisplacement )
    {
        if ( displacement > 0 && displacement )
            sstr << '+' << std::hex << displacement;
        else if ( displacement < 0 )
            sstr << '-' << std::hex << -displacement;
    }

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////////

void Module::getSourceLine( ULONG64 offset, std::string &fileName, ULONG &lineNo, LONG &displacement )
{
    getSymSession()->getSourceLine( offset, fileName, lineNo, displacement );
}

///////////////////////////////////////////////////////////////////////////////////

std::string Module::getSourceFile( ULONG64 offset )
{
    std::string  fileName;
    ULONG  lineNo;
    LONG  displacement;

    getSymSession()->getSourceLine( offset, fileName, lineNo, displacement );

    return fileName;
}

void Module::prepareSymbolFile()
{
    try
    {
        // load symbols for modules, if need
        getSymSession();
    }
    catch (const SymbolException &e)
    {
        DBG_UNREFERENCED_LOCAL_VARIABLE(e);
    }
}

///////////////////////////////////////////////////////////////////////////////////

python::list Module::enumSymbols( const std::string  &mask)
{
    python::list  lst;

    SymbolPtrList  symlst = getSymScope()->findChildren( SymTagData, mask, true );

    for ( SymbolPtrList::iterator it = symlst.begin(); it != symlst.end(); ++it )
    {
        if ( (*it)->getDataKind() == DataIsConstant )
        {
            lst.append( python::make_tuple( (*it)->getName(), python::object() ) );
        }
        else
        {
            lst.append( python::make_tuple( (*it)->getName(), (*it)->getVa() ) );
        }
    }

    symlst = getSymScope()->findChildren( SymTagFunction, mask, true );

    for ( SymbolPtrList::iterator it = symlst.begin(); it != symlst.end(); ++it )
    {
        lst.append( python::make_tuple( (*it)->getName(), (*it)->getVa() ) );
    }

    return lst;
}

///////////////////////////////////////////////////////////////////////////////////

python::list Module::enumTypes( const std::string  &mask )
{
    python::list  lst;
    int  tags[] = { SymTagUDT, SymTagEnum };

    for ( size_t i = 0; i < sizeof(tags)/sizeof(tags[0]); ++i )
    {
        SymbolPtrList  symlst = getSymScope()->findChildren( tags[i], mask, true );

        for ( SymbolPtrList::iterator it = symlst.begin(); it != symlst.end(); ++it )
        {
            lst.append( (*it)->getName() );
        }
    }

    return lst;
}

///////////////////////////////////////////////////////////////////////////////////

std::string Module::queryVersion( const std::string &value) {
     return getModuleVersionInfo( m_base, value );
}

///////////////////////////////////////////////////////////////////////////////////

python::tuple Module::getVersion()
{
    USHORT a1,a2,a3,a4;
    getModuleFileVersion( m_base, a1,a2,a3,a4 );

    return python::make_tuple(a1,a2,a3,a4);
}

///////////////////////////////////////////////////////////////////////////////////

}; // end of namespace pykd
