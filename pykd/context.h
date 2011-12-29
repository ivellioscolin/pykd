
#include <map>
#include <DbgEng.h>
#include <CvConst.h>

#include "dbgexcept.h"

#pragma once

namespace Ctx {

////////////////////////////////////////////////////////////////////////////////

typedef pykd::DbgException Exception;

////////////////////////////////////////////////////////////////////////////////

class Registers
{
public:
    Registers(
        IDebugControl4 *control,
        IDebugAdvanced2 *advanced
    );

    // get register value by ID
    ULONG64 getValue(ULONG cvRegId) const;
    bool getValueNoThrow(ULONG cvRegId, ULONG64 &val) const;

    // get @$ip pseudo register
    ULONG64 getIp() const;

    // get @$retreg pseudo register
    ULONG64 getRetReg() const;

    // get @$csp pseudo register
    ULONG64 getSp() const;

    // enumerate register values: tuple<CV_REG_ID, VALUE>
    ULONG getCount() const {
        return static_cast<ULONG>( m_regValues.size() );
    }
    python::object getByIndex(ULONG ind) const;

private:

    // query i386 registers
    void getI386Context(
        IDebugAdvanced2 *advanced
    );

    // query AMD64 registers
    void getAmd64Context(
        IDebugAdvanced2 *advanced
    );

private:
    typedef std::map<ULONG, ULONG64> RegValues;
    RegValues m_regValues;

    ULONG m_processorType;
};

////////////////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr< Registers > ContextPtr;

////////////////////////////////////////////////////////////////////////////////

}

