
#include "stdafx.h"

#include <boost\python\tuple.hpp>
#include <boost\algorithm\string\case_conv.hpp>

#include "context.h"
#include "stkframe.h"

/////////////////////////////////////////////////////////////////////////////////

namespace pykd {


////////////////////////////////////////////////////////////////////////////////
// Fill 32-bit register context
////////////////////////////////////////////////////////////////////////////////
template <typename TContext>
void FillRegistersFromContext32(
    ThreadContext::RegValues &regValues,
    const TContext &Context
)
{
    regValues[CV_REG_DR0] = Context.Dr0;
    regValues[CV_REG_DR1] = Context.Dr1;
    regValues[CV_REG_DR2] = Context.Dr2;
    regValues[CV_REG_DR3] = Context.Dr3;
    regValues[CV_REG_DR6] = Context.Dr6;
    regValues[CV_REG_DR7] = Context.Dr7;

    regValues[CV_REG_GS] = Context.SegGs;
    regValues[CV_REG_FS] = Context.SegFs;
    regValues[CV_REG_ES] = Context.SegEs;
    regValues[CV_REG_DS] = Context.SegDs;

    regValues[CV_REG_EDI] = Context.Edi;
    regValues[CV_REG_ESI] = Context.Esi;
    regValues[CV_REG_EBX] = Context.Ebx;
    regValues[CV_REG_EDX] = Context.Edx;
    regValues[CV_REG_ECX] = Context.Ecx;
    regValues[CV_REG_EAX] = Context.Eax;

    regValues[CV_REG_EBP] = Context.Ebp;

    regValues[CV_REG_ESP] = Context.Esp;
    regValues[CV_REG_SS] = Context.SegSs;

    regValues[CV_REG_EIP] = Context.Eip;
    regValues[CV_REG_CS] = Context.SegCs;

    regValues[CV_REG_EFLAGS] = Context.EFlags;
}

/////////////////////////////////////////////////////////////////////////////////

struct CvRegName
{
    CvRegName(CV_HREG_e cvValue, const std::string &name) 
        : m_cvValue( cvValue ), m_name( boost::to_lower_copy(name) )
    {
    }
    CV_HREG_e m_cvValue;
    std::string m_name;
};
#define _REG_NAME(prefix, regName) CvRegName(CV_##prefix##regName, #regName)

/////////////////////////////////////////////////////////////////////////////////
#define _REG_X86(regName) _REG_NAME(REG_, regName)
static const CvRegName g_x86Registers[] = {
    _REG_X86(DR0), _REG_X86(DR1), _REG_X86(DR2), _REG_X86(DR3), _REG_X86(DR6), _REG_X86(DR7),
    _REG_X86(GS), _REG_X86(FS), _REG_X86(ES), _REG_X86(DS),
    _REG_X86(EDI), _REG_X86(EBX), _REG_X86(EDX), _REG_X86(ECX), _REG_X86(EAX),
    _REG_X86(EBP), _REG_X86(ESP), _REG_X86(SS),
    _REG_X86(EIP), _REG_X86(CS),
    CvRegName(CV_REG_EFLAGS, "efl")
};
#undef  _REG_X86

/////////////////////////////////////////////////////////////////////////////////
#define _REG_X64(regName) _REG_NAME(AMD64_, regName)
static const CvRegName g_x64Registers[] = {
    _REG_X64(MXCSR),
    _REG_X64(CS), _REG_X64(DS), _REG_X64(ES), _REG_X64(FS), _REG_X64(GS), _REG_X64(SS),

    _REG_X64(DR0), _REG_X64(DR1), _REG_X64(DR2), _REG_X64(DR3), _REG_X64(DR6), _REG_X64(DR7),

    _REG_X64(RAX), _REG_X64(RCX), _REG_X64(RDX), _REG_X64(RBX), _REG_X64(RSP), _REG_X64(RBP), _REG_X64(RSI), _REG_X64(RDI), 
    _REG_X64(R8), _REG_X64(R9), _REG_X64(R10), _REG_X64(R11), _REG_X64(R12), _REG_X64(R13), _REG_X64(R14), _REG_X64(R15), 

    _REG_X64(RIP),
    CvRegName(CV_AMD64_EFLAGS, "efl")
};
#undef  _REG_X64

#undef  _REG_NAME

/////////////////////////////////////////////////////////////////////////////////

void ThreadContext::getI386Context()
{
    queryRegisters(g_x86Registers, _countof(g_x86Registers));
}

/////////////////////////////////////////////////////////////////////////////////

void ThreadContext::getAmd64Context()
{
    queryRegisters(g_x64Registers, _countof(g_x64Registers));
}

/////////////////////////////////////////////////////////////////////////////////

ThreadContext::ThreadContext( IDebugClient4 *client ) :
    pykd::DbgObject( client )
{
    HRESULT hres = m_control->GetEffectiveProcessorType(&m_processorType);
    if (S_OK != hres)
        throw DbgException( "IDebugControl::GetEffectiveProcessorType", hres );

    switch (m_processorType)
    {
    case IMAGE_FILE_MACHINE_I386:
        getI386Context();
        return;

    case IMAGE_FILE_MACHINE_AMD64:
        getAmd64Context();
        return;
    }

    throwUnsupportedProcessor(__FUNCTION__);
}

/////////////////////////////////////////////////////////////////////////////////

ThreadContext::ThreadContext( 
    IDebugClient4 *client,
    ULONG processorType
)   : pykd::DbgObject(client)
    , m_processorType(processorType)
{
}

/////////////////////////////////////////////////////////////////////////////////

void ThreadContext::queryRegisters(
    const CvRegName *regs,
    ULONG countOfRegs
)
{
    HRESULT hres;

    std::vector<ULONG> regIndices( countOfRegs );

    for (ULONG i = 0; i < countOfRegs; ++i)
    {
        hres = m_registers->GetIndexByName(regs[i].m_name.c_str(), &regIndices[i]);
        if (S_OK != hres)
            throw DbgException( "IDebugRegisters::GetIndexByName", hres);
    }

    std::vector<DEBUG_VALUE> regValues( countOfRegs );
    hres = 
        m_registers->GetValues(
            static_cast<ULONG>(countOfRegs),
            &regIndices[0],
            0,
            &regValues[0]);
    if (S_OK != hres)
        throw DbgException( "IDebugRegisters::GetValues", hres);

    for (ULONG i = 0; i < countOfRegs; ++i)
    {
        const DEBUG_VALUE &regValue = regValues[i];
        switch (regValue.Type)
        {
        case DEBUG_VALUE_INT8:
            m_regValues[regs[i].m_cvValue] = regValue.I8;
            break;
        case DEBUG_VALUE_INT16:
            m_regValues[regs[i].m_cvValue] = regValue.I16;
            break;
        case DEBUG_VALUE_INT32:
            m_regValues[regs[i].m_cvValue] = regValue.I32;
            break;
        case DEBUG_VALUE_INT64:
            m_regValues[regs[i].m_cvValue] = regValue.I64;
            break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////

ContextPtr ThreadContext::getWow64Context( IDebugClient4 *client )
{
    ContextPtr ptrContext( new ThreadContext(client, IMAGE_FILE_MACHINE_I386) );

    ULONG processorType;
    HRESULT hres = ptrContext->m_control->GetEffectiveProcessorType(&processorType);
    if (S_OK != hres)
        throw DbgException( "IDebugControl::GetEffectiveProcessorType", hres );
    if (IMAGE_FILE_MACHINE_I386 != processorType)
        throw DbgException( "Only for WOW64 processor mode" );

    hres = ptrContext->m_control->GetActualProcessorType(&processorType);
    if (S_OK != hres)
        throw DbgException( "IDebugControl::GetActualProcessorType", hres );
    if (IMAGE_FILE_MACHINE_AMD64 != processorType)
        throw DbgException( "Only for WOW64 processor mode" );

    // 
    //  *** undoc ***
    // !wow64exts.r
    // http://www.woodmann.com/forum/archive/index.php/t-11162.html
    // http://www.nynaeve.net/Code/GetThreadWow64Context.cpp
    // 

    ULONG64 teb64Address;
    hres = ptrContext->m_system->GetCurrentThreadTeb(&teb64Address);
    if (S_OK != hres)
        throw DbgException( "IDebugSystemObjects::GetCurrentThreadTeb", hres);

    // ? @@C++(#FIELD_OFFSET(nt!_TEB64, TlsSlots))
    // hardcoded in !wow64exts.r (6.2.8250.0)
    static const ULONG teb64ToTlsOffset = 0x01480;
    static const ULONG WOW64_TLS_CPURESERVED = 1;
    ULONG64 cpuAreaAddress;
    ULONG readedBytes;
    hres = 
        ptrContext->m_dataSpaces->ReadVirtual(
            teb64Address + teb64ToTlsOffset + (sizeof(ULONG64) * WOW64_TLS_CPURESERVED),
            &cpuAreaAddress,
            sizeof(cpuAreaAddress),
            &readedBytes);
    if (S_OK != hres || readedBytes != sizeof(cpuAreaAddress))
        throw DbgException( "IDebugDataSpaces::ReadVirtual", hres);

    // CPU Area is:
    // +00 unknown ULONG
    // +04 WOW64_CONTEXT struct
    static const ULONG cpuAreaToWow64ContextOffset = sizeof(ULONG);
    WOW64_CONTEXT Context = {0};
    hres = 
        ptrContext->m_dataSpaces->ReadVirtual(
            cpuAreaAddress + cpuAreaToWow64ContextOffset,
            &Context,
            sizeof(Context),
            &readedBytes);
    if (S_OK != hres || readedBytes != sizeof(Context))
        throw DbgException( "IDebugDataSpaces::ReadVirtual", hres);

    FillRegistersFromContext32(ptrContext->m_regValues, Context);

    return ptrContext;
}

/////////////////////////////////////////////////////////////////////////////////

ULONG64 ThreadContext::getValue(ULONG cvRegId) const
{
    ULONG64  val;

    if ( getValueNoThrow( cvRegId, val ) )
        return val;

    throw DbgException(__FUNCTION__ ": Register missing");
}

/////////////////////////////////////////////////////////////////////////////////

ULONG64 ThreadContext::getValueByName( const std::string  &regName ) const
{
    std::string     upcaseName = boost::to_upper_copy( regName );

    if ( IMAGE_FILE_MACHINE_I386 == m_processorType )
    {
        for ( ULONG i = 0; i < pyDia::Symbol::cntI386RegName; ++i )
        {
            if ( upcaseName == pyDia::Symbol::i386RegName[i].second )
                return getValue( pyDia::Symbol::i386RegName[i].first );
        }
    }
    else
    {
        for ( ULONG i = 0; i < pyDia::Symbol::cntAmd64RegName; ++i )
        {
            if ( upcaseName == pyDia::Symbol::amd64RegName[i].second )
                return getValue( pyDia::Symbol::amd64RegName[i].first );
        }
    }

    throwUnsupportedProcessor(__FUNCTION__);
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

ContextPtr ThreadContext::forkByStackFrame(const StackFrame &stkFrmae) const
{
    ContextPtr newContext( new ThreadContext(*this) );
    switch (m_processorType)
    {
    case IMAGE_FILE_MACHINE_I386:
        newContext->m_regValues[CV_REG_EIP] = stkFrmae.m_instructionOffset;
        newContext->m_regValues[CV_REG_EBP] = stkFrmae.m_frameOffset;
        newContext->m_regValues[CV_REG_ESP] = stkFrmae.m_stackOffset;
        return newContext;

    case IMAGE_FILE_MACHINE_AMD64:
        newContext->m_regValues[CV_AMD64_RIP] = stkFrmae.m_instructionOffset;
        newContext->m_regValues[CV_AMD64_RBP] = stkFrmae.m_frameOffset;
        newContext->m_regValues[CV_AMD64_RSP] = stkFrmae.m_stackOffset;
        return newContext;
    }

    throwUnsupportedProcessor(__FUNCTION__);
}

/////////////////////////////////////////////////////////////////////////////////

python::object ThreadContext::getByIndex(ULONG ind) const
{
    RegValues::const_iterator it = m_regValues.begin();
    for (ULONG i = 0; it != m_regValues.end(); ++i, ++it )
    {
        if (i == ind)
        {
            switch (m_processorType)
            {
            case IMAGE_FILE_MACHINE_I386:

                for ( ULONG j = 0; j < pyDia::Symbol::cntI386RegName; ++j )
                    if ( pyDia::Symbol::i386RegName[j].first == it->first )
                        return python::make_tuple( it->first, pyDia::Symbol::i386RegName[j].second, it->second);
              
                break;


            case IMAGE_FILE_MACHINE_AMD64:

                for ( ULONG j = 0; j < pyDia::Symbol::cntAmd64RegName; ++j )
                    if ( pyDia::Symbol::amd64RegName[j].first == it->first )
                        return python::make_tuple( it->first, pyDia::Symbol::amd64RegName[j].second, it->second);
              
                break;
            }
        }            
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

void ThreadContext::throwUnsupportedProcessor(PCSTR szFunction) const
{
    std::stringstream sstream;
    sstream << szFunction << ":\n";
    sstream << "Unsupported processor type: 0x" << std::hex << m_processorType;
    throw DbgException( sstream.str() );
}

/////////////////////////////////////////////////////////////////////////////////

std::string ThreadContext::print() const 
{
    std::stringstream    sstr;

    RegValues::const_iterator it = m_regValues.begin();
    for (; it != m_regValues.end(); ++it )
    {
        switch (m_processorType)
        {
        case IMAGE_FILE_MACHINE_I386:

            for ( ULONG j = 0; j < pyDia::Symbol::cntI386RegName; ++j )
                if ( pyDia::Symbol::i386RegName[j].first == it->first )
                    sstr << pyDia::Symbol::i386RegName[j].second << '=' << std::hex << it->second << std::endl;              
          
            break;


        case IMAGE_FILE_MACHINE_AMD64:

            for ( ULONG j = 0; j < pyDia::Symbol::cntAmd64RegName; ++j )
                if ( pyDia::Symbol::amd64RegName[j].first == it->first )
                    sstr << pyDia::Symbol::amd64RegName[j].second << '=' << std::hex << it->second << std::endl;     

            break;
        }

    }

    return sstr.str();
}

/////////////////////////////////////////////////////////////////////////////////

}
