#pragma once

#include <string>

#include <boost\enable_shared_from_this.hpp>

#include "udtutils.h"
#include "variant.h"
#include "symengine.h"
#include "dbgexcept.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class TypeInfo;
typedef boost::shared_ptr<TypeInfo>  TypeInfoPtr;

///////////////////////////////////////////////////////////////////////////////////

class TypeInfo : boost::noncopyable, public intBase, public boost::enable_shared_from_this<TypeInfo> {

public:

    static
    TypeInfoPtr getTypeInfoByName( const std::string &symName );

    static
    ULONG64 getSymbolSize( const std::string &symName );

    static
    std::string findSymbol( ULONG64 offset, bool safe = true );

    static
    ULONG64 getOffset( const std::string &symbolName );

    static 
    TypeInfoPtr  getTypeInfo( SymbolPtr &symScope, const std::string &symName );

    static 
    TypeInfoPtr  getTypeInfo( SymbolPtr &symbol );

    static
    TypeInfoPtr  getBaseTypeInfo( const std::string &name );

    static
    TypeInfoPtr  getBaseTypeInfo( SymbolPtr &symbol ); 

    static
    bool isBaseType( const std::string &name );

public:

    TypeInfo() :
        m_staticOffset( 0 ),
        m_constant( false ),
        m_staticMember( false ),
        m_virtualMember( false ),
        m_virtualBasePtr( 0 ),
        m_virtualDispIndex( 0 ),
        m_virtualDispSize( 0 ),
        m_ptrSize( 0 )
        {}

    virtual std::string print() {
        std::stringstream   sstr;
        sstr << "Type name: " << getName();
        sstr << "  Size: 0x" << std::hex << getSize() << " (" << std::dec << getSize() << ")";
        return sstr.str();
    }

    virtual std::string getName() = 0;

    virtual ULONG getSize() = 0;

    virtual TypeInfoPtr getField( const std::string &fieldName ) {
        throw TypeException( getName(), "type is not a struct" ); 
    }

    virtual TypeInfoPtr getFieldByIndex( ULONG index ) {
        throw TypeException( getName(), "type is not a struct" ); 
    }

    virtual std::string getFieldNameByIndex( ULONG index ) {
        throw TypeException( getName(), "type is not a struct" ); 
    }

    virtual ULONG getFieldOffsetByNameRecirsive( const std::string &fieldName ) {
        throw TypeException( getName(), "type is not a struct" ); 
    }
    virtual ULONG getFieldOffsetByNameNotRecursively( const std::string &fieldName ) {
        throw TypeException( getName(), "type is not a struct" ); 
    }

    virtual ULONG getFieldOffsetByIndex( ULONG index ) {
        throw TypeException( getName(), "type is not a struct" ); 
    }

    virtual ULONG getFieldCount() {
        throw TypeException( getName(), "type is not a struct" ); 
    }

    virtual BaseTypeVariant  getValue();

    virtual bool isBasicType() {
        return false;
    }

    virtual bool isPointer() {
        return false;
    }

    virtual bool isArray() {
        return false;
    }

    virtual bool isUserDefined() {
        return false;
    }

    virtual bool isBitField() {
        return false;
    }

    virtual bool isEnum() {
        return false;
    }

    virtual ULONG getCount() {
        throw TypeException( getName(), "type is not an array" );
    }

    virtual TypeInfoPtr getElementType() {
        throw TypeException( getName(), "type is not an array" );   
    }

    virtual ULONG getBitOffset() {
        return 0;
    }

    virtual ULONG getBitWidth() {
        return 8 * getSize();
    }

    virtual python::dict asMap() {
        throw TypeException( getName(), "type cannot be converted to a dict" );
    }

    virtual TypeInfoPtr deref() {
        throw TypeException( getName(), "type is not a pointer" );
    }

    virtual ULONG getElementCount() {
        throw PyException( PyExc_TypeError, "object has no len()" );
    }

    virtual python::tuple getElementByIndex( ULONG index ) {
        throw PyException( PyExc_TypeError, "object is unsubscriptable");  
    }

    virtual void appendField(const std::string &fieldName, TypeInfoPtr fieldType) {
        throw TypeException( getName(), "type is not is not extensible" );
    }

    TypeInfoPtr ptrTo();

    virtual ULONG getAlignReq() {
        return 1;
    }

    void setConstant( const BaseTypeVariant& var )
    {
        m_constant = true;
        m_constantValue = var;
    }
    bool isConstant() const
    {
       return  m_constant == true;
    }

    bool isStaticMember() const
    {
       return  m_staticMember == true;
    }

    bool isVirtualMember() const 
    {
        return m_virtualMember == true;
    }

