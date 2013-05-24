#pragma once

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

};

} // end namespace pykd
