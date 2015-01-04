#pragma once 

#include "kdlib/disasm.h"

#include "pythreadstate.h"
#include "stladaptor.h"

namespace pykd {

inline kdlib::Disasm*  loadDisasm()
{
    AutoRestorePyState  pystate;
    return new kdlib::Disasm();
}


inline kdlib::Disasm*  loadDisasmWithOffset(kdlib::MEMOFFSET_64 offset)
{
    AutoRestorePyState  pystate;
    return new kdlib::Disasm(offset);
}

class DisasmAdapter  {

public:

    static std::wstring  disassemble(kdlib::Disasm& disasm) 
    {
        AutoRestorePyState  pystate;
        return disasm.disassemble();
    }

    static std::wstring  jump(kdlib::Disasm& disasm, kdlib::MEMOFFSET_64 offset) 
    {
        AutoRestorePyState  pystate;
        return disasm.jump(offset);
    }

    static std::wstring jumprel( kdlib::Disasm& disasm, kdlib::MEMDISPLACEMENT delta)
    {
        AutoRestorePyState  pystate;
        return disasm.jumprel(delta);
    }

    static kdlib::MEMOFFSET_64 getNearInstruction( kdlib::Disasm& disasm, kdlib::MEMDISPLACEMENT delta )
    {
        AutoRestorePyState  pystate;
        return disasm.getNearInstruction(delta);
    }

    static std::wstring reset( kdlib::Disasm& disasm )
    {
        AutoRestorePyState  pystate;
        return disasm.reset();
    }

    static std::wstring assembly( kdlib::Disasm& disasm, const std::wstring &instr )
    {
        AutoRestorePyState  pystate;
        return disasm.assembly(instr);
    }

    static std::wstring instruction( kdlib::Disasm& disasm )
    {
        AutoRestorePyState  pystate;
        return disasm.instruction();
    }

    static python::list opcode(kdlib::Disasm& disasm)
    {
        std::vector<unsigned char>  lst;

        do {
            AutoRestorePyState  pystate;
            lst = disasm.opcode();
        } while (false);

        return vectorToList(lst);
    }

    static std::wstring opmnemo(kdlib::Disasm& disasm)
    {
        AutoRestorePyState  pystate;
        return disasm.opmnemo();
    }

    static kdlib::MEMOFFSET_64 begin( kdlib::Disasm& disasm )
    {
        AutoRestorePyState  pystate;
        return disasm.begin();
    }

    static kdlib::MEMOFFSET_64 current( kdlib::Disasm& disasm )
    {
        AutoRestorePyState  pystate;
        return disasm.current();
    }

    static size_t length( kdlib::Disasm& disasm )
    {
        AutoRestorePyState  pystate;
        return disasm.length();
    }

    static kdlib::MEMOFFSET_64 ea( kdlib::Disasm& disasm )
    {
        AutoRestorePyState  pystate;
        return disasm.ea();
    }
};


} // namespace pykd

