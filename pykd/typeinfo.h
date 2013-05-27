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


    static kdlib::TypeInfoPtr getElementByName( kdlib::TypeInfo &typeInfo, const std::wstring &name ) {
        return typeInfo.getElement(name);
    }


    static kdlib::TypeInfoPtr getElementByIndex( kdlib::TypeInfo &typeInfo, size_t index ) {
        return typeInfo.getElement(index);
    }

    static std::wstring print( kdlib::TypeInfo &typeInfo ) {
        return L"TYPEINFO STR";
    }

};

} // end namespace pykd
