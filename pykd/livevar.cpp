
////////////////////////////////////////////////////////////////////////////////

#include <stdafx.h>

#include "dbgclient.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

namespace impl {

////////////////////////////////////////////////////////////////////////////////

struct addLocals {
    python::dict &m_locals;
    const Module &m_module;
    ULONG m_rva;
    Ctx::ContextPtr m_ctx;
    IDebugClient4 *m_client;
    ULONG m_formalNameCounter;

    void append(pyDia::SymbolPtr symParent);

private:
    void appendVar(pyDia::SymbolPtr symData);

    void generateUniqueName(std::string &varName);

    TypedVarPtr getTypeVarByOffset(
        pyDia::SymbolPtr symData,
        ULONG64 varOffset
    );
};

////////////////////////////////////////////////////////////////////////////////

struct Exception : public DbgException {
    Exception() : DbgException("build list of locals: internal exception")
    {
    }
};

////////////////////////////////////////////////////////////////////////////////

void addLocals::append(pyDia::SymbolPtr symParent)
{
    // add all local variables
    pyDia::SymbolPtrList lstLocals = symParent->findChildrenImpl(SymTagData);
    pyDia::SymbolPtrList::iterator it = lstLocals.begin();
    while (it != lstLocals.end())
    {
        try
        {
            appendVar(*it);
        }
        catch (const DbgException &e)
        {
            DBG_UNREFERENCED_LOCAL_VARIABLE(e);
        }
        ++it;
    }

    // process all scopes
    pyDia::SymbolPtrList lstScopes = symParent->findChildrenImpl(SymTagBlock);
    it = lstScopes.begin();
    while ( it != lstScopes.end() )
    {
        const ULONG scopeRva = (*it)->getRva();
        if ( (scopeRva <= m_rva) && (scopeRva + (*it)->getSize() > m_rva) )
            append(*it);
        ++it;
    }
}

////////////////////////////////////////////////////////////////////////////////

void addLocals::appendVar(pyDia::SymbolPtr symData)
{
    TypedVarPtr typedVar;

    std::string varName = symData->getName();

    // check name for unique. f.e. may be may be somewhat parameters
    // with name "__formal"
    generateUniqueName(varName);

    switch (symData->getLocType())
    {
    case LocIsStatic:
        typedVar = 
            getTypeVarByOffset(
                symData,
                m_module.getBase() + symData->getRva() );
        break;

    case LocIsRegRel:
        typedVar = 
            getTypeVarByOffset(
                symData,
                m_ctx->getValue( symData->getRegisterId() )+ symData->getOffset() );
        break;

    case LocIsEnregistered: // FIXME
    default:
        throw Exception();
    }
    typedVar->setDataKind( symData->getDataKind() );
    m_locals[varName] = typedVar;
}

////////////////////////////////////////////////////////////////////////////////

void addLocals::generateUniqueName(std::string &varName)
{
    if ( !m_locals.has_key(varName) )
        return;

    std::string origVarName = varName;
    while ( m_locals.has_key(varName) )
    {
        std::stringstream sstream;
        sstream << origVarName << ++m_formalNameCounter;
        varName = sstream.str();
    }
}

////////////////////////////////////////////////////////////////////////////////

TypedVarPtr addLocals::getTypeVarByOffset(
    pyDia::SymbolPtr symData,
    ULONG64 varOffset
)
{
    pyDia::SymbolPtr symType = symData->getType();

    TypeInfoPtr typeInfo = TypeInfo::getTypeInfo( symType );

    return TypedVar::getTypedVar( m_client, typeInfo, varOffset );
}

////////////////////////////////////////////////////////////////////////////////

static ULONG getUnnamedChildRva(
    pyDia::SymbolPtr symParent,
    ULONG SymTag
)
{
    pyDia::SymbolPtrList childs = symParent->findChildrenImpl(SymTag);
    if (childs.empty())
        throw Exception();

    return (*childs.begin())->getRva();
}

////////////////////////////////////////////////////////////////////////////////

static bool isOutOfDebugRange(
    ULONG rva,
    pyDia::SymbolPtr symFunc
)
{
    try
    {
        if (rva < getUnnamedChildRva(symFunc, SymTagFuncDebugStart))
            return true;

        if (rva > getUnnamedChildRva(symFunc, SymTagFuncDebugEnd))
            return true;
    }
    catch (const DbgException &)
    {
    }
    return false;
}

}

////////////////////////////////////////////////////////////////////////////////

python::dict DebugClient::getLocals(Ctx::ContextPtr ctx OPTIONAL)
{
    if (!ctx)
        ctx = getThreadContext();

    const ULONG64 instrPtr = ctx->getIp();

    Module mod = loadModuleByOffset( instrPtr );
    const ULONG rva = static_cast<ULONG>( instrPtr - mod.getBase() );

    pyDia::GlobalScopePtr globScope = mod.getDia();
    LONG funcDispl;
    pyDia::SymbolPtr symFunc = 
        globScope->findByRvaImpl(rva, SymTagFunction, funcDispl);
    if (impl::isOutOfDebugRange(rva, symFunc))
        return python::dict();  // out of function debug range

    python::dict locals;
    impl::addLocals Locals = { locals, mod, rva, ctx, m_client, 0 };

    Locals.append(symFunc);

    return locals;
}

////////////////////////////////////////////////////////////////////////////////

}

////////////////////////////////////////////////////////////////////////////////
