#pragma once

#include "typeinfo.h"
#include "intbase.h"
#include "dbgobj.h"
#include "dbgexcept.h"

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

class TypedVar;
typedef boost::shared_ptr<TypedVar>  TypedVarPtr;

///////////////////////////////////////////////////////////////////////////////////

class TypedVar : public intBase, protected DbgObject {

public:

    static TypedVarPtr  getTypedVar( IDebugClient4 *client, const TypeInfoPtr& typeInfo, ULONG64 offset );

    ULONG64 getAddress() const {
        return m_offset;
    }

    ULONG getSize() const {
        return m_size;
    }

    ULONG getOffset() {
        return m_typeInfo->getOffset();
    }

    TypeInfoPtr
    getType() const {
        return m_typeInfo;
    }

    virtual TypedVarPtr deref() {
        throw TypeException( m_typeInfo->getName(), "object can not be derefernced" );
    }

    virtual TypedVarPtr getField( const std::string &fieldName ) {
        throw TypeException( m_typeInfo->getName(), "no fields");
    }

    virtual std::string  print() {
        return "";
    }

    virtual std::string  printValue() {
        return "";
    }

    virtual ULONG getElementCount() {
        throw PyException( PyExc_TypeError, "object has no len()" );
    }

    virtual TypedVarPtr getElementByIndex( ULONG  index ) {
        throw PyException( PyExc_TypeError, "object is unsubscriptable");  
    }

    virtual TypedVarPtr getElementByIndexPtr( const TypedVarPtr& tv ) {
        return getElementByIndex( boost::apply_visitor( VariantToULong(), tv->getValue() ) );
    }

    virtual BaseTypeVariant getValue() {
        return m_offset;
    }

    ULONG getDataKind() const {
        return m_dataKind;
    }

    void setDataKind(ULONG dataKind) {
        m_dataKind = dataKind;
    }

protected:

    TypedVar ( IDebugClient4 *client, const TypeInfoPtr& typeInfo, ULONG64 offset );

    TypeInfoPtr             m_typeInfo;

    ULONG64                 m_offset;

    ULONG                   m_size;

    ULONG                   m_dataKind;
};


///////////////////////////////////////////////////////////////////////////////////

class BasicTypedVar : public TypedVar {

public:

    BasicTypedVar ( IDebugClient4 *client, const TypeInfoPtr& typeInfo, ULONG64 offset ) : TypedVar(client, typeInfo, offset){}


    virtual std::string  print();

    virtual std::string  printValue();

    virtual BaseTypeVariant  getValue();

};

///////////////////////////////////////////////////////////////////////////////////

class PtrTypedVar : public TypedVar {

public:

    PtrTypedVar ( IDebugClient4 *client, const TypeInfoPtr& typeInfo, ULONG64 offset ) : TypedVar(client, typeInfo, offset){}

    virtual std::string print();

    virtual std::string  printValue();

    virtual TypedVarPtr deref();

    virtual BaseTypeVariant getValue();

};

///////////////////////////////////////////////////////////////////////////////////

class ArrayTypedVar: public TypedVar {

public:

    ArrayTypedVar ( IDebugClient4 *client, const TypeInfoPtr& typeInfo, ULONG64 offset ) : TypedVar(client, typeInfo, offset){}

    virtual ULONG getElementCount() {
        return m_typeInfo->getCount();
    }

    virtual std::string print();

    virtual std::string  printValue();

    virtual TypedVarPtr getElementByIndex( ULONG  index );
};

///////////////////////////////////////////////////////////////////////////////////

class UdtTypedVar : public TypedVar {

public:

    UdtTypedVar( IDebugClient4 *client, const TypeInfoPtr& typeInfo, ULONG64 offset ) : TypedVar(client, typeInfo, offset){}

    virtual std::string print();

    virtual std::string  printValue();

    virtual TypedVarPtr getField( const std::string &fieldName );
};

///////////////////////////////////////////////////////////////////////////////////

class BitFieldVar: public TypedVar {

public:

    BitFieldVar( IDebugClient4 *client, const TypeInfoPtr& typeInfo, ULONG64 offset ) : TypedVar(client, typeInfo, offset){}

    virtual std::string  printValue();

    virtual BaseTypeVariant  getValue();
};

///////////////////////////////////////////////////////////////////////////////////

class EnumTypedVar : public TypedVar {
public:

    EnumTypedVar( IDebugClient4 *client, const TypeInfoPtr& typeInfo, ULONG64 offset ) : TypedVar(client, typeInfo, offset){}

    virtual std::string print();

    virtual std::string  printValue();

    virtual BaseTypeVariant  getValue();
};

///////////////////////////////////////////////////////////////////////////////////


} // namespace pykd
