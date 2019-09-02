#pragma once

#include "kdlib/windbg.h"

#include "pythreadstate.h"

namespace pykd {


///////////////////////////////////////////////////////////////////////////////

inline void dprint( const std::wstring &str, bool dml = false )
{
     python::object       sys = python::import("sys");

     if (dml && 0 != PyObject_HasAttrString(python::object(sys.attr("stdout")).ptr(), "writedml"))
        sys.attr("stdout").attr("writedml")(str);
     else
        sys.attr("stdout").attr("write")( str );
}
    
///////////////////////////////////////////////////////////////////////////////
    
inline void dprintln( const std::wstring &str, bool dml = false )
{
    pykd::dprint(str + L"\n", dml);
}
    
///////////////////////////////////////////////////////////////////////////////
    
inline void eprint( const std::wstring &str )
{
    python::object  sys = python::import("sys");
    sys.attr("stderr").attr("write")(str);
}
    
///////////////////////////////////////////////////////////////////////////////
    
inline void eprintln( const std::wstring &str )
{
    pykd::eprint(str + L"\n");
}

///////////////////////////////////////////////////////////////////////////////
    
inline void dinput(const std::wstring &str)
{
    AutoRestorePyState  pystate;
    kdlib::dinput(str);
}

inline void setStatusMessage(const std::wstring &str)
{
    AutoRestorePyState  pystate;
    kdlib::setStatusMessage(str);
}

///////////////////////////////////////////////////////////////////////////////

class DbgOut : public  kdlib::windbg::WindbgOut
{
public:

    virtual void write( const std::wstring& str ) {
        AutoRestorePyState  pystate;
        kdlib::windbg::WindbgOut::write(str);
    }

    virtual void writedml( const std::wstring& str ) {
        AutoRestorePyState  pystate;
        kdlib::windbg::WindbgOut::writedml(str);
    }

    void flush() {
    }

    std::wstring encoding() {
        return L"ascii";
    }

    bool closed() {
        return false;
    }

    bool isatty() {
        return false;
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

    bool closed() {
        return false;
    }
};

///////////////////////////////////////////////////////////////////////////////

inline
bool isWindbgExt()
{
    python::object  sys = python::import("sys");
    return 0 != PyObject_HasAttrString(python::object(sys.attr("stdout")).ptr(), "writedml");
}

///////////////////////////////////////////////////////////////////////////////

}
