#pragma once

namespace pykd {

void disasmAssemblay( ULONG64 offset, const std::string &instruction, ULONG64 &nextOffset );
void disasmDisassembly( ULONG64 offset, std::string &instruction, ULONG64 &nextOffset );

} // end pykd namespace 