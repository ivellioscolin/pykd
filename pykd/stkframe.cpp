// 
// Stack frame: DEBUG_STACK_FRAME wrapper
// 

#include "stdafx.h"
#include "stkframe.h"
#include "dbgengine.h"
#include "module.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

StackFrame::StackFrame( const STACK_FRAME_DESC& desc )
{
    m_frameNumber = desc.number;
    m_instructionOffset = desc.instructionOffset;
    m_returnOffset = desc.returnOffset;
    m_frameOffset = desc.frameOffset;
    m_stackOffset = desc.stackOffset;
}

////////////////////////////////////////////////////////////////////////////////

std::string StackFrame::print() const
{
    std::stringstream sstream;

    sstream << std::dec << "(" << m_frameNumber << ")";

    sstream << " ip= 0x" << std::hex << m_instructionOffset;
    sstream << ", ret= 0x" << std::hex << m_returnOffset;
    sstream << ", frame= 0x" << std::hex << m_frameOffset;
    sstream << ", stack= 0x" << std::hex << m_stackOffset;

    return sstream.str();
}


////////////////////////////////////////////////////////////////////////////////

ScopeVarsPtr StackFrame::getLocals()
{
    return ScopeVarsPtr( new LocalVars( shared_from_this() ) );
}

///////////////////////////////////////////////////////////////////////////////

ScopeVarsPtr StackFrame::getParams()
{
    return ScopeVarsPtr( new FunctionParams( shared_from_this() ) );
}

///////////////////////////////////////////////////////////////////////////////

ULONG64 StackFrame::getValue(RegRealativeId rri, LONG64 offset /*= 0*/) const
{
    switch (rri)
    {
    case rriInstructionPointer: return m_instructionOffset + offset;
    case rriStackFrame: return m_frameOffset + offset;
    case rriStackPointer: return m_stackOffset + offset;
    }

    BOOST_ASSERT(!"Unexcepted error");
    throw DbgException(__FUNCTION__ " Unexcepted error" );
}

////////////////////////////////////////////////////////////////////////////////

static bool IsInDebugRange(
    SymbolPtr func,
    ULONG ipRva
)
{
    SymbolPtrList lstFuncDebugStart;
    SymbolPtrList lstFuncDebugEnd;

    try
    {
        lstFuncDebugStart = func->findChildren(SymTagFuncDebugStart);
        if (1 != lstFuncDebugStart.size())
        {
            BOOST_ASSERT(lstFuncDebugStart.empty());
            return true;
        }

        lstFuncDebugEnd = func->findChildren(SymTagFuncDebugEnd);
        if (1 != lstFuncDebugEnd.size())
        {
            BOOST_ASSERT(lstFuncDebugEnd.empty());
            return true;
        }
    }
    catch (const SymbolException &except)
    {
        DBG_UNREFERENCED_PARAMETER(except);
        return true;
    }

    return 
        ((*lstFuncDebugStart.begin())->getRva() <= ipRva) &&
        ((*lstFuncDebugEnd.begin())->getRva() >= ipRva);
}

