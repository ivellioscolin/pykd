#pragma     once

#include "kdlib/typeinfo.h"

namespace pykd {

struct TypeInfoAdapter : public kdlib::TypeInfo {

    static kdlib::TypeInfoPtr getTypeInfoByName( const std::wstring &name )
    {
        return kdlib::loadType( name );
    }

    static kdlib::MEMOFFSET_32 getElementOffset( kdlib::TypeInfo &typeInfo, const std::wstring &name ) {
        return typeInfo.getElementOffset( name );
    }


    static kdlib::MEMOFFSET_64 getStaticOffset( kdlib::TypeInfo &typeInfo, const std::wstring &name ) {
        return typeInfo.getElementVa( name );
    }


    static kdlib::TypeInfoPtr getElement( kdlib::TypeInfo &typeInfo, const std::wstring &name ) {
        return typeInfo.getElement(name);
    }
};

} // end namespace pykd
