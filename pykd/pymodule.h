#pragma once

#include <sstream>

#include <comutil.h>


#include "kdlib/module.h"
#include "kdlib/exceptions.h"

#include "stladaptor.h"
#include "pythreadstate.h"
#include "dbgexcept.h"
#include "pytypedvar.h"

namespace pykd {

typedef boost::shared_ptr< kdlib::FixedFileInfo > FixedFileInfoPtr;

python::list getModuleList();

struct ModuleAdapter : public kdlib::Module 
{

    static kdlib::ModulePtr loadModuleByName( const std::wstring &name )
    {
        AutoRestorePyState  pystate;
        return kdlib::loadModule( name );
    }

    static kdlib::ModulePtr loadModuleByOffset( kdlib::MEMOFFSET_64 offset )
    {
        AutoRestorePyState  pystate;
        return kdlib::loadModule( offset);
    }

    static std::wstring  getName( kdlib::Module& module )
    {
        AutoRestorePyState  pystate;
        return module.getName();
    }

    static kdlib::MEMOFFSET_64  getBase( kdlib::Module& module )
    {
        AutoRestorePyState  pystate;
        return module.getBase();
    }

    static kdlib::MEMOFFSET_64  getEnd( kdlib::Module& module )
    {
        AutoRestorePyState  pystate;
        return module.getEnd();
    }

    static size_t getSize( kdlib::Module& module )
    {
        AutoRestorePyState  pystate;
        return module.getSize();
    }

    static void reloadSymbols(kdlib::Module& module)
    {
        AutoRestorePyState  pystate;
        module.reloadSymbols();
    }

    static std::wstring getImageName( kdlib::Module& module )
    {
        AutoRestorePyState  pystate;
        return module.getImageName();
    }

    static std::wstring getSymFile( kdlib::Module& module )
    {
        AutoRestorePyState  pystate;
        return module.getSymFile();
    }

    static kdlib::MEMOFFSET_64 getSymbolVa( kdlib::Module& module, const std::wstring &symbolName )
    {
        AutoRestorePyState  pystate;
        return module.getSymbolVa(symbolName);
    }

    static python::object getAttrByName(kdlib::Module& module, const std::wstring &symbolName)
    {
        AutoRestorePyState  pystate;
        try {
            return python::object(module.getSymbolVa(symbolName));
        }
        catch (kdlib::DbgException&)
        { }

        return python::object(module.getTypeByName(symbolName));
    }

    static python::object getItemByKey(kdlib::Module& module, const std::wstring &symbolName)
    {
        AutoRestorePyState  pystate;

        try {
            return python::object(module.getSymbolVa(symbolName));
        }
        catch (kdlib::DbgException&)
        {
        }

        try {
            return python::object(module.getTypeByName(symbolName));
        }
        catch (kdlib::DbgException&)
        {
        }

        std::wstringstream sstr;
        sstr << L"module hase symbol " << L'\'' << symbolName << L'\'';
        throw KeyException(std::string(_bstr_t(sstr.str().c_str())).c_str());
    }
    

    static kdlib::MEMOFFSET_32 getSymbolRva( kdlib::Module& module, const std::wstring &symbolName )
    {
        AutoRestorePyState  pystate;
        return module.getSymbolRva(symbolName);
    }

    static size_t getSymbolSize( kdlib::Module& module, const std::wstring &symbolName )
    {
        AutoRestorePyState  pystate;
        return module.getSymbolSize(symbolName);
    }

    static kdlib::TypeInfoPtr getTypeByName( kdlib::Module& module, const std::wstring &typeName ) 
    {
        AutoRestorePyState  pystate;
        return module.getTypeByName(typeName);
    }

    static kdlib::TypedVarPtr getTypedVarByAddr( kdlib::Module& module, kdlib::MEMOFFSET_64 offset )
    {
        AutoRestorePyState  pystate;
        return module.getTypedVarByAddr(offset);
    }

    static kdlib::TypedVarPtr getTypedVarByName( kdlib::Module& module, const std::wstring &symbolName )
    {
        AutoRestorePyState  pystate;
        return module.getTypedVarByName(symbolName);
    }

    static kdlib::TypedVarPtr getTypedVarByTypeName( kdlib::Module& module, const std::wstring &typeName, python::object& dataStorage )
    {
        return  getTypedVarByTypeInfo( getTypeByName(module, typeName), dataStorage);
    }

    static kdlib::TypedVarPtr getTypedVarWithPrototype(kdlib::Module& module, const std::wstring &symbolName, const std::wstring &prototype)
    {
        AutoRestorePyState  pystate;
        return module.getTypedVarWithPrototype(symbolName, prototype);
    }

    static kdlib::TypedVarPtr containingRecord(kdlib::Module& module,  kdlib::MEMOFFSET_64 offset, const std::wstring &typeName,  const std::wstring &fieldName )
    {
        AutoRestorePyState  pystate;
        return module.containingRecord(offset, typeName, fieldName);
    }


    static unsigned long getCheckSum( kdlib::Module& module ) 
    {
        AutoRestorePyState  pystate;
        return module.getCheckSum();
    }

    static unsigned long getTimeDataStamp( kdlib::Module& module )
    {
        AutoRestorePyState  pystate;
        return module.getTimeDataStamp();
    }

    static bool isUnloaded( kdlib::Module& module )
    {
        AutoRestorePyState  pystate;
        return module.isUnloaded();
    }

    static bool isUserMode( kdlib::Module& module )
    {
        AutoRestorePyState  pystate;
        return module.isUserMode();
    }

    static std::string getVersionInfo( kdlib::Module& module, const std::string &value )
    {
        AutoRestorePyState  pystate;
        return module.getVersionInfo(value);
    }

    static python::tuple getVersion(kdlib::Module& module)
    {
        AutoRestorePyState  pystate;
        unsigned long a1, a2, a3, a4;
        module.getFileVersion(a1, a2, a3, a4);
        return python::make_tuple(a1, a2, a3, a4);
    }   

    static FixedFileInfoPtr getFixedFileInfo( kdlib::Module& module );

    static std::wstring print( kdlib::Module& module );

    static python::list enumSymbols( kdlib::Module& module, const std::wstring  &mask = L"*" );

	static python::list enumTypes(kdlib::Module& module, const std::wstring &mask = L"*");

    static std::wstring findSymbol( kdlib::Module& module, kdlib::MEMOFFSET_64 offset, bool showDisplacement = true );
    
    static python::tuple findSymbolAndDisp( kdlib::Module& module, kdlib::MEMOFFSET_64 offset );

    static python::list getTypedVarListByTypeName( kdlib::Module& module, kdlib::MEMOFFSET_64 offset, const std::wstring &typeName, const std::wstring &fieldName );

    static python::list getTypedVarArrayByTypeName( kdlib::Module& module, kdlib::MEMOFFSET_64 offset, const std::wstring &typeName, size_t number );

    static bool isContainedSymbol(kdlib::ModulePtr& module, const std::wstring& symbolName);
};

} // end namespace pykd
