#pragma once


#include <DbgEng.h>
#include <atlbase.h>
#include <comutil.h>

#include <string>
#include <vector>

#include "pycontext.h"
#include "pyclass.h"

//////////////////////////////////////////////////////////////////////////////

class DbgOut
{
public:

    DbgOut(PDEBUG_CLIENT client) :
        m_control(client)
    {}

    void write(const std::wstring& str)
    {
        AutoRestorePyState  pystate;

        m_control->ControlledOutputWide(
            DEBUG_OUTCTL_THIS_CLIENT,
            DEBUG_OUTPUT_NORMAL,
            L"%ws",
            str.c_str()
            );

    }

    void writedml(const std::wstring& str)
    {
        AutoRestorePyState  pystate;

        m_control->ControlledOutputWide(
            DEBUG_OUTCTL_THIS_CLIENT | DEBUG_OUTCTL_DML,
            DEBUG_OUTPUT_NORMAL,
            L"%ws",
            str.c_str()
            );
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

public:

    BEGIN_PYTHON_METHOD_MAP(DbgOut, "dbgout")
       PYTHON_METHOD1("write", write, "write");
       PYTHON_METHOD1("writedml", writedml, "writedml");
       PYTHON_METHOD0("flush", flush, "flush");
       PYTHON_PROPERTY("encoding", encoding, "encoding");
       PYTHON_PROPERTY("closed", closed, "closed");
       PYTHON_METHOD0("isatty", isatty, "isatty");
    END_PYTHON_METHOD_MAP

private:

    CComQIPtr<IDebugControl4>  m_control;

};

///////////////////////////////////////////////////////////////////////////////

class DbgIn
{
public:

    DbgIn(PDEBUG_CLIENT client) :
        m_control(client)
    {}

    std::wstring readline()
    {
        AutoRestorePyState  pystate;

        std::vector<wchar_t>  inputBuffer(0x10000);

        ULONG  read = 0;
        m_control->InputWide(&inputBuffer[0], static_cast<ULONG>(inputBuffer.size()), &read);

        std::wstring  inputstr = std::wstring(&inputBuffer[0]);

        return inputstr.empty() ? L"\n" : inputstr;
    }

    bool closed() {
        return false;
    }

public:

    BEGIN_PYTHON_METHOD_MAP(DbgIn, "dbgin")
        PYTHON_METHOD0("readline", readline, "readline");
        PYTHON_PROPERTY("closed", closed, "closed");
    END_PYTHON_METHOD_MAP
    
private:

    CComQIPtr<IDebugControl4>  m_control;
};

//////////////////////////////////////////////////////////////////////////////

