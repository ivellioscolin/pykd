#pragma once

#include <map>
#include <CvConst.h>

#include "context.h"
#include "dbgobj.h"
#include "dbgexcept.h"

namespace pykd {


////////////////////////////////////////////////////////////////////////////////

class ThreadContext;
typedef boost::shared_ptr< ThreadContext > ContextPtr;

class StackFrame;
struct CvRegName;

////////////////////////////////////////////////////////////////////////////////

std::string processorToStr(ULONG processorMode);

////////////////////////////////////////////////////////////////////////////////

class ThreadContext : private DbgObject
{
public:
    typedef std::map<ULONG, ULONG64> RegValues;

    ThreadContext( IDebugClient4 *client );
    static ContextPtr getWow64Context( IDebugClient4 *client );

    // get register value by ID
    ULONG64 getValue(ULONG cvRegId) const;
    ULONG64 getValueByName( const std::string &regName ) const;
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

    ContextPtr forkByStackFrame(const StackFrame &stkFrmae) const;

    std::string print() const;

protected:
    ThreadContext( 
        IDebugClient4 *client,
        ULONG processorType
    );

    void queryRegisters(
        const CvRegName *regs,
        ULONG countOfRegs
    );

    // query i386 registers
    void getI386Context();

    // query AMD64 registers
    void getAmd64Context();

    // try query as "sub-register"
    bool getSubValue(ULONG cvRegId, ULONG64 &val) const;

    void __declspec(noreturn) throwUnsupportedProcessor(PCSTR szFunction) const;

private:
    RegValues m_regValues;

    ULONG m_processorType;
};

////////////////////////////////////////////////////////////////////////////////

}

