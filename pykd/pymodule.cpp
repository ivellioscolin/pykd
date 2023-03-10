#include "stdafx.h"

#include "pymodule.h"
#include <iomanip>
#include <ctime>

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

python::list getModuleList()
{
    std::vector<kdlib::ModulePtr >  moduleLst;

    do {
        AutoRestorePyState  pystate;
        for ( unsigned long i = 0; i < kdlib::getNumberModules(); ++i)
            moduleLst.push_back( kdlib::loadModule(kdlib::getModuleOffsetByIndex(i) ) );

    } while(false);

    return vectorToList(moduleLst);
}

///////////////////////////////////////////////////////////////////////////////

FixedFileInfoPtr ModuleAdapter::getFixedFileInfo( kdlib::Module& module )
{
    AutoRestorePyState  pystate;

    kdlib::FixedFileInfo fixedFileInfo;
    module.getFixedFileInfo(fixedFileInfo);

    return FixedFileInfoPtr( new kdlib::FixedFileInfo(fixedFileInfo) );
}

///////////////////////////////////////////////////////////////////////////////

std::wstring ModuleAdapter::print( kdlib::Module& module ) 
{
    AutoRestorePyState  pystate;

    std::wstringstream   sstr;

    //prepareSymbolFile();

    sstr << L"Module: " << module.getName() <<  std::endl;
    sstr << L"Start: " << std::hex <<  module.getBase() << L" End: " <<  module.getEnd() << L" Size: " <<  module.getSize() << std::endl;
    //sstr << (m_unloaded ? ", UNLOADED!" : "") << std::endl;
    sstr << L"Image: " <<  module.getImageName() << std::endl;
    sstr << L"Symbols: " << module.getSymFile() << std::endl;

    time_t  timeStamp = module.getTimeDataStamp();
    sstr << L"Timestamp: " << timeStamp << " (" << std::put_time(std::gmtime(&timeStamp), L"%c") << L')' << std::endl;
    sstr << L"Check Sum: " << module.getCheckSum() << std::endl;

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

python::list ModuleAdapter::enumSymbols( kdlib::Module& module, const std::wstring  &mask )
{
    kdlib::SymbolOffsetList  offsetLst;
    
    do {
        AutoRestorePyState  pystate;
        offsetLst = module.enumSymbols( mask );
    } while(false);

    python::list  pyLst;
    for (  kdlib::SymbolOffsetList::const_iterator it = offsetLst.begin(); it != offsetLst.end(); ++it )
        pyLst.append( python::make_tuple( it->first, it->second ) );
    return pyLst;
}

///////////////////////////////////////////////////////////////////////////////

python::list  ModuleAdapter::enumTypes(kdlib::Module& module, const std::wstring &mask)
{
	kdlib::TypeNameList  typeLst;

	do {
		AutoRestorePyState  pystate;
		typeLst = module.enumTypes(mask);
	} while (false);

	python::list  pyLst;
	for (kdlib::TypeNameList::const_iterator it = typeLst.begin(); it != typeLst.end(); ++it)
		pyLst.append(*it);
	return pyLst;
}

///////////////////////////////////////////////////////////////////////////////

bool ModuleAdapter::isContainedSymbol(kdlib::ModulePtr& module, const std::wstring& symbolName)
{
    if (!module->enumSymbols(symbolName).empty())
        return true;

    return !module->enumTypes(symbolName).empty();
}

///////////////////////////////////////////////////////////////////////////////

std::wstring ModuleAdapter::findSymbol( kdlib::Module& module, kdlib::MEMOFFSET_64 offset, bool showDisplacement ) 
{
    AutoRestorePyState  pystate;

    kdlib::MEMDISPLACEMENT  displacement = 0;

    std::wstring  symbolName = module.findSymbol( offset, displacement );
    if ( !showDisplacement || displacement == 0 )
        return symbolName;

    std::wstringstream  wsstr;

    wsstr << symbolName;

    if ( displacement > 0  )
        wsstr << L'+' << std::hex << displacement;
    else
        wsstr << L'-' << std::hex << -displacement;

    return wsstr.str();
}

///////////////////////////////////////////////////////////////////////////////

python::tuple ModuleAdapter::findSymbolAndDisp( kdlib::Module& module, kdlib::MEMOFFSET_64 offset )
{
    kdlib::MEMDISPLACEMENT  displacement = 0;
    std::wstring  symbolName;

    do {
        AutoRestorePyState  pystate;
        symbolName = module.findSymbol( offset, displacement );
    } while(false);

    return python::make_tuple( symbolName, displacement );
}

///////////////////////////////////////////////////////////////////////////////

python::list ModuleAdapter::getTypedVarListByTypeName( kdlib::Module& module, kdlib::MEMOFFSET_64 offset, const std::wstring &typeName, const std::wstring &fieldName )
{
    kdlib::TypedVarList  lst;
    
    do {
        AutoRestorePyState  pystate;
        lst = module.loadTypedVarList( offset, typeName, fieldName );
    } while(false);

    return vectorToList( lst );
}

///////////////////////////////////////////////////////////////////////////////

python::list ModuleAdapter::getTypedVarArrayByTypeName( kdlib::Module& module, kdlib::MEMOFFSET_64 offset, const std::wstring &typeName, size_t number )
{
    kdlib::TypedVarList  lst;
    
    do {
        AutoRestorePyState  pystate;
        lst =  module.loadTypedVarArray( offset, typeName, number );
    } while(false);

    return vectorToList( lst );
}

///////////////////////////////////////////////////////////////////////////////


} // namespace pykd
