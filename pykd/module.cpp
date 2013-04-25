#include "stdafx.h"
#include "dbgengine.h"
#include "module.h"
#include "dbgexcept.h"
#include "vardata.h"
#include "symsessioncache.h"

namespace pykd {

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
    completeConstruct();
}

/////////////////////////////////////////////////////////////////////////////////////

Module::Module(ULONG64 offset )
{
    m_base = findModuleBase( addr64(offset) );
    m_name = getModuleName( m_base );
    completeConstruct();
}

/////////////////////////////////////////////////////////////////////////////////////

void Module::completeConstruct()
{
    m_imageName = getModuleImageName( m_base );
    m_timeDataStamp = getModuleTimeStamp( m_base );
    m_checkSum = getModuleCheckSum( m_base );
    m_size = getModuleSize( m_base );
    m_unloaded = isModuleUnloaded( m_base );
    m_userMode = isModuleUserMode( m_base );
}

/////////////////////////////////////////////////////////////////////////////////////

SymbolSessionPtr& Module::getSymSession()
{
    if (m_symSession)
        return m_symSession;

    SymCacheModuleKey cacheKey = { m_name, m_size, m_timeDataStamp, m_checkSum };
    if ( findSymCacheEntry( cacheKey, m_symSession ) )
    {
        if ( !m_symSession )
            throw SymbolException( "failed to load symbol file" );

        return m_symSession;
    }

    try
    {
        m_symSession = loadSymbolFile( m_base, m_imageName);
        if (m_symSession)
        {
            insertSymCacheEntry( m_base, cacheKey, m_symSession );
            return m_symSession;
        }
    }
    catch(const SymbolException &)
    {}

    // TODO: read image file path and load using IDiaReadExeAtOffsetCallback

    try
    {
        std::string symfile = getModuleSymbolFileName(m_base);
        if (!symfile.empty() )
        {
            m_symSession = loadSymbolFile(symfile, m_base);
        }

        if (m_symSession)
        {
            insertSymCacheEntry( m_base, cacheKey, m_symSession );
            return m_symSession;
        }
    }
    catch(const DbgException&)
    {}

    try
    {
        m_symSession = loadSymbolFromExports(m_base);
        if (m_symSession)
        {
            insertSymCacheEntry( m_base, cacheKey, m_symSession );
            return m_symSession;
        }
    }
    catch(const DbgException&)
    {}

    insertSymCacheEntry( m_base, cacheKey, SymbolSessionPtr() );
    throw SymbolException( "failed to load symbol file" );
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG64 Module::prepareVa(ULONG64 offset)
{
    offset = addr64(offset);

    if ( offset < m_base || offset > getEnd() )
        throw DbgException( "address is out of the module space" );

    return offset;
}

/////////////////////////////////////////////////////////////////////////////////////

SymbolPtr Module::getSymScope()
{
    return getSymSession()->getSymbolScope();
}

/////////////////////////////////////////////////////////////////////////////////////

void Module::reloadSymbols()
{
    SymCacheModuleKey cacheKey = { m_name, m_size, m_timeDataStamp, m_checkSum };
    eraseSymCacheEntry( cacheKey );

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
    sstr << "Start: " << std::hex << m_base << " End: " << getEnd() << " Size: " << m_size;
    sstr << (m_unloaded ? ", UNLOADED!" : "") << std::endl;
    sstr << "Image: " << m_imageName << std::endl;
    if ( m_symSession )
    {
         sstr << "Symbols: " << m_symSession->getSymbolFileName() << std::endl;
         std::string buildDesc = m_symSession->getBuildDescription();
         if (!buildDesc.empty())
            sstr << "\t" << buildDesc << std::endl;
    }
    else
    {
         sstr << "Symbols: not found" << std::endl;
    }


    sstr << "Timestamp: " << m_timeDataStamp << std::endl;
    sstr << "Check Sum: " << m_checkSum << std::endl;

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////////

python::list Module::getUdts()
{
    SymbolPtrList symlst = getSymScope()->findChildren( SymTagUDT );

    python::list lst;
    for ( SymbolPtrList::iterator it = symlst.begin(); it != symlst.end(); ++it )
        lst.append( (*it)->getName() );

    return lst;
}

///////////////////////////////////////////////////////////////////////////////////

python::list Module::getEnums()
{
    SymbolPtrList symlst = getSymScope()->findChildren( SymTagEnum );

    python::list lst;
    for ( SymbolPtrList::iterator it = symlst.begin(); it != symlst.end(); ++it )
        lst.append( (*it)->getName() );

    return lst;
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr 
Module::getTypedVarByAddr( ULONG64 offset )
{
    offset = addr64(offset);

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
    offset = prepareVa(offset);

   return getSymSession()->findByRva( (ULONG)(offset - m_base ), symTag, displacment );
}

///////////////////////////////////////////////////////////////////////////////////

std::string Module::getSymbolNameByVa( ULONG64 offset, bool showDisplacement )
{
    offset = prepareVa(offset);

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

void Module::getSymbolAndDispByVa( ULONG64 offset, std::string &symbolName, LONG &displacement)
{
    offset = prepareVa(offset);

    SymbolPtr sym = getSymSession()->findByRva( (ULONG)(offset - m_base ), SymTagNull, &displacement );

    symbolName = sym->getName();
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

    SymbolPtrList  symlst = getSymScope()->findChildren( SymTagData, mask );

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

    symlst = getSymScope()->findChildren( SymTagFunction, mask );

    for ( SymbolPtrList::iterator it = symlst.begin(); it != symlst.end(); ++it )
    {
        lst.append( python::make_tuple( (*it)->getName(), (*it)->getVa() ) );
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