python::object StackFrame::getLocalByName( const std::string& name )
{
    ModulePtr mod = Module::loadModuleByOffset( m_instructionOffset);

    LONG displacemnt;
    SymbolPtr func = mod->getSymbolByVa( m_instructionOffset, SymTagFunction, &displacemnt );

#ifdef _DEBUG
    std::string funcName;
    funcName = func->getName();
#endif  // _DEBUG

    if (!IsInDebugRange(func, static_cast<ULONG>( m_instructionOffset - mod->getBase())))
    {
        throw DbgException("is not debug range");
    }

    // find var in current scope
    SymbolPtrList symList = func->findChildren(SymTagData);
    SymbolPtrList::iterator itVar = symList.begin();
    SymbolPtr symVar;
    for (; itVar != symList.end(); ++itVar)
    {
        if ( (*itVar)->getName() == name )
        {
            symVar = *itVar;
            break;
        }
    }

    if ( itVar == symList.end() )
    {
        // find inners scopes
        SymbolPtrList scopeList = func->findChildren(SymTagBlock);
        SymbolPtrList::iterator itScope = scopeList.begin();

        ULONG ipRva = static_cast<ULONG>( m_instructionOffset - mod->getBase());

        for (; itScope != scopeList.end() && !symVar; ++itScope)
        {
            SymbolPtr scope = *itScope;
            ULONG scopeRva = scope->getRva();
            if (scopeRva <= ipRva && (scopeRva + scope->getSize()) > ipRva)
            {
                SymbolPtrList symList = scope->findChildren(SymTagData);
                SymbolPtrList::iterator itVar = symList.begin();

                for (; itVar != symList.end(); ++itVar)
                {
                    if ( (*itVar)->getName() == name )
                    {
                        symVar = *itVar;
                        break;
                    }
                }
            }
        }
    }

    if ( !symVar )
        throw DbgException("local var not found");

    ULONG64 varAddr;
    const LocationType locType = static_cast<LocationType>(symVar->getLocType());
    switch (locType)
    {
    case LocIsStatic:
        varAddr = mod->getBase() + symVar->getRva();
        break;

    case LocIsRegRel:
        {
            RegRealativeId rri;
            rri = static_cast<RegRealativeId>(symVar->getRegRealativeId());
            varAddr = getValue(rri, symVar->getOffset());
        }
        break;

    default:
        BOOST_ASSERT(LocIsEnregistered == locType);
        throw DbgException("");
    }

    TypeInfoPtr typeInfo = TypeInfo::getTypeInfo(symVar);
    TypedVarPtr typedVar = TypedVar::getTypedVarByTypeInfo(typeInfo, varAddr);
    typedVar->setDataKind( symVar->getDataKind() );

    return python::object( typedVar );
}

////////////////////////////////////////////////////////////////////////////////

python::object StackFrame::getParamByName( const std::string& name )
{
    ModulePtr mod = Module::loadModuleByOffset( m_instructionOffset);

    LONG displacemnt;
    SymbolPtr func = mod->getSymbolByVa( m_instructionOffset, SymTagFunction, &displacemnt );

#ifdef _DEBUG
    std::string funcName;
    funcName = func->getName();
#endif  // _DEBUG

    if (!IsInDebugRange(func, static_cast<ULONG>( m_instructionOffset - mod->getBase())))
    {
        throw DbgException("is not debug range");
    }

    // find var in current scope
    SymbolPtrList symList = func->findChildren(SymTagData);
    SymbolPtrList::iterator itVar = symList.begin();
    SymbolPtr symVar;
    for (; itVar != symList.end(); ++itVar)
    {
        if ( (*itVar)->getDataKind() == DataIsParam && (*itVar)->getName() == name )
        {
            symVar = *itVar;
            break;
        }
    }

    if ( !symVar )
        throw DbgException("local var not found");

    ULONG64 varAddr;
    const LocationType locType = static_cast<LocationType>(symVar->getLocType());
    switch (locType)
    {
    case LocIsStatic:
        varAddr = mod->getBase() + symVar->getRva();
        break;

    case LocIsRegRel:
        {
            RegRealativeId rri;
            rri = static_cast<RegRealativeId>(symVar->getRegRealativeId());
            varAddr = getValue(rri, symVar->getOffset());
        }
        break;

    default:
        BOOST_ASSERT(LocIsEnregistered == locType);
        throw DbgException("");
    }

    TypeInfoPtr typeInfo = TypeInfo::getTypeInfo(symVar);
    TypedVarPtr typedVar = TypedVar::getTypedVarByTypeInfo(typeInfo, varAddr);
    typedVar->setDataKind( symVar->getDataKind() );

    return python::object( typedVar );
}

////////////////////////////////////////////////////////////////////////////////

