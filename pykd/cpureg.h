#pragma once

#include "intbase.h"
#include "dbgobj.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class CpuReg : public intBase, protected DbgObject {

public:

    CpuReg( IDebugClient4 *client, const std::string &regName );

    CpuReg( IDebugClient4 *client, ULONG index );

    std::string
    name() const {
        return m_name;
    }

    ULONG index() const {
        return m_index;
    }

    virtual BaseTypeVariant getValue();

private:

    std::string     m_name;

    ULONG           m_index;
};

///////////////////////////////////////////////////////////////////////////////////

CpuReg getRegByName( const std::string &regName );

CpuReg getRegByIndex( ULONG index );

ULONG64 loadMSR( ULONG  msr );

void setMSR( ULONG msr, ULONG64 value);

///////////////////////////////////////////////////////////////////////////////////

}; // end pykd namespace

