
#include <stdafx.h>

#include "localvar.h"
#include "module.h"
#include "symengine.h"
#include "dbgengine.h"
#include "typedvar.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

python::dict getLocals()
{
    return getLocalsByFrame( getCurrentStackFrame() );
}

///////////////////////////////////////////////////////////////////////////////

class BuildLocals
{
public:
    BuildLocals( ModulePtr mod, const StackFrame &frame )
        : m_mod(mod), m_frame(frame), m_formalGen(0) 
    {
        m_ipRva = static_cast<ULONG>(m_frame.m_instructionOffset - m_mod->getBase());
    }

    void process( SymbolPtr symParent );

    python::dict &getResult() {
         return m_dct;
    }

protected:
    void addVar(SymbolPtr symVar);

private:
    ModulePtr m_mod;
    const StackFrame &m_frame;
    python::dict m_dct;
    size_t m_formalGen;
    ULONG m_ipRva;
};

///////////////////////////////////////////////////////////////////////////////

void BuildLocals::process(
    SymbolPtr symParent
)
{
    // add vars from current scope
    SymbolPtrList symList = symParent->findChildren(SymTagData);
    SymbolPtrList::iterator itVar = symList.begin();
    for (; itVar != symList.end(); ++itVar)
        addVar(*itVar);

    // find inners scopes
    symList = symParent->findChildren(SymTagBlock);
    SymbolPtrList::iterator itScope = symList.begin();
    for (; itScope != symList.end(); ++itScope)
    {
        SymbolPtr scope = *itScope;
        ULONG scopeRva = scope->getRva();
        if (scopeRva <= m_ipRva && (scopeRva + scope->getSize()) > m_ipRva)
            process(scope);
    }
}

///////////////////////////////////////////////////////////////////////////////

void BuildLocals::addVar(SymbolPtr symVar)
{
    std::string name;
    try
    {
        name = symVar->getName();
        if (name.empty())
            return;
    }
    catch (const SymbolException &except)
    {
        DBG_UNREFERENCED_PARAMETER(except);
        return;
    }

    if (m_dct.has_key(name))
    {
        std::stringstream sstream;
        sstream << name << "#" << std::dec << ++m_formalGen;
        name = sstream.str();
    }

    BOOST_ASSERT(!m_dct.has_key(name));
    if (m_dct.has_key(name))
        return;

    ULONG64 varAddr;
    const LocationType locType = static_cast<LocationType>(symVar->getLocType());
    switch (locType)
    {
    case LocIsStatic:
        varAddr = m_mod->getBase() + symVar->getRva();
        break;

    case LocIsRegRel:
        {
            RegRealativeId rri;
            try
            {
                rri = static_cast<RegRealativeId>(symVar->getRegRealativeId());
            }
            catch (const DbgException &except)
            {
                DBG_UNREFERENCED_PARAMETER(except);
                return;
            }
            varAddr = m_frame.getValue(rri, symVar->getOffset());
        }
        break;

    default:
        BOOST_ASSERT(LocIsEnregistered == locType);
        return;
    }

    TypeInfoPtr typeInfo = TypeInfo::getTypeInfo(symVar);
    TypedVarPtr typedVar = TypedVar::getTypedVarByTypeInfo(typeInfo, varAddr);
    typedVar->setDataKind( symVar->getDataKind() );
    m_dct[name] = typedVar;
}

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

python::dict getLocalsByFrame( const StackFrame &frame )
{
    // query target fuction by $ip register
    ModulePtr mod;
    SymbolPtr func;
    try
    {
        mod = Module::loadModuleByOffset(frame.m_instructionOffset);
        LONG displacemnt;
        func = mod->getSymbolByVa(frame.m_instructionOffset, SymTagFunction, &displacemnt );
    }
    catch (const DbgException &except)
    {
        DBG_UNREFERENCED_PARAMETER(except);
        return python::dict();
    }

#ifdef _DEBUG
    std::string funcName;
    funcName = func->getName();
#endif  // _DEBUG

    if (!IsInDebugRange(func, static_cast<ULONG>(frame.m_instructionOffset - mod->getBase())))
    {
        // not in debug range
        return python::dict();
    }

    BuildLocals buildLocals(mod, frame);
    buildLocals.process(func);
    return buildLocals.getResult();
}

///////////////////////////////////////////////////////////////////////////////

} //end pykd namespace

