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

python::object callFunctionByVar( python::tuple& args, python::dict& kwargs )
{
    kdlib::NumVariant   retVal;

    kdlib::TypedVarPtr   funcvar = python::extract<kdlib::TypedVarPtr>(args[0]);

     size_t  argCount  = python::len(args) - 1;

    if ( argCount != funcvar->getType()->getElementCount() )
        throw kdlib::TypeException(L"wrong argument count");

    kdlib::CallArgList  argLst;

    for ( size_t  i = 0; i < argCount; ++i )
    {
        kdlib::TypeInfoPtr  argType = funcvar->getType()->getElement(i);

        python::object  arg = args[i+1];

        if ( argType->isBase() )
        {
             kdlib::NumVariant  var= NumVariantAdaptor::convertToVariant(arg);

            if ( argType->getName() == L"Char" )
            {
                argLst.push_back( var.asChar() );
            }
            else if ( argType->getName() == L"WChar" )
            {
                argLst.push_back( var.asShort() );
            }
            else if ( argType->getName() == L"Int1B" )
            {
                argLst.push_back( var.asChar() );
            }
            else if ( argType->getName() == L"UInt1B" )
            {
                argLst.push_back( var.asUChar() );
            }
            else if ( argType->getName() == L"Int2B" )
            {
                argLst.push_back( var.asShort() );
            }
            else if ( argType->getName() == L"UInt2B" )
            {
                argLst.push_back( var.asUShort() );
            }
            else if ( argType->getName() == L"Int4B" )
            {
                argLst.push_back( var.asLong() );
            }
            else if ( argType->getName() == L"UInt4B" )
            {
                argLst.push_back( var.asULong() );
            }
            else if ( argType->getName() == L"Int8B" )
            {
                argLst.push_back( var.asLongLong() );
            }
            else if ( argType->getName() == L"UInt8B" )
            {
                argLst.push_back(var.asULongLong());
            }
            else if ( argType->getName() == L"Long" )
            {
                argLst.push_back( var.asLong() );
            }
            else if ( argType->getName() == L"ULong" )
            {
                argLst.push_back( var.asULong() );
            }
            else if ( argType->getName() == L"Bool" )
            {
                argLst.push_back( var.asChar() );
            }
            else if ( argType->getName() == L"Float" )
            {
                argLst.push_back( var.asFloat() );
            }
            else if ( argType->getName() == L"Double")
            {
                argLst.push_back( var.asDouble() );
            }
            else
            {
                throw kdlib::TypeException( std::wstring(L"unsupported argument type ") + argType->getName() );
            }
        }
        else if ( argType->isPointer() )
        {
            kdlib::MEMOFFSET_64  addr;

            python::extract<kdlib::NumBehavior>  getNumVar(arg);
            python::extract<unsigned long long>  getLongLong(arg);
            python::extract<long>  getLong(arg);

            if ( getNumVar.check() )
            {
                kdlib::NumVariant   var = getNumVar();
                addr = var.asULongLong();
            }
            if ( getLongLong.check() )
            {
                addr = getLongLong();
            }
            else if ( getLong.check() )
            {
                addr = getLong();
            }
            else
            {
                std::wstringstream  sstr;
                sstr << "failed to convert " << i << " argument to pointer";
                throw kdlib::TypeException(sstr.str() );
            }

            switch ( argType->getPtrSize() )
            {
            case 4:
                argLst.push_back( static_cast<unsigned long>(addr) );
                break;

            case 8:
                argLst.push_back( static_cast<unsigned long long>(addr) );
                break;

            default:
                throw kdlib::TypeException(L"unsupported call argument");
            }         
        }
        else
        {
            throw kdlib::TypeException(L"unsupported argument type");
        }
    }

    {
        AutoRestorePyState  pystate;
        retVal = funcvar->call(argLst);
    }

    return NumVariantAdaptor::convertToPython(retVal);
}

///////////////////////////////////////////////////////////////////////////////

} // pykd namespace
