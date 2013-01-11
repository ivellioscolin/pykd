#include "stdafx.h"
#include "dbgengine.h"
#include "disasmengine.h"
#include "disasm.h"
#include "dbgexcept.h"


namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

Disasm::Disasm( ULONG64 offset )
    : m_ea(0)
{
    m_beginOffset = addr64(offset);

    if ( m_beginOffset == 0 )
        m_beginOffset = getRegInstructionPointer();

    m_currentOffset = m_beginOffset;

    doDisasm();
}

/////////////////////////////////////////////////////////////////////////////////

void Disasm::doDisasm()
{
    ULONG64     endOffset = 0;

    disasmDisassembly( m_currentOffset, m_disasm, endOffset, m_ea );

    m_length = (ULONG)(endOffset - m_currentOffset);
}

/////////////////////////////////////////////////////////////////////////////////

std::string
Disasm::assembly( const std::string &instr )
{
    ULONG64     endOffset = 0;
    disasmAssemblay( m_currentOffset, instr, endOffset );

    m_currentOffset = endOffset;

    doDisasm();
    
    return m_disasm;
}

/////////////////////////////////////////////////////////////////////////////////

}; // end pykd namespace
