#pragma once

#include "kdlib/windbg.h"

///////////////////////////////////////////////////////////////////////////////

class PykdExt : public kdlib::windbg::WindbgExtension
{
public:

    KDLIB_EXT_COMMAND_METHOD(py);

private:

    void startConsole();

    virtual void setUp();

    virtual void tearDown();

};

///////////////////////////////////////////////////////////////////////////////