bool StackFrame::isContainsLocal( const std::string& name )
{
   ModulePtr mod = Module::loadModuleByOffset( m_instructionOffset);

    LONG displacemnt;
    SymbolPtr func;
    
    try {
        func = mod->getSymbolByVa( m_instructionOffset, SymTagFunction, &displacemnt );
    }
    catch(SymbolException&)
    {
        return false;
    }

#ifdef _DEBUG
    std::string funcName;
    funcName = func->getName();
#endif  // _DEBUG

    if (!IsInDebugRange(func, static_cast<ULONG>( m_instructionOffset - mod->getBase())))
    {
        return false;
    }

    // find var in current scope
    SymbolPtrList symList = func->findChildren(SymTagData);
    SymbolPtrList::iterator itVar = symList.begin();
    for (; itVar != symList.end(); ++itVar)
    {
        if ( (*itVar)->getName() == name )
        {
            return true;
        }
    }

    if ( itVar == symList.end() )
    {
        // find inners scopes
        SymbolPtrList scopeList = func->findChildren(SymTagBlock);
        SymbolPtrList::iterator itScope = scopeList.begin();

        ULONG ipRva = static_cast<ULONG>( m_instructionOffset - mod->getBase());

        for (; itScope != scopeList.end(); ++itScope)
        {
            SymbolPtr scope = *itScope;
            ULONG scopeRva = scope->getRva();
            if (scopeRva <= ipRva && (scopeRva + scope->getSize()) > ipRva)
            {
                SymbolPtrList symList = scope->findChildren(SymTagData);
                SymbolPtrList::iterator itVar = symList.begin();

                for (; itVar != symList.end(); ++itVar)
                {
                    if ( (*itVar)->getName() == name )
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////

bool StackFrame::isContainsParam( const std::string& name )
{
    ModulePtr mod = Module::loadModuleByOffset( m_instructionOffset);

    LONG displacemnt;
    SymbolPtr func;
    
    try {
        func = mod->getSymbolByVa( m_instructionOffset, SymTagFunction, &displacemnt );
    }
    catch(SymbolException&)
    {
        return false;
    }

#ifdef _DEBUG
    std::string funcName;
    funcName = func->getName();
#endif  // _DEBUG

    if (!IsInDebugRange(func, static_cast<ULONG>( m_instructionOffset - mod->getBase())))
    {
        return false;
    }

    // find var in current scope
    SymbolPtrList symList = func->findChildren(SymTagData);
    SymbolPtrList::iterator itVar = symList.begin();
    for (; itVar != symList.end(); ++itVar)
    {
        if ( (*itVar)->getDataKind() == DataIsParam && (*itVar)->getName() == name )
        {
            return true;
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////

ULONG StackFrame::getLocalCount()
{
    ULONG count = 0;

    ModulePtr mod;
    mod = Module::loadModuleByOffset( m_instructionOffset);

    LONG displacemnt;
    SymbolPtr func;
    
    try {
        func = mod->getSymbolByVa( m_instructionOffset, SymTagFunction, &displacemnt );
    }
    catch(SymbolException&)
    {
        return 0;
    }

#ifdef _DEBUG
    std::string funcName;
    funcName = func->getName();
#endif  // _DEBUG

    if (!IsInDebugRange(func, static_cast<ULONG>( m_instructionOffset - mod->getBase())))
    {
        return 0;
    }

    // find var in current scope
    SymbolPtrList symList = func->findChildren(SymTagData);

    SymbolPtrList::iterator it;
    for ( it = symList.begin(); it != symList.end(); it++ )
    {
        if ( (*it)->getName() != "" )
            count++;
    }

   // find inners scopes
    SymbolPtrList scopeList = func->findChildren(SymTagBlock);
    SymbolPtrList::iterator itScope = scopeList.begin();

    ULONG ipRva = static_cast<ULONG>( m_instructionOffset - mod->getBase());

    for (; itScope != scopeList.end(); ++itScope)
    {
        SymbolPtr scope = *itScope;
        ULONG scopeRva = scope->getRva();
        if (scopeRva <= ipRva && (scopeRva + scope->getSize()) > ipRva)
        {
            SymbolPtrList symList = scope->findChildren(SymTagData);
            count += (ULONG)symList.size();
        }
    } 

    return count;
}

////////////////////////////////////////////////////////////////////////////////

ULONG StackFrame::getParamCount()
{
    ULONG count = 0;

    ModulePtr mod;
    mod = Module::loadModuleByOffset( m_instructionOffset);

    LONG displacemnt;
    SymbolPtr func;
    
    try {
        func = mod->getSymbolByVa( m_instructionOffset, SymTagFunction, &displacemnt );
    }
    catch(SymbolException&)
    {
        return 0;
    }

#ifdef _DEBUG
    std::string funcName;
    funcName = func->getName();
#endif  // _DEBUG

    if (!IsInDebugRange(func, static_cast<ULONG>( m_instructionOffset - mod->getBase())))
    {
        return 0;
    }

    // find var in current scope
    SymbolPtrList symList = func->findChildren(SymTagData);

    SymbolPtrList::iterator it;
    for ( it = symList.begin(); it != symList.end(); it++ )
    {
        if ( (*it)->getDataKind() == DataIsParam ) 
            count++;
    }

    return count;
}

////////////////////////////////////////////////////////////////////////////////

python::object  StackFrame::getLocalByIndex( ULONG index )
{
    ModulePtr mod = Module::loadModuleByOffset( m_instructionOffset);

    LONG displacemnt;
    SymbolPtr func;
    
    try {
        func = mod->getSymbolByVa( m_instructionOffset, SymTagFunction, &displacemnt );
    }
    catch(SymbolException&)
    {
        throw PyException( PyExc_IndexError, "index out of range" );
    }

#ifdef _DEBUG
    std::string funcName;
    funcName = func->getName();
#endif  // _DEBUG

    if (!IsInDebugRange(func, static_cast<ULONG>( m_instructionOffset - mod->getBase())))
    {
       throw PyException( PyExc_IndexError, "index out of range" );
    }

    // find var in current scope
    SymbolPtrList symList = func->findChildren(SymTagData);
    SymbolPtrList::iterator itVar = symList.begin();
    SymbolPtr symVar;
    ULONG i = 0;
    for (; itVar != symList.end(); ++itVar, ++i)
    {
        if ( i == index )
        {
            symVar = *itVar;
            break;
        }
    }

    if ( itVar == symList.end() )
    {
        // find inners scopes
        SymbolPtrList scopeList = func->findChildren(SymTagBlock);
        SymbolPtrList::iterator itScope = scopeList.begin();

        ULONG ipRva = static_cast<ULONG>( m_instructionOffset - mod->getBase());

        for (; itScope != scopeList.end() && !symVar; ++itScope)
        {
            SymbolPtr scope = *itScope;
            ULONG scopeRva = scope->getRva();
            if (scopeRva <= ipRva && (scopeRva + scope->getSize()) > ipRva)
            {
                SymbolPtrList symList = scope->findChildren(SymTagData);
                SymbolPtrList::iterator itVar = symList.begin();

                for (; itVar != symList.end(); ++itVar, ++i)
                {
                    if ( i == index )
                    {
                        symVar = *itVar;
                        break;
                    }
                }
            }
        }
    }

    if ( !symVar )
        throw PyException( PyExc_IndexError, "index out of range" );

    ULONG64 varAddr;
    const LocationType locType = static_cast<LocationType>(symVar->getLocType());
    switch (locType)
    {
    case LocIsStatic:
        varAddr = mod->getBase() + symVar->getRva();
        break;

    case LocIsRegRel:
        {
            RegRealativeId rri;
            rri = static_cast<RegRealativeId>(symVar->getRegRealativeId());
            varAddr = getValue(rri, symVar->getOffset());
        }
        break;

    default:
        BOOST_ASSERT(LocIsEnregistered == locType);
        throw DbgException("");
    }

    TypeInfoPtr typeInfo = TypeInfo::getTypeInfo(symVar);
    TypedVarPtr typedVar = TypedVar::getTypedVarByTypeInfo(typeInfo, varAddr);
    typedVar->setDataKind( symVar->getDataKind() );

    return python::object( typedVar );
}

////////////////////////////////////////////////////////////////////////////////

python::object StackFrame::getParamByIndex( ULONG index )
{
    ModulePtr mod = Module::loadModuleByOffset( m_instructionOffset);

    LONG displacemnt;
    SymbolPtr func;
    
    try {
        func = mod->getSymbolByVa( m_instructionOffset, SymTagFunction, &displacemnt );
    }
    catch(SymbolException&)
    {
        throw PyException( PyExc_IndexError, "index out of range" );
    }

#ifdef _DEBUG
    std::string funcName;
    funcName = func->getName();
#endif  // _DEBUG

    if (!IsInDebugRange(func, static_cast<ULONG>( m_instructionOffset - mod->getBase())))
    {
        throw PyException( PyExc_IndexError, "index out of range" );
    }

    // find var in current scope
    SymbolPtrList symList = func->findChildren(SymTagData);
    SymbolPtrList::iterator itVar = symList.begin();
    SymbolPtr symVar;
    for ( ULONG i = 0; itVar != symList.end(); ++itVar )
    {
        if ( (*itVar)->getDataKind() == DataIsParam  )
        {
            if ( i == index )
            {
                symVar = *itVar;
                break;
            }

            i++;
        }
    }

    if ( !symVar )
        throw PyException( PyExc_IndexError, "index out of range" );

    ULONG64 varAddr;
    const LocationType locType = static_cast<LocationType>(symVar->getLocType());
    switch (locType)
    {
    case LocIsStatic:
        varAddr = mod->getBase() + symVar->getRva();
        break;

    case LocIsRegRel:
        {
            RegRealativeId rri;
            rri = static_cast<RegRealativeId>(symVar->getRegRealativeId());
            varAddr = getValue(rri, symVar->getOffset());
        }
        break;

    default:
        BOOST_ASSERT(LocIsEnregistered == locType);
        throw DbgException("");
    }

    TypeInfoPtr typeInfo = TypeInfo::getTypeInfo(symVar);
    TypedVarPtr typedVar = TypedVar::getTypedVarByTypeInfo(typeInfo, varAddr);
    typedVar->setDataKind( symVar->getDataKind() );

    return python::object( typedVar );
}

////////////////////////////////////////////////////////////////////////////////

python::list getCurrentStack()
{
    std::vector<STACK_FRAME_DESC> frames; 
    getStackTrace( frames );

    python::list frameList;

    for ( ULONG i = 0; i < frames.size(); ++i )
    {
        python::object  frameObj( StackFramePtr( new StackFrame( frames.at(i) ) ) );
        frameList.append( frameObj );
    }

    return frameList; 
}

////////////////////////////////////////////////////////////////////////////////

python::list getCurrentStackWow64()
{
    std::vector<STACK_FRAME_DESC> frames; 
    getStackTraceWow64( frames );

    python::list frameList;

    for ( ULONG i = 0; i < frames.size(); ++i )
    {
        python::object  frameObj( StackFramePtr( new StackFrame( frames.at(i) ) ) );
        frameList.append( frameObj );
    }

    return frameList; 
}

////////////////////////////////////////////////////////////////////////////////

StackFramePtr getCurrentStackFrame()
{
    STACK_FRAME_DESC frame; 
    getCurrentFrame( frame );
    return StackFramePtr( new StackFrame( frame ) );
}

///////////////////////////////////////////////////////////////////////////////

ScopeVarsPtr getLocals()
{
    return getCurrentStackFrame()->getLocals();
}

///////////////////////////////////////////////////////////////////////////////

ScopeVarsPtr getParams()
{
    return getCurrentStackFrame()->getParams();
}

///////////////////////////////////////////////////////////////////////////////

} // namespace pykd



