#pragma once

#include "kdlib/windbg.h"

#include "pythreadstate.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

class DbgOut : public  kdlib::windbg::WindbgOut
{
public:

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

class SysDbgOut : public DbgOut 
{
public:

    virtual void write( const std::wstring& str)  {
        python::object       sys = python::import("sys");
        sys.attr("stdout").attr("write")( str );
    }

    virtual void writedml( const std::wstring& str) {
        write(str);
    }
};

///////////////////////////////////////////////////////////////////////////////

class SysDbgIn : public DbgIn
{
public:

    virtual std::wstring readline() {
        python::object    sys = python::import("sys");
        return python::extract<std::wstring>( sys.attr("stdin").attr("readline") );
    }
};

///////////////////////////////////////////////////////////////////////////////

inline void dprint( const std::wstring &str, bool dml = false )
{
    kdlib::dprint(str,dml);
}

///////////////////////////////////////////////////////////////////////////////

inline void dprintln( const std::wstring &str, bool dml = false )
{
    kdlib::dprintln(str,dml);
}

///////////////////////////////////////////////////////////////////////////////

inline void eprint( const std::wstring &str )
{
    kdlib::eprint(str);
}

///////////////////////////////////////////////////////////////////////////////

inline void eprintln( const std::wstring &str )
{
    kdlib::eprintln(str);
}

///////////////////////////////////////////////////////////////////////////////

}
