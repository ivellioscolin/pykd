
#pragma once

////////////////////////////////////////////////////////////////////////////////

#include "typeinfo.h"
#include "dbgengine.h"
#include <string>

////////////////////////////////////////////////////////////////////////////////

namespace pykd {


class TypeBuilder {

public:

    TypeBuilder( ULONG pointerSize = 0 ) 
    {
        m_ptrSize = pointerSize ? pointerSize : ptrSize();
    }

    TypeInfoPtr getUInt1B() { return TypeInfo::getBaseTypeInfo( "UInt1B", m_ptrSize ); }
    TypeInfoPtr getUInt2B() { return TypeInfo::getBaseTypeInfo( "UInt2B", m_ptrSize ); }
    TypeInfoPtr getUInt4B() { return TypeInfo::getBaseTypeInfo( "UInt4B", m_ptrSize ); }
    TypeInfoPtr getUInt8B() { return TypeInfo::getBaseTypeInfo( "UInt8B", m_ptrSize ); }
    TypeInfoPtr getInt1B() { return TypeInfo::getBaseTypeInfo( "Int1B", m_ptrSize ); }
    TypeInfoPtr getInt2B() { return TypeInfo::getBaseTypeInfo( "Int2B", m_ptrSize ); }
    TypeInfoPtr getInt4B() { return TypeInfo::getBaseTypeInfo( "Int4B", m_ptrSize ); }
    TypeInfoPtr getInt8B() { return TypeInfo::getBaseTypeInfo( "Int8B", m_ptrSize ); }
    TypeInfoPtr getLong() { return TypeInfo::getBaseTypeInfo( "Long", m_ptrSize ); }
    TypeInfoPtr getULong() { return TypeInfo::getBaseTypeInfo( "ULong", m_ptrSize ); }
    TypeInfoPtr getBool() { return TypeInfo::getBaseTypeInfo( "Bool", m_ptrSize ); }
    TypeInfoPtr getChar() { return TypeInfo::getBaseTypeInfo( "Char", m_ptrSize ); }
    TypeInfoPtr getWChar() { return TypeInfo::getBaseTypeInfo( "WChar", m_ptrSize ); }

    TypeInfoPtr getVoidPtr() { 
        return TypeInfoPtr( new PointerTypeInfo(m_ptrSize) );
    }

    TypeInfoPtr createStruct( const std::string &name, ULONG align = 0 );

    TypeInfoPtr createUnion( const std::string &name);

private:

     ULONG  m_ptrSize;
};


////////////////////////////////////////////////////////////////////////////////

class CustomBase : public UdtTypeInfoBase 
{

protected:
    
    CustomBase( const std::string &name, ULONG pointerSize, ULONG align ) :
        UdtTypeInfoBase( name ),
        m_size( 0 ),
        m_ptrSize( pointerSize ),
        m_align( align )
        {}

    void throwIfFiledExist(const std::string &fieldName);
    void throwIfTypeRecursive(TypeInfoPtr type);

protected:

    virtual ULONG getSize() {
        return m_size;
    }

protected:

    ULONG  m_ptrSize;

    ULONG  m_align;

    std::string  m_name;

    ULONG  m_size;
};

////////////////////////////////////////////////////////////////////////////////

class CustomStruct : public CustomBase 
{
public:
    
    CustomStruct( const std::string &name, ULONG ptrSize, ULONG align ) :
        CustomBase( name, ptrSize, align )
        {}

private:

    virtual void appendField(const std::string &fieldName, TypeInfoPtr &fieldType );

};

////////////////////////////////////////////////////////////////////////////////

class CustomUnion : public CustomBase 
{
public:
    
    CustomUnion( const std::string &name, ULONG ptrSize, ULONG align ) :
        CustomBase( name, ptrSize, align )
        {}

private:

    virtual  void appendField(const std::string &fieldName, TypeInfoPtr &fieldType );

};

////////////////////////////////////////////////////////////////////////////////




















//
//////////////////////////////////////////////////////////////////////////////////
//
//class CustomTypeBase : public UdtFieldColl
//{
//    typedef UdtFieldColl Base;
//protected:
//    CustomTypeBase(const std::string &name, ULONG pointerSize);
//
//    void throwIfFiledExist(const std::string &fieldName);
//    void throwIfTypeRecursive(TypeInfoPtr type);
//
//private:
//    void throwIfTypeRecursiveImpl(TypeInfoPtr type);
//};
//
//////////////////////////////////////////////////////////////////////////////////
//
//class CustomStruct : public CustomTypeBase
//{
//    typedef CustomTypeBase Base;
//public:
//    static TypeInfoPtr create(const std::string &name, ULONG align = 0, ULONG pointerSize = 0);
//
//protected:
//    CustomStruct(const std::string &name, ULONG align, ULONG pointerSize)
//        : Base(name, pointerSize), m_name(name), m_align(align) 
//    {
//    }
//
//    virtual std::string getName() override {
//        return m_name;
//    }
//
//    virtual ULONG getSize() override;
//
//    virtual void appendField(const std::string &fieldName, TypeInfoPtr fieldType) override;
//
//    virtual std::string getTypeString() const override {
//        return "custom struct";
//    }
//
//private:
//    std::string m_name;
//    ULONG m_align;
//};
//
//////////////////////////////////////////////////////////////////////////////////
//
//class CustomUnion : public CustomTypeBase
//{
//    typedef CustomTypeBase Base;
//public:
//    static TypeInfoPtr create(const std::string &name, ULONG pointerSize = 0);
//
//protected:
//    CustomUnion(const std::string &name, ULONG pointerSize) 
//        : Base(name, pointerSize)
//    {
//    }
//
//    virtual ULONG getSize() override;
//
//    virtual void appendField(const std::string &fieldName, TypeInfoPtr fieldType) override;
//
//    virtual std::string getTypeString() const override {
//        return "custom union";
//    }
//
//private:
//    std::string m_name;
//};
//
//////////////////////////////////////////////////////////////////////////////////
//
//TypeInfoPtr PtrToVoid();
//
//////////////////////////////////////////////////////////////////////////////////

}   // namespace pykd

////////////////////////////////////////////////////////////////////////////////
