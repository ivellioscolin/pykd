#pragma once

#include "typeinfo.h"
#include "intbase.h"

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

class TypedVar;
typedef boost::shared_ptr<TypedVar>  TypedVarPtr;

///////////////////////////////////////////////////////////////////////////////////

class TypedVar : public intBase {

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

    TypeInfo
    getType() const {
        return m_typeInfo;
    }

    virtual TypedVarPtr  getField( const std::string &fieldName );

    virtual std::string  print() {
        return "TypeVar";
    }

protected:

    virtual ULONG64  getValue() const {
        return m_offset;
    }
    
    virtual void setValue( ULONG64  value) {
        m_offset = value;
    }

private:

    TypeInfo        m_typeInfo;

    ULONG64         m_offset;
};

///////////////////////////////////////////////////////////////////////////////////

class BasicTypedVar : public TypedVar {

public:

    BasicTypedVar ( const TypeInfo& typeInfo, ULONG64 offset ) : TypedVar(typeInfo, offset){}

    TypedVarPtr
    virtual getField( const std::string &fieldName ) {
        throw DbgException("no fields");
    }

    virtual std::string  print() {
        return "BasicTypedVar";
    }

};

///////////////////////////////////////////////////////////////////////////////////

}; // namespace pykd
