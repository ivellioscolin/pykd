#pragma once

#include <string>

#include "diawrapper.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class TypeInfo {

public:

    TypeInfo( pyDia::GlobalScopePtr &diaScope, const std::string &symName );

    TypeInfo( pyDia::SymbolPtr  &diaType ) :
        m_offset( 0 ),
        m_dia( diaType )
        {}

    TypeInfo
    getField( const std::string &fieldName ) {
        pyDia::SymbolPtr  field = m_dia->getChildByName( fieldName );
        TypeInfo ti( field->getType() );
        ti.m_offset = field->getOffset();
        return ti;
    }

    std::string 
    getName();

    ULONG
    getOffset() {
        return m_offset;
    }

    ULONG
    getSize() {
        return (ULONG)m_dia->getSize();
    }  

    bool
    isBasicType();

    bool
    isArrayType();

    bool
    isPointer();

    bool
    isUserDefined();
  
private:

    pyDia::SymbolPtr    m_dia;

    ULONG               m_offset;
};

///////////////////////////////////////////////////////////////////////////////////

}; // namespace pykd
