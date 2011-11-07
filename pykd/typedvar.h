#pragma once

#include "typeinfo.h"
#include "intbase.h"
#include "dbgobj.h"

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

class TypedVar;
typedef boost::shared_ptr<TypedVar>  TypedVarPtr;

///////////////////////////////////////////////////////////////////////////////////

class TypedVar : public intBase, protected DbgObject {

public:

    TypedVar ( const TypeInfo& typeInfo, ULONG64 offset );


    TypedVar ( IDebugClient4 *client, const TypeInfo& typeInfo, ULONG64 offset );

    ULONG64 getAddress() const {
        return m_offset;
    }

    ULONG getSize() const {
        return m_size;
    }

    ULONG getOffset() {
        return m_typeInfo.getOffset();
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
       throw DbgException("can not change");
    }

    TypeInfo                m_typeInfo;

    ULONG64                 m_offset;

    ULONG                   m_size;
};

///////////////////////////////////////////////////////////////////////////////////

class BasicTypedVar : public TypedVar {

public:

    BasicTypedVar ( IDebugClient4 *client, const TypeInfo& typeInfo, ULONG64 offset ) : TypedVar(client, typeInfo, offset){}

    TypedVarPtr
    virtual getField( const std::string &fieldName ) {
        throw DbgException("no fields");
    }

    virtual std::string  print() {
        return "BasicTypedVar";
    }

    virtual ULONG64  getValue() const;

};

///////////////////////////////////////////////////////////////////////////////////

class PtrTypedVar : public TypedVar {

public:

    PtrTypedVar ( IDebugClient4 *client, const TypeInfo& typeInfo, ULONG64 offset ) : TypedVar(client, typeInfo, offset){}

    TypedVarPtr
    virtual getField( const std::string &fieldName ) {
        throw DbgException("no fields");
    }

    virtual std::string  print() {
        return "PtrTypedVar";
    }

    virtual ULONG64  getValue() const;

};

///////////////////////////////////////////////////////////////////////////////////

} // namespace pykd
