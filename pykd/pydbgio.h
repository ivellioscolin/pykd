#pragma once

#include "kdlib/windbg.h"

#include "pystate.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

class DbgOut : public  kdlib::windbg::WindbgOut
{
public:

    DbgOut() {
        int a = 10;
    }

    void flush() {
    }

    std::wstring encoding() {
        return L"ascii";
    }
};

///////////////////////////////////////////////////////////////////////////////

class DbgIn : public kdlib::windbg::WindbgIn
{
public:

    std::wstring readline() {
        AutoRestorePyState  pystate;
        return kdlib::windbg::WindbgIn::readline();
    }

    std::wstring encoding() {
        return L"ascii";
    }
};

///////////////////////////////////////////////////////////////////////////////

}
