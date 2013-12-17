
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

    TypeInfoPtr createUnion( const std::string &name, ULONG align = 0);

private:

     ULONG  m_ptrSize;
};


////////////////////////////////////////////////////////////////////////////////

class CustomBase : public UdtTypeInfoBase 
{
public:

    virtual ULONG getSize();

protected:

    CustomBase( const std::string &name, ULONG pointerSize, ULONG align ) :
        UdtTypeInfoBase( name )
        {
            m_ptrSize = pointerSize;
            m_align = align ? align : m_ptrSize;
            m_size = 0;
        }

    static ULONG alignUp(ULONG val, ULONG align);

    void throwIfFiledExist(const std::string &fieldName);
    void throwIfTypeRecursive(TypeInfoPtr type);

    void checkAppendField(const std::string &fieldName, TypeInfoPtr &fieldType);

protected:

    ULONG  m_align;

    ULONG  m_size;
};

////////////////////////////////////////////////////////////////////////////////

class CustomStruct : public CustomBase 
{
public:

    CustomStruct( const std::string &name, ULONG ptrSize, ULONG align ) :
        CustomBase( name, ptrSize, align )
        {}

    virtual void appendField(const std::string &fieldName, TypeInfoPtr &fieldType);
};

////////////////////////////////////////////////////////////////////////////////

class CustomUnion : public CustomBase 
{
public:

    CustomUnion( const std::string &name, ULONG ptrSize, ULONG align ) :
        CustomBase( name, ptrSize, align )
        {}

    virtual  void appendField(const std::string &fieldName, TypeInfoPtr &fieldType);
};

//////////////////////////////////////////////////////////////////////////////////

}   // namespace pykd

////////////////////////////////////////////////////////////////////////////////
