#pragma once

#include <comutil.h>

#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
namespace python = boost::python;

#include "kdlib/typedvar.h"

#include "stladaptor.h"
#include "pythreadstate.h"
#include "dbgexcept.h"

namespace pykd {

inline kdlib::TypedVarPtr getTypedVarByTypeName( const std::wstring &name, kdlib::MEMOFFSET_64 addr ) 
{
    AutoRestorePyState  pystate;
    return kdlib::loadTypedVar( name, addr );
}

inline kdlib::TypedVarPtr getTypedVarByName( const std::wstring &name ) 
{
    AutoRestorePyState  pystate;
    return kdlib::loadTypedVar( name );
}

inline  kdlib::TypedVarPtr getTypedVarByTypeInfo( const kdlib::TypeInfoPtr &typeInfo, kdlib::MEMOFFSET_64 addr )
{
    AutoRestorePyState  pystate;
    return kdlib::loadTypedVar( typeInfo, addr );
}

python::list getTypedVarListByTypeName( kdlib::MEMOFFSET_64 offset, const std::wstring &typeName, const std::wstring &fieldName );
python::list getTypedVarListByType( kdlib::MEMOFFSET_64 offset, kdlib::TypeInfoPtr &typeInfo, const std::wstring &fieldName );
python::list getTypedVarArrayByTypeName( kdlib::MEMOFFSET_64 offset, const std::wstring &typeName, size_t number );
python::list getTypedVarArrayByType( kdlib::MEMOFFSET_64 offset, kdlib::TypeInfoPtr &typeInfo, size_t number );

inline kdlib::TypedVarPtr containingRecordByName( kdlib::MEMOFFSET_64 offset, const std::wstring &typeName, const std::wstring &fieldName )
{
    AutoRestorePyState  pystate;
    return kdlib::containingRecord( offset, typeName, fieldName );
}

inline kdlib::TypedVarPtr containingRecordByType( kdlib::MEMOFFSET_64 offset, kdlib::TypeInfoPtr &typeInfo, const std::wstring &fieldName ) 
{
    AutoRestorePyState  pystate;
    return kdlib::containingRecord( offset, typeInfo, fieldName );
}


struct TypedVarAdapter {


    static python::tuple getLocation(kdlib::TypedVar& typedVar)
    {

        kdlib::VarStorage   storage;
        std::wstring regName;
        kdlib::MEMOFFSET_64  memOffset;
        {
            AutoRestorePyState  pystate;
            storage = typedVar.getStorage();
            if (storage == kdlib::RegisterVar)
                regName = typedVar.getRegisterName();
            else
                memOffset = typedVar.getAddress();
        }

        if (storage == kdlib::RegisterVar)
            return python::make_tuple(storage, regName);
        return python::make_tuple(storage, memOffset);
    }

    static kdlib::MEMOFFSET_64 getAddress( kdlib::TypedVar& typedVar )
    {
        AutoRestorePyState  pystate;
        return typedVar.getAddress();
    }

    static kdlib::MEMOFFSET_64 getDebugStart( kdlib::TypedVar& typedVar )
    {
        AutoRestorePyState  pystate;
        return typedVar.getDebugStart();
    }

    static kdlib::MEMOFFSET_64 getDebugEnd( kdlib::TypedVar& typedVar )
    {
        AutoRestorePyState  pystate;
        return typedVar.getDebugEnd();
    }

    static size_t getSize( kdlib::TypedVar& typedVar ) 
    {
        AutoRestorePyState  pystate;
        return typedVar.getSize();
    }

    static kdlib::MEMOFFSET_32 getFieldOffsetByName( kdlib::TypedVar& typedVar, const std::wstring &name )
    {
        AutoRestorePyState  pystate;
        return typedVar.getElementOffset( name );
    }

    static kdlib::TypedVarPtr getField( kdlib::TypedVar& typedVar, const std::wstring &name ) 
    {
        AutoRestorePyState  pystate;
        return typedVar.getElement( name );
    }

    static kdlib::TypedVarPtr getFieldAttr(kdlib::TypedVar& typedVar, const std::wstring &name)
    {
        try
        {
            return getField(typedVar, name);
        }
        catch (kdlib::DbgException&)
        {
            std::wstringstream sstr;
            sstr << L"typed var has no field " << L'\'' << name << L'\'';
            throw AttributeException(std::string(_bstr_t(sstr.str().c_str())).c_str());
        }
    }

    static size_t getElementCount( kdlib::TypedVar& typedVar ) 
    {
        AutoRestorePyState  pystate;
        return typedVar.getElementCount();
    }

    static std::wstring getElementName( kdlib::TypedVar& typedVar, long index )
    {
        AutoRestorePyState  pystate;
        return typedVar.getElementName( index );
    }    

    static kdlib::TypedVarPtr getElementByIndex( kdlib::TypedVar& typedVar, long index ) 
    {
        AutoRestorePyState  pystate;
        return typedVar.getElement( index );
    }

    static std::wstring print( kdlib::TypedVar& typedVar ) 
    {
        AutoRestorePyState  pystate;
        return typedVar.str();
    }

    static python::list getFields( kdlib::TypedVar& typedVar );

    static kdlib::TypeInfoPtr getType( kdlib::TypedVar& typedVar )
    {
        AutoRestorePyState  pystate;
        return typedVar.getType();
    }

    static kdlib::TypedVarPtr deref( kdlib::TypedVar& typedVar ) 
    {
        AutoRestorePyState  pystate;
        return typedVar.deref();
    }

    static python::list getElementsDir(kdlib::TypedVar& typedVar);

    static bool isZero(kdlib::TypedVar& typedVar)
    {
        AutoRestorePyState  pystate;
        return typedVar.getValue() == 0;
    }

    static bool isNotZero(kdlib::TypedVar& typedVar)
    {
        return !isZero(typedVar);
    }

    static kdlib::TypedVarPtr castByName(kdlib::TypedVar& typedVar, const std::wstring &typeName)
    {
        AutoRestorePyState  pystate;
        return typedVar.castTo(typeName);
    }

    static kdlib::TypedVarPtr castByTypeInfo(kdlib::TypedVar& typedVar, const kdlib::TypeInfoPtr& typeInfo)
    {
        AutoRestorePyState  pystate;
        return typedVar.castTo(typeInfo);
    }
};

} // end namespace pykd
