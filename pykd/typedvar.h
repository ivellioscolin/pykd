#pragma once

#include "typeinfo.h"
#include "intbase.h"
#include "dbgobj.h"
#include "dbgexcept.h"
#include "vardata.h"

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

class TypedVar;
typedef boost::shared_ptr<TypedVar>  TypedVarPtr;

///////////////////////////////////////////////////////////////////////////////////

class TypedVar : public intBase, protected DbgObject {

public:

    static TypedVarPtr getTypedVar( IDebugClient4 *client, const TypeInfoPtr& typeInfo, VarDataPtr varData );

    static TypedVarPtr getTypedVarByName( const std::string &varName );

    static TypedVarPtr getTypedVarByTypeName( const std::string &typeName, ULONG64 addr );

    static TypedVarPtr getTypedVarByTypeInfo( const TypeInfoPtr &typeInfo, ULONG64 addr );

    ULONG64 getAddress() const {
        return m_varData->getAddr();
    }

    ULONG getSize() const {
        return m_size;
    }

    ULONG getFieldOffsetByNameRecirsive(const std::string &fieldName) {
        return m_typeInfo->getFieldOffsetByNameRecirsive(fieldName);
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

    virtual python::object getElementByIndex( ULONG  index ) {
        throw PyException( PyExc_TypeError, "object is unsubscriptable");  
    }

    python::object getElementByIndexPtr( const TypedVarPtr& tv ) {
        return getElementByIndex( boost::apply_visitor( VariantToULong(), tv->getValue() ) );
    }

    virtual BaseTypeVariant getValue() {
        return m_varData->getAddr();
    }

    ULONG getDataKind() const {
        return m_dataKind;
    }

    void setDataKind(ULONG dataKind) {
        m_dataKind = dataKind;
    }

protected:

    TypedVar ( IDebugClient4 *client, const TypeInfoPtr& typeInfo, VarDataPtr varData );

    TypeInfoPtr             m_typeInfo;

    VarDataPtr              m_varData;

    ULONG                   m_size;

    ULONG                   m_dataKind;
};


///////////////////////////////////////////////////////////////////////////////////

class BasicTypedVar : public TypedVar {

public:

    BasicTypedVar ( IDebugClient4 *client, const TypeInfoPtr& typeInfo, VarDataPtr varData ) 
        : TypedVar(client, typeInfo, varData)
    {
    }


    virtual std::string  print();

    virtual std::string  printValue();

    virtual BaseTypeVariant  getValue();

};

///////////////////////////////////////////////////////////////////////////////////

class PtrTypedVar : public TypedVar {

public:

    PtrTypedVar ( IDebugClient4 *client, const TypeInfoPtr& typeInfo, VarDataPtr varData )
        : TypedVar(client, typeInfo, varData)
    {
    }

    virtual std::string print();

    virtual std::string  printValue();

    virtual TypedVarPtr deref();

    virtual BaseTypeVariant getValue();

};

///////////////////////////////////////////////////////////////////////////////////

class ArrayTypedVar: public TypedVar {

public:

    ArrayTypedVar ( IDebugClient4 *client, const TypeInfoPtr& typeInfo, VarDataPtr varData )
        : TypedVar(client, typeInfo, varData)
    {
    }

    virtual ULONG getElementCount() {
        return m_typeInfo->getCount();
    }

    virtual std::string print();

    virtual std::string  printValue();

    virtual python::object getElementByIndex( ULONG  index );
};

///////////////////////////////////////////////////////////////////////////////////

class UdtTypedVar : public TypedVar {

public:

    UdtTypedVar( IDebugClient4 *client, const TypeInfoPtr& typeInfo, VarDataPtr varData ) 
        : TypedVar(client, typeInfo, varData)
    {
    }

protected:

    virtual std::string print();

    virtual std::string  printValue();

    virtual TypedVarPtr getField( const std::string &fieldName );

    virtual ULONG getElementCount() {
        return m_typeInfo->getFieldCount();                
    }

    virtual python::object getElementByIndex( ULONG  index );

    LONG getVirtualBaseDisplacement( TypeInfoPtr& typeInfo );
};

///////////////////////////////////////////////////////////////////////////////////

class BitFieldVar: public TypedVar {

public:

    BitFieldVar( IDebugClient4 *client, const TypeInfoPtr& typeInfo, VarDataPtr varData ) 
        : TypedVar(client, typeInfo, varData)
    {
    }

    virtual std::string  printValue();

    virtual BaseTypeVariant  getValue();
};

///////////////////////////////////////////////////////////////////////////////////

class EnumTypedVar : public TypedVar {
public:

    EnumTypedVar( IDebugClient4 *client, const TypeInfoPtr& typeInfo, VarDataPtr varData ) 
        : TypedVar(client, typeInfo, varData)
    {
    }

    virtual std::string print();

    virtual std::string  printValue();

    virtual BaseTypeVariant  getValue();
};

///////////////////////////////////////////////////////////////////////////////////


} // namespace pykd
