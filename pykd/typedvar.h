#pragma once

#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
namespace python = boost::python;

#include "kdlib/typedvar.h"

namespace pykd {

struct TypedVarAdapter {

    static kdlib::TypedVarPtr getTypedVarByName( const std::wstring &name ) {
        return kdlib::loadTypedVar( name );
    }

    static kdlib::TypedVarPtr getTypedVarByTypeName( const std::wstring &name, kdlib::MEMOFFSET_64 addr ) {
        return kdlib::loadTypedVar( name, addr );
    }

    static kdlib::TypedVarPtr getTypedVarByTypeInfo( const kdlib::TypeInfoPtr &typeInfo, kdlib::MEMOFFSET_64 addr )
    {
        return kdlib::loadTypedVar( typeInfo, addr );
    }

    static kdlib::MEMOFFSET_32 getFieldOffsetByName( kdlib::TypedVar& typedVar, const std::wstring &name ) {
        return typedVar.getElementOffset( name );
    }

    static kdlib::TypedVarPtr getField( kdlib::TypedVar& typedVar, const std::wstring &name ) {
        return typedVar.getElement( name );
    }


    static kdlib::TypedVarPtr getElementByIndex( kdlib::TypedVar& typedVar, long index ) {
        return typedVar.getElement( index );
    }

    static std::wstring print( kdlib::TypedVar& typedVar ) {
        return L"TYPEDVAR STR";
    }

    static kdlib::TypedVarPtr containingRecordByName( kdlib::MEMOFFSET_64 offset, const std::wstring &typeName, const std::wstring &fieldName ) {
        return kdlib::containingRecord( offset, typeName, fieldName );
    }

    static kdlib::TypedVarPtr containingRecordByType( kdlib::MEMOFFSET_64 offset, kdlib::TypeInfoPtr &typeInfo, const std::wstring &fieldName ) {
        return kdlib::containingRecord( offset, typeInfo, fieldName );
    }

    static python::list getFields( kdlib::TypedVar& typedVar )
    {
        python::list  lst;
        for ( size_t i = 0; i < typedVar.getElementCount(); ++i )
        {
            std::wstring  name = typedVar.getElementName(i);
            kdlib::MEMOFFSET_32  offset = typedVar.getElementOffset(i);
            kdlib::TypedVarPtr  val = typedVar.getElement(i);
            lst.append( python::make_tuple( name, offset, val ) );
        }

        return lst;
    }
};

} // end namespace pykd
