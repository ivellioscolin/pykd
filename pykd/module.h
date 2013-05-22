#pragma once

#include "kdlib/module.h"

namespace pykd {


struct ModuleAdapter : public kdlib::Module 
{

    static kdlib::ModulePtr loadModuleByName( const std::wstring &name )
    {
        return kdlib::loadModule( name );
    }

    static kdlib::ModulePtr loadModuleByOffset( kdlib::MEMOFFSET_64 offset )
    {
        return kdlib::loadModule( offset);
    }

    static std::wstring print( kdlib::Module& module ) {
        return L"PYKD MODULE";
    }

};

} // end namespace pykd
