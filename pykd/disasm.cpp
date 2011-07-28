#include "stdafx.h"
#include "dbgext.h"
#include "disasm.h"
#include "dbgexcept.h"
#include "dbgmem.h"

/////////////////////////////////////////////////////////////////////////////////

void disasm::doDisasm()
{
    HRESULT     hres;
    char        buffer[0x100];
    ULONG       disasmSize = 0;
    ULONG64     offset = addr64(m_currentOffset);
    ULONG64     endOffset = 0;

    if ( m_beginOffset == 0 )
    {
        ULONG64     currentOffset = 0;

        hres = dbgExt->registers->GetInstructionOffset( &currentOffset );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugRegisters::GetInstructionOffset failed" );

        offset += currentOffset;
    }

    hres = 
        dbgExt->control->Disassemble(
            offset,
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

    m_length = (ULONG)(endOffset - offset);

    m_disasm = std::string( buffer, disasmSize - 2);
}

/////////////////////////////////////////////////////////////////////////////////