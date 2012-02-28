#pragma once

#include <string>

#include "diawrapper.h"
#include "intbase.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class TypeInfo;
typedef boost::shared_ptr<TypeInfo>  TypeInfoPtr;

///////////////////////////////////////////////////////////////////////////////////

class TypeInfo : boost::noncopyable, public intBase {

public:

    static 
    TypeInfoPtr  getTypeInfo( pyDia::SymbolPtr &symScope, const std::string &symName );

    static 
    TypeInfoPtr  getTypeInfo( pyDia::SymbolPtr &symScope, pyDia::SymbolPtr &symChild );

    static 
    TypeInfoPtr  getTypeInfo( pyDia::SymbolPtr &symbol );

    static
    TypeInfoPtr  getBaseTypeInfo( const std::string &name );

    static
    TypeInfoPtr  getBaseTypeInfo( pyDia::SymbolPtr &symbol ); 

public:

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

    ULONG getOffset() {
        return m_offset;
    }

    void setOffset( ULONG offset ) {
        m_offset = offset;
    }

    void setConstant( const VARIANT& var )
    {
        m_constant = true;
        m_constantValue = var;
    }

protected:

    std::string getComplexName();

    static
    TypeInfoPtr getComplexType( pyDia::SymbolPtr &symScope, const std::string &symName );

    static
    TypeInfoPtr getRecurciveComplexType( TypeInfoPtr &lowestType, std::string &suffix, ULONG ptrSize );

    ULONG       m_offset;

    bool        m_constant;

    VARIANT     m_constantValue;
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

    BitFieldTypeInfo(  pyDia::SymbolPtr &symbol );

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

private:

    ULONG           m_size;
    ULONG           m_bitWidth;
    ULONG           m_bitPos;
    std::string     m_name;
};

///////////////////////////////////////////////////////////////////////////////////

class UdtTypeInfo : public TypeInfo 
{
public:

    UdtTypeInfo ( pyDia::SymbolPtr &symbol ) :
      m_dia( symbol )
      {}

protected:

    virtual std::string getName() {
        return m_dia->getName();
    }

    virtual ULONG getSize() {
        return (ULONG)m_dia->getSize();
    }

    virtual TypeInfoPtr getField( const std::string &fieldName );

    virtual TypeInfoPtr getFieldByIndex( ULONG index );

    virtual std::string getFieldNameByIndex( ULONG index );

    virtual ULONG getFieldCount();

    virtual bool isUserDefined() {
        return true;
    }

    static bool getBaseField(
        pyDia::SymbolPtr symUdt,
        const std::string &fieldName,
        LONG &addOffset,
        pyDia::SymbolPtr &symField,
        ULONG currLevel = 0
    );

    pyDia::SymbolPtr    m_dia;
};

///////////////////////////////////////////////////////////////////////////////////

class EnumTypeInfo : public TypeInfo
{
public:

    EnumTypeInfo ( pyDia::SymbolPtr &symbol ) :
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

    pyDia::SymbolPtr    m_dia;
};

///////////////////////////////////////////////////////////////////////////////////

class PointerTypeInfo : public TypeInfo {

public:

    PointerTypeInfo( const TypeInfoPtr  ptr, ULONG ptrSize ) :
        m_size( ptrSize ),
        m_derefType( ptr )
        {}        

    PointerTypeInfo( pyDia::SymbolPtr &symbol  );

    PointerTypeInfo( pyDia::SymbolPtr &symScope, const std::string &symName );

    virtual std::string getName();

    virtual ULONG getSize();

    virtual bool isPointer() {
        return true;
    }

    virtual TypeInfoPtr deref() {
        return getDerefType();
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

    ArrayTypeInfo( pyDia::SymbolPtr &symbol  );

    ArrayTypeInfo( pyDia::SymbolPtr &symScope, const std::string &symName, ULONG count );

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

    TypeInfoPtr getDerefType() {
        return m_derefType;
    }

private:

    TypeInfoPtr     m_derefType;

    ULONG           m_count;
};

///////////////////////////////////////////////////////////////////////////////////

}; // namespace pykd