    void setStaticOffset( ULONG64 offset ) {
        m_staticOffset = offset; 
        m_staticMember = true;
    }

    void setVirtualBase( TypeInfoPtr virtualBase, LONG virtualBasePtr, ULONG virtualDispIndex, ULONG virtualDispSize )
    {
        m_virtualMember = true;
        m_virtualBaseType = virtualBase;
        m_virtualBasePtr = virtualBasePtr;
        m_virtualDispIndex = virtualDispIndex;
        m_virtualDispSize = virtualDispSize;
    }

    ULONG64 getStaticOffset();

    void getVirtualDisplacement( ULONG &virtualBasePtr, ULONG &virtualDispIndex, ULONG &virtualDispSize ) {
        virtualBasePtr = m_virtualBasePtr;
        virtualDispIndex = m_virtualDispIndex;
        virtualDispSize = m_virtualDispSize;
    };

    TypeInfoPtr getVirtualBase() {
        return m_virtualBaseType;
    }

    bool is(TypeInfo *rhs) const {
        return this == rhs;
    }

    ULONG ptrSize() const {
        return m_ptrSize;
    }

protected:

    std::string getComplexName();

    static
    TypeInfoPtr getComplexType( SymbolPtr &symScope, const std::string &symName );

    static
    TypeInfoPtr getRecurciveComplexType( TypeInfoPtr &lowestType, std::string &suffix, ULONG ptrSize );

    ULONG64     m_staticOffset;

    bool        m_constant;

    bool        m_staticMember;

    bool        m_virtualMember;

    BaseTypeVariant  m_constantValue;

    LONG        m_virtualBasePtr;

    ULONG       m_virtualDispIndex;

    ULONG       m_virtualDispSize;

    TypeInfoPtr m_virtualBaseType;

    ULONG       m_ptrSize;
};

///////////////////////////////////////////////////////////////////////////////////

template<typename T>
class TypeInfoWrapper : public TypeInfo
{
public:    
    TypeInfoWrapper( const std::string &name ) :
      m_name(name)
      {}

private:

    virtual std::string getName() {
        return m_name;
    }

    virtual ULONG getSize() {
        return sizeof(T);
    }

    virtual bool isBasicType() {
        return true;
    }

    std::string     m_name;

};

///////////////////////////////////////////////////////////////////////////////////

class BitFieldTypeInfo : public TypeInfo
{
public:

    BitFieldTypeInfo( SymbolPtr &symbol );

    virtual std::string getName() {
        return m_name;
    }

    virtual ULONG getSize() {
        return m_size;
    }

    virtual bool isBitField() {
        return true;
    }

    virtual ULONG getBitOffset() {
        return m_bitPos;
    }

    virtual ULONG getBitWidth() {
        return m_bitWidth;
    }

    virtual ULONG getAlignReq() override {
        return m_size;
    }

private:

    ULONG           m_size;
    ULONG           m_bitWidth;
    ULONG           m_bitPos;
    std::string     m_name;
};

///////////////////////////////////////////////////////////////////////////////////

class UdtFieldColl : public TypeInfo
{
protected:
    virtual std::string getName() {
        return m_fields.getName();
    }

    virtual TypeInfoPtr getField( const std::string &fieldName ) {
        return lookupField(fieldName).m_type;
    }

    virtual TypeInfoPtr getFieldByIndex( ULONG index ) {
        return lookupField(index).m_type;
    }

    virtual std::string getFieldNameByIndex( ULONG index ) {
        return lookupField(index).m_name;
    }

    virtual ULONG getFieldOffsetByNameRecirsive( const std::string &fieldName ) {
        return UdtUtils::getFieldOffsetRecirsive( shared_from_this(), fieldName );
    }

    virtual ULONG getFieldOffsetByNameNotRecursively( const std::string &fieldName ) {
        return lookupField(fieldName).m_offset;
    }

    virtual ULONG getFieldOffsetByIndex( ULONG index ) {
        return lookupField(index).m_offset;
    }

    virtual ULONG getFieldCount() {
        refreshFields();
        return (ULONG)m_fields.size();
    }

    virtual ULONG getElementCount() {
        return getFieldCount();
    }

    virtual python::tuple getElementByIndex( ULONG index ) {
        return python::make_tuple( getFieldNameByIndex(index), getFieldByIndex(index) );
    }

    virtual std::string print();

    virtual bool isUserDefined() {
        return true;
    }

    virtual ULONG getAlignReq() override;

protected:
    UdtFieldColl(const std::string &typeName) : m_fields(typeName) {}

    virtual void refreshFields() {}
    virtual std::string getTypeString() const = 0;

    bool empty() const {
        return m_fields.empty();
    }

