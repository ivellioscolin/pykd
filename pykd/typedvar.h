#pragma once

#include "typeinfo.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class TypedVar {

public:

    TypedVar ( const TypeInfo& typeInfo, ULONG64 offset ) :
      m_typeInfo( typeInfo ),
      m_offset( offset )
      {}

    ULONG64 getAddress() const {
        return m_offset;
    }

    ULONG getSize() {
        return m_typeInfo.getSize();
    }

private:

    TypeInfo        m_typeInfo;

    ULONG64         m_offset;
};

///////////////////////////////////////////////////////////////////////////////////

}; // namespace pykd
