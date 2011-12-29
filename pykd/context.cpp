
#include "stdafx.h"

#include <boost\python\tuple.hpp>

#include "context.h"

/////////////////////////////////////////////////////////////////////////////////

namespace Ctx {

/////////////////////////////////////////////////////////////////////////////////

namespace I386 {
#include "defctxi386.h"
}

/////////////////////////////////////////////////////////////////////////////////

void Registers::getI386Context(
    IDebugAdvanced2 *advanced
)
{
    I386::CONTEXT Context = {0};

    HRESULT hres = advanced->GetThreadContext(&Context, sizeof(Context));
    if (S_OK != hres)
        throw Exception( pykd::buildExceptDesc("IDebugAdvanced2::GetThreadContext", hres) );

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

void Registers::getAmd64Context(
    IDebugAdvanced2 *advanced
)
{
    AMD64::CONTEXT Context = {0};

    HRESULT hres = advanced->GetThreadContext(&Context, sizeof(Context));
    if (S_OK != hres)
        throw Exception( pykd::buildExceptDesc("IDebugAdvanced2::GetThreadContext", hres) );

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

Registers::Registers(
    IDebugControl4 *control,
    IDebugAdvanced2 *advanced
)
{
    HRESULT hres = control->GetExecutingProcessorType(&m_processorType);
    if (S_OK != hres)
        throw Exception( pykd::buildExceptDesc("IDebugControl::GetExecutingProcessorType", hres) );

    switch (m_processorType)
    {
    case IMAGE_FILE_MACHINE_I386:
        getI386Context(advanced);
        return;

    case IMAGE_FILE_MACHINE_AMD64:
        getAmd64Context(advanced);
        return;
    }

    std::stringstream sstream;
    sstream << __FUNCTION__ << ":\n";
    sstream << "Unsupported processor type: 0x" << std::hex << m_processorType;
    throw Exception( sstream.str() );
}

/////////////////////////////////////////////////////////////////////////////////

ULONG64 Registers::getValue(ULONG cvRegId) const
{
    RegValues::const_iterator it = m_regValues.find(cvRegId);
    if (it == m_regValues.end())
        throw Exception(__FUNCTION__ ": Register missing");
    return it->second;
}

/////////////////////////////////////////////////////////////////////////////////

bool Registers::getValueNoThrow(ULONG cvRegId, ULONG64 &val) const
{
    RegValues::const_iterator it = m_regValues.find(cvRegId);
    if (it == m_regValues.end())
        return false;

    val = it->second;
    return true;
}

/////////////////////////////////////////////////////////////////////////////////

ULONG64 Registers::getIp() const
{
    return getValue(
        IMAGE_FILE_MACHINE_I386 == m_processorType ? CV_REG_EIP : CV_AMD64_RIP
    );
}

/////////////////////////////////////////////////////////////////////////////////

ULONG64 Registers::getRetReg() const
{
    return getValue(
        IMAGE_FILE_MACHINE_I386 == m_processorType ? CV_REG_EAX : CV_AMD64_RAX
    );
}

/////////////////////////////////////////////////////////////////////////////////

ULONG64 Registers::getSp() const
{
    return getValue(
        IMAGE_FILE_MACHINE_I386 == m_processorType ? CV_REG_ESP : CV_AMD64_RSP
    );
}

/////////////////////////////////////////////////////////////////////////////////

python::object Registers::getByIndex(ULONG ind) const
{
    RegValues::const_iterator it = m_regValues.begin();
    for (ULONG i = 0; it != m_regValues.end(); ++i, ++it )
    {
        if (i == ind)
            return python::make_tuple(it->first, it->second);
    }

    PyErr_SetString(PyExc_IndexError, "Index out of range");
    python::throw_error_already_set();

    return python::object();
}

/////////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////
