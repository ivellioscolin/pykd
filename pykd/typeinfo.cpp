#include "stdafx.h"

#include "typeinfo.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

TypeInfo::TypeInfo( pyDia::GlobalScopePtr &diaScope, const std::string &symName ) : 
    m_offset( 0 )
{
    pyDia::SymbolPtr  typeSym = diaScope->getChildByName( symName );

    if ( typeSym->getSymTag() == SymTagData )
    {
        m_dia = typeSym->getType();
    }
    else
    {
        m_dia = typeSym;
    }
}

///////////////////////////////////////////////////////////////////////////////////

std::string 
TypeInfo::getName() 
{
    std::stringstream    sstr;

    pyDia::SymbolPtr    diaptr = m_dia;
    
    int                 symtag = diaptr->getSymTag();

    while( symtag == SymTagArrayType || symtag == SymTagPointerType )
    {
        if ( symtag == SymTagArrayType )
        {
            sstr << '[' << diaptr->getCount() << ']';
        }
        else
        {
            sstr << '*';
        }

        diaptr = diaptr->getType();
        symtag = diaptr->getSymTag();
    }

    std::string    typeName = symtag == SymTagBaseType ?
        diaptr->getBasicTypeName( diaptr->getBaseType() ) :
        diaptr->getName();

    typeName += sstr.str();

    return typeName;
};

///////////////////////////////////////////////////////////////////////////////////

}; // end namespace pykd