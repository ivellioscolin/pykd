#pragma once

#include <string>

#include "kdlib/windbg.h"

///////////////////////////////////////////////////////////////////////////////

class PykdExt : public kdlib::windbg::WindbgExtension
{
public:

    KDLIB_EXT_COMMAND_METHOD(py);

    static bool isInit();

private:

    void startConsole();

    virtual void setUp();

    virtual void tearDown();

    std::string getScriptFileName( const std::string &scriptName );

    std::vector<std::string> m_paths;
};

///////////////////////////////////////////////////////////////////////////////

class PykdInterruptWatch : public kdlib::windbg::InterruptWatch
{
    virtual bool onInterrupt();

    static int quit(void *);
};

///////////////////////////////////////////////////////////////////////////////