    void push_back(const UdtUtils::Field &field) {
        m_fields.push_back(field);
    }

    template <typename T>
    const UdtUtils::Field &lookupField( T index) {
        refreshFields();
        return m_fields.lookup(index);
    }

    UdtUtils::Field &last(){
        return *m_fields.rbegin();
    }

private:
    UdtUtils::FieldCollection m_fields;
};

///////////////////////////////////////////////////////////////////////////////////

class UdtTypeInfo : public UdtFieldColl
{
public:

    UdtTypeInfo (SymbolPtr &symbol ) :
        UdtFieldColl( symbol->getName() ),
        m_dia( symbol )
    {
    }

protected:
    virtual ULONG getSize() {
        return (ULONG)m_dia->getSize();
    }
    void getVirtualFields();

    virtual void refreshFields() override;

    virtual std::string getTypeString() const override {
        return "struct/class";
    }

    void getFields( 
        SymbolPtr &rootSym, 
        SymbolPtr &baseVirtualSym,
        ULONG startOffset = 0,
        LONG virtualBasePtr = 0,
        ULONG virtualDispIndex = 0,
        ULONG m_virtualDispSize = 0 );


private:
    SymbolPtr m_dia;
};

///////////////////////////////////////////////////////////////////////////////////

class EnumTypeInfo : public TypeInfo
{
public:

    EnumTypeInfo ( SymbolPtr &symbol ) :
      m_dia( symbol )
      {}

protected:

    virtual std::string getName() {
        return m_dia->getName();
    }

    virtual ULONG getSize() {
        return (ULONG)m_dia->getSize();
    }

    virtual TypeInfoPtr getFieldByIndex( ULONG index );

    virtual std::string getFieldNameByIndex( ULONG index );

    virtual ULONG getFieldCount();

    virtual TypeInfoPtr getField( const std::string &fieldName );

    virtual python::dict asMap();

    virtual bool isEnum() {
        return true;
    }

    virtual std::string print();

    virtual ULONG getAlignReq() override {
        return getSize();
    }

    SymbolPtr    m_dia;
};

///////////////////////////////////////////////////////////////////////////////////

class PointerTypeInfo : public TypeInfo {

public:

    PointerTypeInfo( const TypeInfoPtr  ptr, ULONG ptrSize ) :
        m_size( ptrSize ),
        m_derefType( ptr )
        {}        

    PointerTypeInfo( SymbolPtr &symbol  );

    PointerTypeInfo( SymbolPtr &symScope, const std::string &symName );

    // void *
    PointerTypeInfo( ULONG size );

    virtual std::string getName();

    virtual ULONG getSize();

    virtual bool isPointer() {
        return true;
    }

    virtual TypeInfoPtr deref() {
        return getDerefType();
    }

    virtual ULONG getAlignReq() override {
        return m_size;
    }

    TypeInfoPtr getDerefType() {
        if (!m_derefType)
            throw TypeException("<ptr>", "this pointer can not be dereferenced");
        return m_derefType;
    }

    bool derefPossible() const {
        return m_derefType;
    }

    const std::string getDerefName() const {
        if (m_derefName.empty())
            throw TypeException("<ptr>", "this pointer can not be dereferenced");
        return m_derefName;
    }

private:
    static std::string VoidTypeName;

    TypeInfoPtr     m_derefType;
    ULONG           m_size;
    std::string     m_derefName;
};

///////////////////////////////////////////////////////////////////////////////////

class ArrayTypeInfo : public TypeInfo {

public:

    ArrayTypeInfo( const TypeInfoPtr  ptr, ULONG count ) :
        m_derefType( ptr ),
        m_count( count )
        {}

    ArrayTypeInfo( SymbolPtr &symbol  );

    ArrayTypeInfo( SymbolPtr &symScope, const std::string &symName, ULONG count );

    virtual std::string getName();

    virtual ULONG getSize();

    virtual bool isArray() {
        return true;
    }

    virtual ULONG getCount() {
        return m_count;
    }

    virtual TypeInfoPtr getElementType() {
        return m_derefType;
    }

    virtual ULONG getAlignReq() override {
        return m_derefType->getAlignReq();
    }

    TypeInfoPtr getDerefType() {
        return m_derefType;
    }

private:

    TypeInfoPtr     m_derefType;

    ULONG           m_count;
};

///////////////////////////////////////////////////////////////////////////////////

void splitSymName( const std::string &fullName, std::string &moduleName, std::string &symbolName );

python::tuple getSourceLine( ULONG64 offset = 0 );

std::string getSourceFile( ULONG64 offset = 0 );

///////////////////////////////////////////////////////////////////////////////////

}; // namespace pykd
