// 
// Breakpoints management
// 

#include "stdafx.h"
#include "bpoint.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

static IDebugBreakpoint *setBreakPoint(
    IDebugControl4 *control,
    ULONG bpType,
    ULONG64 addr
)
{
    IDebugBreakpoint *bp;
    HRESULT hres = control->AddBreakpoint(bpType, DEBUG_ANY_ID, &bp);
    if (S_OK != hres)
        throw DbgException("IDebugControl::AddBreakpoint", hres);

    hres = bp->SetOffset(addr);
    if (S_OK != hres)
    {
        control->RemoveBreakpoint(bp);
        throw DbgException("IDebugBreakpoint::SetOffset", hres);
    }

    ULONG bpFlags;
    hres = bp->GetFlags(&bpFlags);
    if (S_OK != hres)
    {
        control->RemoveBreakpoint(bp);
        throw DbgException("IDebugBreakpoint::GetFlags", hres);
    }

    bpFlags |= DEBUG_BREAKPOINT_ENABLED;
    hres = bp->SetFlags(bpFlags);
    if (S_OK != hres)
    {
        control->RemoveBreakpoint(bp);
        throw DbgException("IDebugBreakpoint::SetFlags", hres);
    }

    return bp;
}

////////////////////////////////////////////////////////////////////////////////

static ULONG getBpId(IDebugBreakpoint *bp, IDebugControl4 *control = NULL)
{
    ULONG Id;
    HRESULT hres = bp->GetId(&Id);
    if (S_OK != hres)
    {
        if (control)
            control->RemoveBreakpoint(bp);
        throw DbgException("IDebugBreakpoint::GetId", hres);
    }
    return Id;
}

////////////////////////////////////////////////////////////////////////////////

ULONG DebugClient::setSoftwareBp(ULONG64 addr)
{
    addr = addr64(addr);
    IDebugBreakpoint *bp = setBreakPoint(m_control, DEBUG_BREAKPOINT_CODE, addr);

    return getBpId(bp, m_control);
}

////////////////////////////////////////////////////////////////////////////////

ULONG DebugClient::setHardwareBp(ULONG64 addr, ULONG size, ULONG accessType)
{
    addr = addr64(addr);
    IDebugBreakpoint *bp = setBreakPoint(m_control, DEBUG_BREAKPOINT_DATA, addr);

    HRESULT hres = bp->SetDataParameters(size, accessType);
    if (S_OK != hres)
    {
        m_control->RemoveBreakpoint(bp);
        throw DbgException("IDebugBreakpoint::SetDataParameters", hres);
    }

    return getBpId(bp, m_control);
}

////////////////////////////////////////////////////////////////////////////////

python::list DebugClient::getAllBp()
{
    ULONG numberOfBps;
    HRESULT hres = m_control->GetNumberBreakpoints(&numberOfBps);
    if (S_OK != hres)
        throw DbgException("IDebugControl::GetNumberBreakpoints", hres);

    python::list lstIds;

    for (ULONG i =0; i < numberOfBps; ++i)
    {
        IDebugBreakpoint *bp;
        hres = m_control->GetBreakpointByIndex(i, &bp);
        if (S_OK != hres)
            throw DbgException("IDebugControl::GetBreakpointByIndex", hres);
        lstIds.append( getBpId(bp) );
    }

    return lstIds;
}

////////////////////////////////////////////////////////////////////////////////

void DebugClient::removeBp(ULONG Id)
{
    IDebugBreakpoint *bp;
    HRESULT hres = m_control->GetBreakpointById(Id, &bp);
    if (S_OK != hres)
        throw DbgException("IDebugControl::GetBreakpointById", hres);

    hres = m_control->RemoveBreakpoint(bp);
    if (S_OK != hres)
        throw DbgException("IDebugControl::RemoveBreakpoint", hres);
}

////////////////////////////////////////////////////////////////////////////////

void DebugClient::removeAllBp()
{
    ULONG numberOfBps;
    do {
        HRESULT hres = m_control->GetNumberBreakpoints(&numberOfBps);
        if (S_OK != hres)
            throw DbgException("IDebugControl::GetNumberBreakpoints", hres);
        if (!numberOfBps)
            break;

        IDebugBreakpoint *bp;
        hres = m_control->GetBreakpointByIndex(0, &bp);
        if (S_OK != hres)
            throw DbgException("IDebugControl::GetBreakpointByIndex", hres);

        hres = m_control->RemoveBreakpoint(bp);
        if (S_OK != hres)
            throw DbgException("IDebugControl::RemoveBreakpoint", hres);

    } while (numberOfBps);
}

////////////////////////////////////////////////////////////////////////////////

}   // namespace pykd


////////////////////////////////////////////////////////////////////////////////
