#include "stdafx.h"

#include "kdlib/module.h"
#include "kdlib/exceptions.h"

#include "pytypeinfo.h"
#include "variant.h"

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

kdlib::TypeInfoPtr TypeInfoAdapter::getElementAttr(kdlib::TypeInfo &typeInfo, const std::wstring &name)
{
    {
        AutoRestorePyState  pystate;

        try {
            return typeInfo.getElement(name);
        }
        catch (kdlib::TypeException&)
        {}

        try {
            return typeInfo.getMethod(name);
        }
        catch (kdlib::TypeException&)
        {}
    }

    std::wstringstream sstr;
    sstr << L'\'' << typeInfo.getName() << L'\'' << L" type has no field " << L'\'' << name << L'\'';
    throw AttributeException(std::string(_bstr_t(sstr.str().c_str())).c_str());
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

python::object  callTypedVar(kdlib::TypedVarPtr& funcobj, python::tuple& args)
{
    kdlib::TypedValue   retVal;

    size_t  argCount  = python::len(args);

    kdlib::TypedValueList  argLst;

    for ( size_t  i = 0; i < argCount; ++i )
    {
        python::extract<kdlib::TypedVarPtr>   getTypedVar(args[i]);
        if ( getTypedVar.check() )
        {
            argLst.push_back( getTypedVar() );
            continue;
        }

        python::extract<kdlib::NumBehavior>  getNumVar(args[i]);
        if ( getNumVar.check() )
        {
            kdlib::NumVariant   var = getNumVar();
            argLst.push_back( var );
            continue;
        }

        kdlib::NumVariant  var= NumVariantAdaptor::convertToVariant(args[i]);
        argLst.push_back( var );
    }

    {
        AutoRestorePyState  pystate;
        retVal = funcobj->call(argLst);
    }

    if ( retVal.getType()->isVoid() )
    {
        return python::object();
    }

   return NumVariantAdaptor::convertToPython(retVal);
}

///////////////////////////////////////////////////////////////////////////////

python::object callFunctionByVar( python::tuple& args, python::dict& kwargs )
{
    kdlib::TypedVarPtr   funcobj = python::extract<kdlib::TypedVarPtr>(args[0]);

    python::tuple  newArgs = python::tuple(args.slice(1, python::_));

    return callTypedVar(funcobj, newArgs );
}

///////////////////////////////////////////////////////////////////////////////

python::object callFunctionByOffset( python::tuple& args, python::dict& kwargs)
{
    kdlib::TypeInfoPtr   functype = python::extract<kdlib::TypeInfoPtr>(args[0]);
    kdlib::MEMOFFSET_64  funcaddr = python::extract<kdlib::MEMOFFSET_64>(args[1]);

    kdlib::TypedVarPtr  funcobj = kdlib::loadTypedVar(functype, funcaddr);
    python::tuple  newArgs = python::tuple(args.slice(2, python::_));

     return callTypedVar( funcobj, newArgs );
}

///////////////////////////////////////////////////////////////////////////////

} // pykd namespace
