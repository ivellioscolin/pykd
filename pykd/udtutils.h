
#pragma once

///////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

#include "symengine.h"

///////////////////////////////////////////////////////////////////////////////////

namespace pykd {

class UdtField;
typedef boost::shared_ptr<UdtField>  UdtFieldPtr;

class TypeInfo;
typedef boost::shared_ptr<TypeInfo>  TypeInfoPtr;

///////////////////////////////////////////////////////////////////////////////////

class UdtField
{

public:

    const std::string& getName() const
    {
        return m_name;
    }

    const std::string& getVirtualBaseClassName() const 
    {
        return m_virtualBaseName;
    }

    ULONG getOffset() const
    {
        return m_offset;
    }

    void setOffset( ULONG offset ) 
    {
         m_offset = offset;
    }

    bool isVirtualMember() const 
    {
        return m_virtualMember;
    }

    bool isStaticMember() const
    {
        return m_staticMember;
    }

    ULONG64 getStaticOffset() const
    {
        return m_staticOffset;
    }

    void setStaticOffset( ULONG64 staticOffset)
    {
        m_staticMember = true;
        m_staticOffset = staticOffset;
    }

    void getVirtualDisplacement( ULONG &virtualBasePtr, ULONG &virtualDispIndex, ULONG &virtualDispSize )
    {
       virtualBasePtr = m_virtualBasePtr;
       virtualDispIndex = m_virtualDispIndex;
       virtualDispSize = m_virtualDispSize;
    }

    void setVirtualDisplacement( ULONG virtualBasePtr, ULONG virtualDispIndex, ULONG virtualDispSize )
    {
         m_virtualMember = true;
         m_virtualBasePtr = virtualBasePtr;
         m_virtualDispIndex = virtualDispIndex;
         m_virtualDispSize = virtualDispSize;
    }

    virtual TypeInfoPtr getTypeInfo() = 0;

protected:

    UdtField( const std::string  &name ) :
         m_name( name ),
         m_offset( 0 ),
         m_virtualMember( false ),
         m_staticMember( false ),
         m_virtualBasePtr( 0 ),
         m_virtualDispIndex( 0 ),
         m_virtualDispSize( 0 )
         {}

    std::string  m_name;

    std::string  m_virtualBaseName;

    ULONG  m_offset;

    ULONG64  m_staticOffset;

    bool  m_staticMember;

    bool  m_virtualMember;

    ULONG  m_virtualBasePtr;
    ULONG  m_virtualDispIndex;
    ULONG  m_virtualDispSize;

};

///////////////////////////////////////////////////////////////////////////////////

class SymbolUdtField : public UdtField 
{
public:

    SymbolUdtField( const SymbolPtr &sym, const std::string& name ) :
        UdtField( name ),
        m_symbol( sym )
        {}

    SymbolPtr& getSymbol() {
        return m_symbol;
    }

private:

    virtual TypeInfoPtr getTypeInfo();

    SymbolPtr  m_symbol;
};

///////////////////////////////////////////////////////////////////////////////////

class CustomUdtField : public UdtField
{
public:

    CustomUdtField( const TypeInfoPtr &typeInfo, const std::string& name ) :
        UdtField( name ),
        m_type( typeInfo )
        {}

private:

    virtual TypeInfoPtr getTypeInfo() {
        return m_type;
    }

    TypeInfoPtr  m_type;
};

///////////////////////////////////////////////////////////////////////////////////

class FieldCollection 
{
public:

    FieldCollection( const std::string &name ) :
      m_name( name )
      {}

    const UdtFieldPtr &lookup(ULONG index) const;
    const UdtFieldPtr &lookup(const std::string &name) const;

    UdtFieldPtr &lookup(ULONG index);
    UdtFieldPtr &lookup(const std::string &name);

    void push_back( const UdtFieldPtr& field ) {
        m_fields.push_back( field );
    }

    ULONG count() const {
        return static_cast<ULONG>( m_fields.size() );
    }

private:

    typedef std::vector<UdtFieldPtr>  FieldList;
    FieldList  m_fields;
    std::string  m_name;
};

///////////////////////////////////////////////////////////////////////////////////

}   // namespace pykd
