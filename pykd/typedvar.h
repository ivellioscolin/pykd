#pragma once

#include "typeinfo.h"
#include "variant.h"
#include "dbgexcept.h"
#include "vardata.h"

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

class TypedVar;
typedef boost::shared_ptr<TypedVar>  TypedVarPtr;

///////////////////////////////////////////////////////////////////////////////////

class TypedVar : public intBase {

public:

    static TypedVarPtr getTypedVar( const TypeInfoPtr& typeInfo, VarDataPtr varData );

    static TypedVarPtr getTypedVarByName( const std::string &varName );

    static TypedVarPtr getTypedVarByTypeName( const std::string &typeName, ULONG64 addr );

    static TypedVarPtr getTypedVarByTypeInfo( const TypeInfoPtr &typeInfo, ULONG64 addr );

    ULONG64 getAddress() const {
        return m_varData->getAddr();
    }

    ULONG getSize() const {
        return m_size;
    }

    ULONG getFieldOffsetByNameRecursive(const std::string &fieldName) {
        return m_typeInfo->getFieldOffsetByNameRecursive(fieldName);
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

    TypedVar ( const TypeInfoPtr& typeInfo, VarDataPtr varData );

    virtual ~TypedVar()
    {}

    TypeInfoPtr             m_typeInfo;

    VarDataPtr              m_varData;

    ULONG                   m_size;

    ULONG                   m_dataKind;
};


///////////////////////////////////////////////////////////////////////////////////

class BasicTypedVar : public TypedVar {

public:

    BasicTypedVar ( const TypeInfoPtr& typeInfo, VarDataPtr varData ) : TypedVar(typeInfo, varData)
    {}

    virtual std::string  print();

    virtual std::string  printValue();

    virtual BaseTypeVariant  getValue();

};

///////////////////////////////////////////////////////////////////////////////////

class PtrTypedVar : public TypedVar {

public:

    PtrTypedVar ( const TypeInfoPtr& typeInfo, VarDataPtr varData ) : TypedVar(typeInfo, varData)
    {}

    virtual std::string print();

    virtual std::string  printValue();

    virtual TypedVarPtr deref();

    virtual BaseTypeVariant getValue();

    virtual TypedVarPtr getField( const std::string &fieldName );

};

///////////////////////////////////////////////////////////////////////////////////

class ArrayTypedVar: public TypedVar {

public:

    ArrayTypedVar ( const TypeInfoPtr& typeInfo, VarDataPtr varData ) : TypedVar(typeInfo, varData)
    {}

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

    UdtTypedVar( const TypeInfoPtr& typeInfo, VarDataPtr varData ) : TypedVar(typeInfo, varData)
    {}

protected:

    virtual std::string print();

    virtual std::string  printValue();

    virtual TypedVarPtr getField( const std::string &fieldName );

    virtual ULONG getElementCount() {
        return m_typeInfo->getFieldCount();
    }

    virtual python::object getElementByIndex( ULONG  index );

    LONG getVirtualBaseDisplacement( const std::string &fieldName );
    LONG getVirtualBaseDisplacementByIndex( ULONG index );
};

///////////////////////////////////////////////////////////////////////////////////

class BitFieldVar: public TypedVar {

public:

    BitFieldVar( const TypeInfoPtr& typeInfo, VarDataPtr varData ) : TypedVar( typeInfo, varData)
    {}

    virtual std::string  printValue();

    virtual BaseTypeVariant  getValue();
};

///////////////////////////////////////////////////////////////////////////////////

class EnumTypedVar : public TypedVar {
public:

    EnumTypedVar( const TypeInfoPtr& typeInfo, VarDataPtr varData ) : TypedVar( typeInfo, varData)
    {}

    virtual std::string print();

    virtual std::string  printValue();

    virtual BaseTypeVariant  getValue();
};

///////////////////////////////////////////////////////////////////////////////////

TypedVarPtr containingRecordByName( ULONG64 addr, const std::string &typeName, const std::string &fieldName );

TypedVarPtr containingRecordByType( ULONG64 addr, const TypeInfoPtr &typeInfo, const std::string &fieldName );

python::list getTypedVarListByTypeName( ULONG64 listHeadAddres, const std::string  &typeName, const std::string &listEntryName );

python::list getTypedVarListByType( ULONG64 listHeadAddres, const TypeInfoPtr &typeInfo, const std::string &listEntryName );

python::list getTypedVarArrayByTypeName( ULONG64 addr, const std::string  &typeName, ULONG number );

python::list getTypedVarArrayByType( ULONG64 addr, const TypeInfoPtr &typeInfo, ULONG number );

///////////////////////////////////////////////////////////////////////////////////

} // namespace pykd
