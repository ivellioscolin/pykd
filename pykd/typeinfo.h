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

};

} // end namespace pykd
