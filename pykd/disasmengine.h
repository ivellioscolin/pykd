#pragma once

namespace pykd {

void disasmAssemblay( ULONG64 offset, const std::string &instruction, ULONG64 &nextOffset );
void disasmDisassembly( ULONG64 offset, std::string &instruction, ULONG64 &nextOffset, ULONG64 &ea );
ULONG64 getNearInstruction( ULONG64 offset, LONG delta );

} // end pykd namespace 