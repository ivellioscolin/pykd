#pragma once

#include "dbgobj.h"
#include "diawrapper.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class TypeInfo {

public:

    TypeInfo( pyDia::SymbolPtr  dia ) :
      m_dia( dia )
      {}
    
private:

    pyDia::SymbolPtr   m_dia;
    
};

///////////////////////////////////////////////////////////////////////////////////

}; // namespace pykd