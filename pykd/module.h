#pragma once

#include <sstream>

#include "kdlib/module.h"

#include "stladaptor.h"

namespace pykd {


struct ModuleAdapter : public kdlib::Module 
{

    static kdlib::ModulePtr loadModuleByName( const std::wstring &name )
    {
        return kdlib::loadModule( name );
    }

    static kdlib::ModulePtr loadModuleByOffset( kdlib::MEMOFFSET_64 offset )
    {
        return kdlib::loadModule( offset);
    }

    static std::wstring print( kdlib::Module& module ) 
    {        
        std::wstringstream   sstr;

        //prepareSymbolFile();

        sstr << L"Module: " << module.getName() <<  std::endl;
        sstr << L"Start: " << std::hex <<  module.getBase() << L" End: " <<  module.getEnd() << L" Size: " <<  module.getSize() << std::endl;
        //sstr << (m_unloaded ? ", UNLOADED!" : "") << std::endl;
        sstr << L"Image: " <<  module.getImageName() << std::endl;
        sstr << L"Symbols: " << module.getSymFile() << std::endl;


        //if ( m_symSession )
        //{
        //     sstr << "Symbols: " << m_symSession->getSymbolFileName() << std::endl;
        //    std::string buildDesc = m_symSession->getBuildDescription();
        //    if (!buildDesc.empty())
        //        sstr << "\t" << buildDesc << std::endl;
        //}
        //else
        //{
        //     sstr << "Symbols: not found" << std::endl;
        //}
 
        sstr << L"Timestamp: " << module.getTimeDataStamp() << std::endl;
        sstr << L"Check Sum: " << module.getCheckSum() << std::endl;

        return sstr.str();
    }

    static python::list enumSymbols( kdlib::Module& module, const std::wstring  &mask = L"*" )
    {
        kdlib::SymbolOffsetList  offsetLst = module.enumSymbols( mask );
        python::list  pyLst;
        for (  kdlib::SymbolOffsetList::const_iterator it = offsetLst.begin(); it != offsetLst.end(); ++it )
            pyLst.append( python::make_tuple( it->first, it->second ) );
        return pyLst;
    }

    static std::wstring findSymbol( kdlib::Module& module, kdlib::MEMOFFSET_64 offset, bool showDisplacement = true ) 
    {
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

    static python::tuple findSymbolAndDisp( kdlib::Module& module, kdlib::MEMOFFSET_64 offset )
    {
        kdlib::MEMDISPLACEMENT  displacement = 0;
        std::wstring  symbolName = module.findSymbol( offset, displacement );
        return python::make_tuple( symbolName, displacement );
    }


    static python::list getTypedVarListByTypeName( kdlib::Module& module, kdlib::MEMOFFSET_64 offset, const std::wstring &typeName, const std::wstring &fieldName )
    {
        kdlib::TypedVarList  lst = module.loadTypedVarList( offset, typeName, fieldName );
        return vectorToList( lst );
    }

    static python::list getTypedVarArrayByTypeName( kdlib::Module& module, kdlib::MEMOFFSET_64 offset, const std::wstring &typeName, size_t number )
    {
        kdlib::TypedVarList  lst =  module.loadTypedVarArray( offset, typeName, number );
        return vectorToList( lst );
    }

};

} // end namespace pykd
