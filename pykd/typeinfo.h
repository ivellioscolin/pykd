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
        m_constant( false ),
        m_ptrSize( 0 )
        {}

    virtual ~TypeInfo()
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

    virtual ULONG getFieldOffsetByNameRecursive( const std::string &fieldName ) {
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

    virtual bool isStaticMember( const std::string& fieldName )
    {
        throw TypeException( getName(), "type is not a struct" ); 
    }

    virtual bool isStaticMemberByIndex( ULONG index )
    {
        throw TypeException( getName(), "type is not a struct" ); 
    }

    virtual ULONG64 getStaticOffsetByName( const std::string& fieldName ) 
    {
        throw TypeException( getName(), "type is not a struct" ); 
    }

    virtual ULONG64 getStaticOffsetByIndex( ULONG index ) 
    {
        throw TypeException( getName(), "type is not a struct" ); 
    }

    virtual bool isVirtualMember( const std::string& fieldName )
    {
        throw TypeException( getName(), "type is not a struct" ); 
    }

    virtual bool isVirtualMemberByIndex( ULONG index ) 
    {
        throw TypeException( getName(), "type is not a struct" ); 
    }

    virtual void getVirtualDisplacement( const std::string& fieldName, ULONG &virtualBasePtr, ULONG &virtualDispIndex, ULONG &virtualDispSize )
    {
        throw TypeException( getName(), "type is not a struct" ); 
    }

    virtual void getVirtualDisplacementByIndex( ULONG index, ULONG &virtualBasePtr, ULONG &virtualDispIndex, ULONG &virtualDispSize )
    {
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

    virtual void appendField(const std::string &fieldName, TypeInfoPtr &fieldType) {
        throw TypeException( getName(), "type is not is not editable" );
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

    bool        m_constant;

    BaseTypeVariant  m_constantValue;

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

class UdtTypeInfoBase : public TypeInfo
{
protected:
    virtual std::string getName() {
       return m_name;
    }

    virtual TypeInfoPtr getField( const std::string &fieldName ) {
        return lookupField(fieldName)->getTypeInfo();
    }

    virtual TypeInfoPtr getFieldByIndex( ULONG index ) {
        return lookupField(index)->getTypeInfo();
    }

    virtual std::string getFieldNameByIndex( ULONG index ) {
        return lookupField(index)->getName();
    }

    virtual ULONG getFieldOffsetByNameRecursive( const std::string &fieldName ) {
        return  getFieldOffsetRecursive( shared_from_this(), fieldName );
    }

    virtual ULONG getFieldOffsetByNameNotRecursively( const std::string &fieldName ) {
        return lookupField(fieldName)->getOffset();
    }

    virtual ULONG getFieldOffsetByIndex( ULONG index ) {
        return lookupField(index)->getOffset();
    }

    virtual bool isStaticMember( const std::string& fieldName ) {
        return lookupField(fieldName)->isStaticMember();
    }

    virtual bool isStaticMemberByIndex( ULONG index ) {
        return lookupField(index)->isStaticMember();
    }

    virtual ULONG64 getStaticOffsetByName( const std::string& fieldName ) {
        return lookupField(fieldName)->getStaticOffset();
    }

    virtual ULONG64 getStaticOffsetByIndex( ULONG index ) {
        return lookupField(index)->getStaticOffset();
    }

    virtual ULONG getFieldCount() {
        if ( !m_fieldsGot )
        {
            refreshFields();
            m_fieldsGot = true;
        }
        return m_fields.count();
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

    virtual bool isVirtualMember( const std::string& fieldName )
    {
        return lookupField(fieldName)->isVirtualMember();
    }

    virtual bool isVirtualMemberByIndex( ULONG index ) 
    { 
        return lookupField(index)->isVirtualMember();
    }

    virtual void getVirtualDisplacement( const std::string& fieldName, ULONG &virtualBasePtr, ULONG &virtualDispIndex, ULONG &virtualDispSize );
    virtual void getVirtualDisplacementByIndex( ULONG index, ULONG &virtualBasePtr, ULONG &virtualDispIndex, ULONG &virtualDispSize );

protected:

    UdtTypeInfoBase(const std::string &typeName) : 
         m_name(typeName),
         m_fieldsGot( false )
         {}

    virtual void refreshFields() {}

    void push_back(const UdtFieldPtr &field) {
        m_fields.push_back(field);
    }

    template <typename T>
    const UdtFieldPtr &lookupField( T index)const {
        if ( !m_fieldsGot )
        {
            refreshFields();
            m_fieldsGot = true;
        }
        return m_fields.lookup(index);
    }

    template <typename T>
    UdtFieldPtr &lookupField( T index) {
        if ( !m_fieldsGot )
        {
            refreshFields();
            m_fieldsGot = true;
        }
        return m_fields.lookup(index);
    }

protected:

    bool m_fieldsGot;

    FieldCollection  m_fields;

    std::string  m_name;

};

///////////////////////////////////////////////////////////////////////////////////

class UdtTypeInfo : public UdtTypeInfoBase
{
public:

    UdtTypeInfo (SymbolPtr &symbol ) :
        UdtTypeInfoBase( symbol->getName() ),
        m_dia( symbol )
    {
    }

protected:
    virtual ULONG getSize() {
        return (ULONG)m_dia->getSize();
    }
    void getVirtualFields();

    virtual void refreshFields() override;

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
