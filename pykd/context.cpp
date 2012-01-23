
#include "stdafx.h"

#include <boost\python\tuple.hpp>

#include "context.h"

/////////////////////////////////////////////////////////////////////////////////

namespace pykd {
    
////////////////////////////////////////////////////////////////////////////////

namespace I386 {
#include "defctxi386.h"
}

/////////////////////////////////////////////////////////////////////////////////

void ThreadContext::getI386Context()
{
    I386::CONTEXT Context = {0};

    HRESULT hres = m_advanced->GetThreadContext(&Context, sizeof(Context));
    if (S_OK != hres)
        throw DbgException( "IDebugAdvanced2::GetThreadContext", hres );

    m_regValues[CV_REG_DR0] = Context.Dr0;
    m_regValues[CV_REG_DR1] = Context.Dr1;
    m_regValues[CV_REG_DR2] = Context.Dr2;
    m_regValues[CV_REG_DR3] = Context.Dr3;
    m_regValues[CV_REG_DR6] = Context.Dr6;
    m_regValues[CV_REG_DR7] = Context.Dr7;

    m_regValues[CV_REG_GS] = Context.SegGs;
    m_regValues[CV_REG_FS] = Context.SegFs;
    m_regValues[CV_REG_ES] = Context.SegEs;
    m_regValues[CV_REG_DS] = Context.SegDs;

    m_regValues[CV_REG_EDI] = Context.Edi;
    m_regValues[CV_REG_ESI] = Context.Esi;
    m_regValues[CV_REG_EBX] = Context.Ebx;
    m_regValues[CV_REG_EDX] = Context.Edx;
    m_regValues[CV_REG_ECX] = Context.Ecx;
    m_regValues[CV_REG_EAX] = Context.Eax;

    m_regValues[CV_REG_EBP] = Context.Ebp;

    m_regValues[CV_REG_ESP] = Context.Esp;
    m_regValues[CV_REG_SS] = Context.SegSs;

    m_regValues[CV_REG_EIP] = Context.Eip;
    m_regValues[CV_REG_CS] = Context.SegCs;

    m_regValues[CV_REG_EFLAGS] = Context.EFlags;
}

/////////////////////////////////////////////////////////////////////////////////

namespace AMD64 {
#include "defctxamd64.h"
}

/////////////////////////////////////////////////////////////////////////////////

void ThreadContext::getAmd64Context()
{
    AMD64::CONTEXT Context = {0};

    HRESULT hres = m_advanced->GetThreadContext(&Context, sizeof(Context));
    if (S_OK != hres)
        throw DbgException( "IDebugAdvanced2::GetThreadContext", hres);

    m_regValues[CV_AMD64_MXCSR] = Context.MxCsr;

    m_regValues[CV_AMD64_CS] = Context.SegCs;
    m_regValues[CV_AMD64_DS] = Context.SegDs;
    m_regValues[CV_AMD64_ES] = Context.SegEs;
    m_regValues[CV_AMD64_FS] = Context.SegFs;
    m_regValues[CV_AMD64_GS] = Context.SegGs;
    m_regValues[CV_AMD64_SS] = Context.SegSs;

    m_regValues[CV_AMD64_EFLAGS] = Context.EFlags;

    m_regValues[CV_AMD64_DR0] = Context.Dr0;
    m_regValues[CV_AMD64_DR1] = Context.Dr1;
    m_regValues[CV_AMD64_DR2] = Context.Dr2;
    m_regValues[CV_AMD64_DR3] = Context.Dr3;
    m_regValues[CV_AMD64_DR6] = Context.Dr6;
    m_regValues[CV_AMD64_DR7] = Context.Dr7;

    m_regValues[CV_AMD64_RAX] = Context.Rax;
    m_regValues[CV_AMD64_RCX] = Context.Rcx;
    m_regValues[CV_AMD64_RDX] = Context.Rdx;
    m_regValues[CV_AMD64_RBX] = Context.Rbx;
    m_regValues[CV_AMD64_RSP] = Context.Rsp;
    m_regValues[CV_AMD64_RBP] = Context.Rbp;
    m_regValues[CV_AMD64_RSI] = Context.Rdi;
    m_regValues[CV_AMD64_RDI] = Context.Rdi;
    m_regValues[CV_AMD64_R8] = Context.R8;
    m_regValues[CV_AMD64_R9] = Context.R9;
    m_regValues[CV_AMD64_R10] = Context.R10;
    m_regValues[CV_AMD64_R11] = Context.R11;
    m_regValues[CV_AMD64_R12] = Context.R12;
    m_regValues[CV_AMD64_R13] = Context.R13;
    m_regValues[CV_AMD64_R14] = Context.R14;
    m_regValues[CV_AMD64_R15] = Context.R15;

    m_regValues[CV_AMD64_RIP] = Context.Rip;
}

/////////////////////////////////////////////////////////////////////////////////

ThreadContext::ThreadContext( IDebugClient4 *client ) :
    pykd::DbgObject( client )
{
    HRESULT hres = m_control->GetExecutingProcessorType(&m_processorType);
    if (S_OK != hres)
        throw DbgException( "IDebugControl::GetExecutingProcessorType", hres );

    switch (m_processorType)
    {
    case IMAGE_FILE_MACHINE_I386:
        getI386Context();
        return;

    case IMAGE_FILE_MACHINE_AMD64:
        getAmd64Context();
        return;
    }

    std::stringstream sstream;
    sstream << __FUNCTION__ << ":\n";
    sstream << "Unsupported processor type: 0x" << std::hex << m_processorType;
    throw DbgException( sstream.str() );
}

/////////////////////////////////////////////////////////////////////////////////

ULONG64 ThreadContext::getValue(ULONG cvRegId) const
{
    ULONG64  val;

    if ( getValueNoThrow( cvRegId, val ) )
        return val;

    throw DbgException(__FUNCTION__ ": Register missing");
}

///////////////////////////////////////////////////////////////////////////////////

bool ThreadContext::getValueNoThrow(ULONG cvRegId, ULONG64 &val) const
{
    if ( getSubValue(cvRegId, val ) )
        return true;

    RegValues::const_iterator it = m_regValues.find(cvRegId);
    if (it == m_regValues.end())
        return false;

    val = it->second;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////

ULONG64 ThreadContext::getIp() const
{
    return getValue(
        IMAGE_FILE_MACHINE_I386 == m_processorType ? CV_REG_EIP : CV_AMD64_RIP
    );
}

/////////////////////////////////////////////////////////////////////////////////

ULONG64 ThreadContext::getRetReg() const
{
    return getValue(
        IMAGE_FILE_MACHINE_I386 == m_processorType ? CV_REG_EAX : CV_AMD64_RAX
    );
}

/////////////////////////////////////////////////////////////////////////////////

ULONG64 ThreadContext::getSp() const
{
    return getValue(
        IMAGE_FILE_MACHINE_I386 == m_processorType ? CV_REG_ESP : CV_AMD64_RSP
    );
}

/////////////////////////////////////////////////////////////////////////////////

python::object ThreadContext::getByIndex(ULONG ind) const
{
    RegValues::const_iterator it = m_regValues.begin();
    for (ULONG i = 0; it != m_regValues.end(); ++i, ++it )
    {
        if (i == ind)
            return python::make_tuple(it->first, it->second);
    }

    throw PyException( PyExc_IndexError, "Index out of range");
}

/////////////////////////////////////////////////////////////////////////////////

struct SubRegister {
    ULONG m_fromReg;
    ULONG m_bitsShift;
    ULONG m_bitsMask;

    void set(ULONG fromReg, ULONG bitsShift, ULONG bitsMask)
    {
        m_fromReg = fromReg; m_bitsShift = bitsShift; m_bitsMask = bitsMask; 
    }
};

/////////////////////////////////////////////////////////////////////////////////


struct SubRegisterMapI386 {

    std::map<ULONG, SubRegister>        subRegs;

    SubRegisterMapI386()
    {
        subRegs[CV_REG_AL].set(CV_REG_EAX, 0x00, 0xff);
        subRegs[CV_REG_CL].set(CV_REG_ECX, 0x00, 0xff);
        subRegs[CV_REG_DL].set(CV_REG_EDX, 0x00, 0xff);
        subRegs[CV_REG_BL].set(CV_REG_EBX, 0x00, 0xff);

        subRegs[CV_REG_AH].set(CV_REG_EAX, 0x08, 0xff);
        subRegs[CV_REG_CH].set(CV_REG_ECX, 0x08, 0xff);
        subRegs[CV_REG_DH].set(CV_REG_EDX, 0x08, 0xff);
        subRegs[CV_REG_BH].set(CV_REG_EBX, 0x08, 0xff);

        subRegs[CV_REG_AX].set(CV_REG_EAX, 0x00, 0xffff);
        subRegs[CV_REG_CX].set(CV_REG_ECX, 0x00, 0xffff);
        subRegs[CV_REG_DX].set(CV_REG_EDX, 0x00, 0xffff);
        subRegs[CV_REG_BX].set(CV_REG_EBX, 0x00, 0xffff);

        subRegs[CV_REG_SP].set(CV_REG_ESP, 0x00, 0xffff);
        subRegs[CV_REG_BP].set(CV_REG_EBP, 0x00, 0xffff);
        subRegs[CV_REG_SI].set(CV_REG_ESI, 0x00, 0xffff);
        subRegs[CV_REG_DI].set(CV_REG_EDI, 0x00, 0xffff);
    }

};

static const std::map<ULONG, SubRegister>  g_SubRegistersI386 = SubRegisterMapI386().subRegs;

/////////////////////////////////////////////////////////////////////////////////

struct SubRegisterMapAmd64 {

    std::map<ULONG, SubRegister>        subRegs; 

    SubRegisterMapAmd64()
    {
        subRegs[CV_AMD64_AL].set(CV_AMD64_RAX, 0x00, 0xff);
        subRegs[CV_AMD64_CL].set(CV_AMD64_RCX, 0x00, 0xff);
        subRegs[CV_AMD64_DL].set(CV_AMD64_RDX, 0x00, 0xff);
        subRegs[CV_AMD64_BL].set(CV_AMD64_RBX, 0x00, 0xff);

        subRegs[CV_AMD64_AH].set(CV_AMD64_RAX, 0x08, 0xff);
        subRegs[CV_AMD64_CH].set(CV_AMD64_RCX, 0x08, 0xff);
        subRegs[CV_AMD64_DH].set(CV_AMD64_RDX, 0x08, 0xff);
        subRegs[CV_AMD64_BH].set(CV_AMD64_RBX, 0x08, 0xff);

        subRegs[CV_AMD64_AX].set(CV_AMD64_RAX, 0x00, 0xffff);
        subRegs[CV_AMD64_CX].set(CV_AMD64_RCX, 0x00, 0xffff);
        subRegs[CV_AMD64_DX].set(CV_AMD64_RDX, 0x00, 0xffff);
        subRegs[CV_AMD64_BX].set(CV_AMD64_RBX, 0x00, 0xffff);

        subRegs[CV_AMD64_SP].set(CV_AMD64_RSP, 0x00, 0xffff);
        subRegs[CV_AMD64_BP].set(CV_AMD64_RBP, 0x00, 0xffff);
        subRegs[CV_AMD64_SI].set(CV_AMD64_RSI, 0x00, 0xffff);
        subRegs[CV_AMD64_DI].set(CV_AMD64_RDI, 0x00, 0xffff);

        subRegs[CV_AMD64_EAX].set(CV_AMD64_RAX, 0x00, 0xffffffff);
        subRegs[CV_AMD64_ECX].set(CV_AMD64_RCX, 0x00, 0xffffffff);
        subRegs[CV_AMD64_EDX].set(CV_AMD64_RDX, 0x00, 0xffffffff);
        subRegs[CV_AMD64_EBX].set(CV_AMD64_RBX, 0x00, 0xffffffff);

        subRegs[CV_AMD64_ESP].set(CV_AMD64_RSP, 0x00, 0xffffffff);
        subRegs[CV_AMD64_EBP].set(CV_AMD64_RBP, 0x00, 0xffffffff);
        subRegs[CV_AMD64_ESI].set(CV_AMD64_RSI, 0x00, 0xffffffff);
        subRegs[CV_AMD64_EDI].set(CV_AMD64_RDI, 0x00, 0xffffffff);

        subRegs[CV_AMD64_R8B].set(CV_AMD64_R8, 0x00, 0xff);
        subRegs[CV_AMD64_R9B].set(CV_AMD64_R9, 0x00, 0xff);
        subRegs[CV_AMD64_R10B].set(CV_AMD64_R10, 0x00, 0xff);
        subRegs[CV_AMD64_R11B].set(CV_AMD64_R11, 0x00, 0xff);
        subRegs[CV_AMD64_R12B].set(CV_AMD64_R12, 0x00, 0xff);
        subRegs[CV_AMD64_R13B].set(CV_AMD64_R13, 0x00, 0xff);
        subRegs[CV_AMD64_R14B].set(CV_AMD64_R14, 0x00, 0xff);
        subRegs[CV_AMD64_R15B].set(CV_AMD64_R15, 0x00, 0xff);

        subRegs[CV_AMD64_R8W].set(CV_AMD64_R8, 0x00, 0xffff);
        subRegs[CV_AMD64_R9W].set(CV_AMD64_R9, 0x00, 0xffff);
        subRegs[CV_AMD64_R10W].set(CV_AMD64_R10, 0x00, 0xffff);
        subRegs[CV_AMD64_R11W].set(CV_AMD64_R11, 0x00, 0xffff);
        subRegs[CV_AMD64_R12W].set(CV_AMD64_R12, 0x00, 0xffff);
        subRegs[CV_AMD64_R13W].set(CV_AMD64_R13, 0x00, 0xffff);
        subRegs[CV_AMD64_R14W].set(CV_AMD64_R14, 0x00, 0xffff);
        subRegs[CV_AMD64_R15W].set(CV_AMD64_R15, 0x00, 0xffff);

        subRegs[CV_AMD64_R8D].set(CV_AMD64_R8, 0x00, 0xffffffff);
        subRegs[CV_AMD64_R9D].set(CV_AMD64_R9, 0x00, 0xffffffff);
        subRegs[CV_AMD64_R10D].set(CV_AMD64_R10, 0x00, 0xffffffff);
        subRegs[CV_AMD64_R11D].set(CV_AMD64_R11, 0x00, 0xffffffff);
        subRegs[CV_AMD64_R12D].set(CV_AMD64_R12, 0x00, 0xffffffff);
        subRegs[CV_AMD64_R13D].set(CV_AMD64_R13, 0x00, 0xffffffff);
        subRegs[CV_AMD64_R14D].set(CV_AMD64_R14, 0x00, 0xffffffff);
        subRegs[CV_AMD64_R15D].set(CV_AMD64_R15, 0x00, 0xffffffff);
    }
};

static const  std::map<ULONG, SubRegister>   g_SubRegistersAmd64 = SubRegisterMapAmd64().subRegs;

/////////////////////////////////////////////////////////////////////////////////

bool ThreadContext::getSubValue(ULONG cvRegId, ULONG64 &val) const
{
    const std::map<ULONG, SubRegister> *subRegs = NULL;
    if (IMAGE_FILE_MACHINE_I386 == m_processorType)
        subRegs = &g_SubRegistersI386;
    else 
        subRegs = &g_SubRegistersAmd64;

    std::map<ULONG, SubRegister>::const_iterator itSubReg = 
        subRegs->find(cvRegId);
    if (itSubReg == subRegs->end())
       return false;

    RegValues::const_iterator itFullReg = m_regValues.find(itSubReg->second.m_fromReg);
    if (itFullReg == m_regValues.end())
        return false;

    val = (itFullReg->second >> itSubReg->second.m_bitsShift) & itSubReg->second.m_bitsMask;

    return true;
}

/////////////////////////////////////////////////////////////////////////////////

}
