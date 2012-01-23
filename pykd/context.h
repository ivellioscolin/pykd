#pragma once

#include <map>
#include <DbgEng.h>
#include <CvConst.h>

#include "context.h"
#include "dbgobj.h"
#include "dbgexcept.h"

namespace pykd {

std::string processorToStr(ULONG processorMode);

////////////////////////////////////////////////////////////////////////////////

class ThreadContext : private DbgObject
{
public:

    ThreadContext( IDebugClient4 *client );

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

    // get processor type
    std::string getProcessorType() const {
        return pykd::processorToStr(m_processorType);
    }

private:

    // query i386 registers
    void getI386Context();

    // query AMD64 registers
    void getAmd64Context();

    // try query as "sub-register"
    bool getSubValue(ULONG cvRegId, ULONG64 &val) const;

private:
    typedef std::map<ULONG, ULONG64> RegValues;
    RegValues m_regValues;

    ULONG m_processorType;
};

////////////////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr< ThreadContext > ContextPtr;

////////////////////////////////////////////////////////////////////////////////

}

