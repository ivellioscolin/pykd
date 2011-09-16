#pragma once

/////////////////////////////////////////////////////////////////////////////////

class disasm {

public:

    disasm( ULONG64 offset = 0) :
      m_beginOffset( offset ),
      m_currentOffset( offset ) {
          doDisasm();
      }

      std::string    next() {
        m_currentOffset += m_length;
        doDisasm();
        return m_disasm;
      }


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

ULONG64
assembly( ULONG64 offset, const std::string &instr );

/////////////////////////////////////////////////////////////////////////////////
