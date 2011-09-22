#include "stdafx.h"
#include "dbgext.h"
#include "disasm.h"
#include "dbgexcept.h"

/////////////////////////////////////////////////////////////////////////////////

disasm::disasm( ULONG64 offset )
{
    HRESULT     hres;

    m_beginOffset = addr64(offset);

    if ( m_beginOffset == 0 )
    {
        hres = dbgExt->registers->GetInstructionOffset( &m_beginOffset );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugRegisters::GetInstructionOffset failed" );
    }

    m_currentOffset = m_beginOffset;

    doDisasm();
}

/////////////////////////////////////////////////////////////////////////////////

void disasm::doDisasm()
{
    HRESULT     hres;
    char        buffer[0x100];
    ULONG       disasmSize = 0;
    ULONG64     endOffset = 0;
    
    hres = 
        dbgExt->control->Disassemble(
            m_currentOffset,
            DEBUG_DISASM_EFFECTIVE_ADDRESS,
            buffer,
            sizeof(buffer),
            &disasmSize,
            &endOffset );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::Disassemble failed" );

    hres = dbgExt->control->GetDisassembleEffectiveOffset( &m_ea );
    if ( FAILED( hres ) )
        m_ea = 0;

    m_length = (ULONG)(endOffset - m_currentOffset);

    m_disasm = std::string( buffer, disasmSize - 2);
}

/////////////////////////////////////////////////////////////////////////////////

std::string
disasm::assembly( const std::string &instr )
{
    HRESULT     hres;

    ULONG64     endOffset = 0;
    hres = dbgExt->control->Assemble( m_currentOffset, instr.c_str(), &endOffset );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::Assemble failed" );

    m_currentOffset = endOffset;

    doDisasm();

    return m_disasm;
}

/////////////////////////////////////////////////////////////////////////////////

