#include "stdafx.h"
#include "disasm.h"
#include "dbgexcept.h"
#include "dbgmem.h"
#include "dbgclient.h"

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

Disasm::Disasm( IDebugClient4 *client, ULONG64 offset ) :
    DbgObject( client )
{
    HRESULT     hres;

    m_beginOffset = addr64(offset);

    if ( m_beginOffset == 0 )
    {
        hres = m_registers->GetInstructionOffset( &m_beginOffset );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugRegisters::GetInstructionOffset failed" );
    }

    m_currentOffset = m_beginOffset;

    doDisasm();
}

/////////////////////////////////////////////////////////////////////////////////

Disasm::Disasm( ULONG64 offset ) :
    DbgObject( g_dbgClient->client() )
{
    HRESULT     hres;

    m_beginOffset = addr64(offset);

    if ( m_beginOffset == 0 )
    {
        hres = m_registers->GetInstructionOffset( &m_beginOffset );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugRegisters::GetInstructionOffset failed" );
    }

    m_currentOffset = m_beginOffset;

    doDisasm();
}

/////////////////////////////////////////////////////////////////////////////////

void Disasm::doDisasm()
{
    HRESULT     hres;
    char        buffer[0x100];
    ULONG       disasmSize = 0;
    ULONG64     endOffset = 0;
    
    hres = 
        m_control->Disassemble(
            m_currentOffset,
            DEBUG_DISASM_EFFECTIVE_ADDRESS,
            buffer,
            sizeof(buffer),
            &disasmSize,
            &endOffset );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::Disassemble failed" );

    hres = m_control->GetDisassembleEffectiveOffset( &m_ea );
    if ( FAILED( hres ) )
        m_ea = 0;

    m_length = (ULONG)(endOffset - m_currentOffset);

    m_disasm = std::string( buffer, disasmSize - 2);
}

/////////////////////////////////////////////////////////////////////////////////

std::string
Disasm::assembly( const std::string &instr )
{
    HRESULT     hres;

    ULONG64     endOffset = 0;
    hres = m_control->Assemble( m_currentOffset, instr.c_str(), &endOffset );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::Assemble failed" );

    m_currentOffset = endOffset;

    doDisasm();

    return m_disasm;
}

/////////////////////////////////////////////////////////////////////////////////

}; // end pykd namespace
