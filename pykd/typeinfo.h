#pragma once

#include "dbgobj.h"
#include "diawrapper.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class TypeInfo {

public:

    TypeInfo( pyDia::SymbolPtr  dia ) :
      m_dia( dia ),
      m_offset( 0 )
      {}

    TypeInfo
    getField( const std::string &fieldName ) {
        pyDia::SymbolPtr  field = m_dia->getChildByName( fieldName );
        TypeInfo ti( field->getType() );
        ti.m_offset = field->getOffset();
        return ti;
    }

    std::string 
    getName() {
        return m_dia->isBasicType() ? 
            m_dia->getBasicTypeName( m_dia->getBaseType() ) :
            m_dia->getName();
    }

    ULONG
    getOffset() {
        return m_offset;
    }

    ULONG64
    getSize() {
        return m_dia->getSize();
    }   
  
private:

    pyDia::SymbolPtr    m_dia;

    ULONG               m_offset;
};

///////////////////////////////////////////////////////////////////////////////////

}; // namespace pykd