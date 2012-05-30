#pragma once

#include "dbgobj.h"

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

class Disasm : private DbgObject {

public:

    Disasm( IDebugClient4 *client, ULONG64 offset = 0 );

    Disasm( ULONG64 offset = 0);

    std::string  disassemble() {
        std::string  s = m_disasm;
        m_currentOffset += m_length;
        doDisasm();
        return s;
    }

    std::string  jump(ULONG64 offset) {
        m_currentOffset = offset;
        doDisasm();
        return disassemble();
    }

    std::string  reset() {
        m_currentOffset = m_beginOffset;
        doDisasm();
        return disassemble();
    }

    std::string
    assembly( const std::string &instr );

    std::string    instruction() const {
        return m_disasm;
    }

    ULONG64 begin() const {
        return m_beginOffset;
    }

    ULONG64 current() const {
        return m_currentOffset;
    }

    ULONG length() const {
        return m_length;
    }

    ULONG64 ea() const {
        return m_ea;
    }

private:

    void doDisasm();

    ULONG64         m_beginOffset;
    ULONG64         m_currentOffset;
    ULONG64         m_ea;
    ULONG           m_length;

    std::string     m_disasm;
};

/////////////////////////////////////////////////////////////////////////////////

} ; // end pykd namespace

