#include "stdafx.h"

#include "kdlib/module.h"
#include "kdlib/exceptions.h"

#include "pytypeinfo.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

std::wstring findSymbol(  kdlib::MEMOFFSET_64 offset, bool showDisplacement ) 
{
    AutoRestorePyState  pystate;

    kdlib::MEMDISPLACEMENT  displacement = 0;
    std::wstring  symbolName;

    try {

        kdlib::ModulePtr  mod = kdlib::loadModule( offset );

        try {

            symbolName = mod->findSymbol( offset, displacement );

            std::wstringstream  sstr;
                
            sstr << mod->getName() << L'!' << symbolName;

            if ( !showDisplacement || displacement == 0 )
                return sstr.str();

            if ( displacement > 0  )
                sstr << L'+' << std::hex << displacement;
            else
                sstr << L'-' << std::hex << -displacement;

            return sstr.str();

        } catch( kdlib::DbgException& )
        {
            std::wstringstream  sstr;
            sstr << mod->getName();
            if (showDisplacement)
                sstr << '+' << std::hex << ( offset - mod->getBase() );
            return sstr.str();
        }

    } catch( kdlib::DbgException& )
    {
        std::wstringstream sstr;
        sstr << std::hex << offset;
        return sstr.str();
    }
}

///////////////////////////////////////////////////////////////////////////////

python::tuple findSymbolAndDisp( ULONG64 offset )
{
    kdlib::MEMDISPLACEMENT  displacement = 0;
    std::wstring  symbolName;
    std::wstring  moduleName;

    do {
        AutoRestorePyState  pystate;
        symbolName = kdlib::findSymbol( offset, displacement );
        moduleName = kdlib::getModuleName( kdlib::findModuleBase( offset ) );
    } while(false);

    return python::make_tuple(moduleName,symbolName,displacement);
}

///////////////////////////////////////////////////////////////////////////////

python::list TypeInfoAdapter::getFields( kdlib::TypeInfo &typeInfo )
{
    typedef boost::tuple<std::wstring,kdlib::TypeInfoPtr> FieldTuple;

    std::list<FieldTuple>  lst;

    do {

        AutoRestorePyState  pystate;

        for ( size_t i = 0; i < typeInfo.getElementCount(); ++i )
        {
            std::wstring  name = typeInfo.getElementName(i);
            kdlib::TypeInfoPtr  val = typeInfo.getElement(i);

            lst.push_back( FieldTuple( name, val ) );
        }

    } while(false);

    python::list pylst;
    
    for ( std::list<FieldTuple>::const_iterator it = lst.begin(); it != lst.end(); ++it)
        pylst.append( python::make_tuple( it->get<0>(), it->get<1>() ) );

    return pylst;
}

///////////////////////////////////////////////////////////////////////////////

python::list TypeInfoAdapter::getElementDir(kdlib::TypeInfo &typeInfo)
{
    std::list<std::wstring>  lst;

    python::list pylst;

    try{

        AutoRestorePyState  pystate;

        for (size_t i = 0; i < typeInfo.getElementCount(); ++i)
        {
            std::wstring  name = typeInfo.getElementName(i);
            lst.push_back(name);
        }

    } catch(kdlib::DbgException&)
    {
        return pylst;
    }

    for (std::list<std::wstring>::const_iterator it = lst.begin(); it != lst.end(); ++it)
        pylst.append(*it);

    return pylst;
}

///////////////////////////////////////////////////////////////////////////////

} // pykd namespace
