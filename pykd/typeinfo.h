#pragma     once

#include "kdlib/typeinfo.h"

namespace pykd {

struct TypeInfoAdapter : public kdlib::TypeInfo {

    static kdlib::TypeInfoPtr getTypeInfoByName( const std::wstring &name )
    {
        return kdlib::loadType( name );
    }

    static std::wstring findSymbol(  kdlib::MEMOFFSET_64 offset, bool showDisplacement = true ) 
    {
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
                sstr << mod->getName() << '+' << std::hex << ( offset - mod->getBase() );
                return sstr.str();
            }

        } catch( kdlib::DbgException& )
        {
            std::wstringstream sstr;
            sstr << std::hex << offset;
            return sstr.str();
        }

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

    static kdlib::TypeInfoPtr ptrTo( kdlib::TypeInfo &typeInfo, size_t ptrSize = 0 ) {
        return typeInfo.ptrTo(ptrSize);
    }
    
};

struct BaseTypesEnum {
    static kdlib::TypeInfoPtr getUInt1B() { return kdlib::loadType(L"UInt1B"); }
    static kdlib::TypeInfoPtr getUInt2B() { return kdlib::loadType(L"UInt2B"); }
    static kdlib::TypeInfoPtr getUInt4B() { return kdlib::loadType(L"UInt4B"); }
    static kdlib::TypeInfoPtr getUInt8B() { return kdlib::loadType(L"UInt8B"); }
    static kdlib::TypeInfoPtr getInt1B() { return kdlib::loadType(L"Int1B"); }
    static kdlib::TypeInfoPtr getInt2B() { return kdlib::loadType(L"Int2B"); }
    static kdlib::TypeInfoPtr getInt4B() { return kdlib::loadType(L"Int4B"); }
    static kdlib::TypeInfoPtr getInt8B() { return kdlib::loadType(L"Int8B"); }
    static kdlib::TypeInfoPtr getLong() { return kdlib::loadType(L"Long"); }
    static kdlib::TypeInfoPtr getULong() { return kdlib::loadType(L"ULong"); }
    static kdlib::TypeInfoPtr getBool() { return kdlib::loadType(L"Bool"); }
    static kdlib::TypeInfoPtr getChar() { return kdlib::loadType(L"Char"); }
    static kdlib::TypeInfoPtr getWChar() { return kdlib::loadType(L"WChar"); }
    static kdlib::TypeInfoPtr getVoidPtr() { return kdlib::loadType(L"Void*"); }
};

} // end namespace pykd
