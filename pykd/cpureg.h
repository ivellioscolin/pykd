#pragma once

#include "variant.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class CpuReg : public intBase {

public:

    CpuReg( const std::string &regName );

    CpuReg( ULONG index );

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

std::string getProcessorMode();

std::string getProcessorType();

///////////////////////////////////////////////////////////////////////////////////

}; // end pykd namespace

