#pragma once

#include "kdlib/windbg.h"

#include "pythreadstate.h"

namespace pykd {

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

    SysDbgOut() {
         m_state = PyThreadState_Get();
    }

    virtual void write( const std::wstring& str)  {
        AutoSavePythonState  pystate( &m_state );
        python::object       sys = python::import("sys");
        sys.attr("stdout").attr("write")( str );
    }

    virtual void writedml( const std::wstring& str) {
        AutoSavePythonState  pystate( &m_state );
        python::object       sys = python::import("sys");
        sys.attr("stdout").attr("write")(str);
    }

private:

    PyThreadState*    m_state;
};

///////////////////////////////////////////////////////////////////////////////

class SysDbgIn : public DbgIn
{
public:

    SysDbgIn() {
         m_state = PyThreadState_Get();
    }

    virtual std::wstring readline() {
        AutoSavePythonState  pystate( &m_state );
        python::object    sys = python::import("sys");
        return python::extract<std::wstring>( sys.attr("stdin").attr("readline") );
    }

private:

    PyThreadState*    m_state;
};

///////////////////////////////////////////////////////////////////////////////

inline void dprint( const std::wstring &str, bool dml = false )
{
    AutoRestorePyState  pystate;
    kdlib::dprint(str,dml);
}

///////////////////////////////////////////////////////////////////////////////

inline void dprintln( const std::wstring &str, bool dml = false )
{
    AutoRestorePyState  pystate;
    kdlib::dprintln(str,dml);
}

///////////////////////////////////////////////////////////////////////////////

inline void eprint( const std::wstring &str )
{
    AutoRestorePyState  pystate;
    kdlib::eprint(str);
}

///////////////////////////////////////////////////////////////////////////////

inline void eprintln( const std::wstring &str )
{
    AutoRestorePyState  pystate;
    kdlib::eprintln(str);
}

///////////////////////////////////////////////////////////////////////////////

}
