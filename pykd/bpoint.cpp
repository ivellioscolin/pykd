// 
// Breakpoints management
// 

#include "stdafx.h"
#include "bpoint.h"
#include "dbgengine.h"


///////////////////////////////////////////////////////////////////////////////

namespace pykd {

ULONG setSoftwareBp(ULONG64 offset, BpCallback &callback /*= BpCallback()*/)
{
    offset = addr64( offset );

    return breakPointSet( offset );
}

///////////////////////////////////////////////////////////////////////////////

ULONG setHardwareBp(ULONG64 offset, ULONG size, ULONG accessType, BpCallback &callback /*= BpCallback()*/ )
{
    offset = addr64( offset );

    return breakPointSet( offset, true, size, accessType );
}

///////////////////////////////////////////////////////////////////////////////






//static IDebugBreakpoint *setBreakPoint(
//    IDebugControl4 *control,
//    ULONG bpType,
//    ULONG64 addr
//)
//{
//    IDebugBreakpoint *bp;
//    HRESULT hres = control->AddBreakpoint(bpType, DEBUG_ANY_ID, &bp);
//    if (S_OK != hres)
//        throw DbgException("IDebugControl::AddBreakpoint", hres);
//
//    hres = bp->SetOffset(addr);
//    if (S_OK != hres)
//    {
//        control->RemoveBreakpoint(bp);
//        throw DbgException("IDebugBreakpoint::SetOffset", hres);
//    }
//
//    ULONG bpFlags;
//    hres = bp->GetFlags(&bpFlags);
//    if (S_OK != hres)
//    {
//        control->RemoveBreakpoint(bp);
//        throw DbgException("IDebugBreakpoint::GetFlags", hres);
//    }
//
//    bpFlags |= DEBUG_BREAKPOINT_ENABLED;
//    hres = bp->SetFlags(bpFlags);
//    if (S_OK != hres)
//    {
//        control->RemoveBreakpoint(bp);
//        throw DbgException("IDebugBreakpoint::SetFlags", hres);
//    }
//
//    return bp;
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//static BPOINT_ID getBpId(IDebugBreakpoint *bp, IDebugControl4 *control = NULL)
//{
//    BPOINT_ID Id;
//    HRESULT hres = bp->GetId(&Id);
//    if (S_OK != hres)
//    {
//        if (control)
//            control->RemoveBreakpoint(bp);
//        throw DbgException("IDebugBreakpoint::GetId", hres);
//    }
//    return Id;
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//BPOINT_ID DebugClient::setSoftwareBp(ULONG64 addr, BpCallback &callback /*= BpCallback()*/)
//{
//    addr = addr64(addr);
//    IDebugBreakpoint *bp = setBreakPoint(m_control, DEBUG_BREAKPOINT_CODE, addr);
//
//    const BPOINT_ID Id = getBpId(bp, m_control);
//
//    if (!callback.is_none())
//    {
//        boost::recursive_mutex::scoped_lock mapBpLock(*m_bpCallbacks.m_lock);
//        m_bpCallbacks.m_map[Id] = callback;
//    }
//
//    return Id;
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//BPOINT_ID DebugClient::setHardwareBp(ULONG64 addr, ULONG size, ULONG accessType, BpCallback &callback /*= BpCallback()*/)
//{
//    addr = addr64(addr);
//    IDebugBreakpoint *bp = setBreakPoint(m_control, DEBUG_BREAKPOINT_DATA, addr);
//
//    HRESULT hres = bp->SetDataParameters(size, accessType);
//    if (S_OK != hres)
//    {
//        m_control->RemoveBreakpoint(bp);
//        throw DbgException("IDebugBreakpoint::SetDataParameters", hres);
//    }
//
//    const BPOINT_ID Id = getBpId(bp, m_control);
//
//    if (!callback.is_none())
//    {
//        boost::recursive_mutex::scoped_lock mapBpLock(*m_bpCallbacks.m_lock);
//        m_bpCallbacks.m_map[Id] = callback;
//    }
//
//    return Id;
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//python::list DebugClient::getAllBp()
//{
//    ULONG numberOfBps;
//    HRESULT hres = m_control->GetNumberBreakpoints(&numberOfBps);
//    if (S_OK != hres)
//        throw DbgException("IDebugControl::GetNumberBreakpoints", hres);
//
//    python::list lstIds;
//
//    for (ULONG i =0; i < numberOfBps; ++i)
//    {
//        IDebugBreakpoint *bp;
//        hres = m_control->GetBreakpointByIndex(i, &bp);
//        if (S_OK != hres)
//            throw DbgException("IDebugControl::GetBreakpointByIndex", hres);
//        lstIds.append( getBpId(bp) );
//    }
//
//    return lstIds;
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//void DebugClient::removeBp(BPOINT_ID Id)
//{
//    IDebugBreakpoint *bp;
//    HRESULT hres = m_control->GetBreakpointById(Id, &bp);
//    if (S_OK != hres)
//        throw DbgException("IDebugControl::GetBreakpointById", hres);
//
//    hres = m_control->RemoveBreakpoint(bp);
//    if (S_OK != hres)
//        throw DbgException("IDebugControl::RemoveBreakpoint", hres);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//void DebugClient::removeAllBp()
//{
//    ULONG numberOfBps;
//    do {
//        HRESULT hres = m_control->GetNumberBreakpoints(&numberOfBps);
//        if (S_OK != hres)
//            throw DbgException("IDebugControl::GetNumberBreakpoints", hres);
//        if (!numberOfBps)
//            break;
//
//        IDebugBreakpoint *bp;
//        hres = m_control->GetBreakpointByIndex(0, &bp);
//        if (S_OK != hres)
//            throw DbgException("IDebugControl::GetBreakpointByIndex", hres);
//
//        hres = m_control->RemoveBreakpoint(bp);
//        if (S_OK != hres)
//            throw DbgException("IDebugControl::RemoveBreakpoint", hres);
//
//    } while (numberOfBps);
//}

////////////////////////////////////////////////////////////////////////////////

}   // end pykd namespace
