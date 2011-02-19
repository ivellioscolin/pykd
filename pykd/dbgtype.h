#pragma once

#include <string>
#include <map>
#include <list>

#include <boost/python.hpp>
#include <boost/python/object.hpp>

#include "dbgmem.h"

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadTypedVar( const std::string &moduleName, const std::string &typeName, ULONG64 address );

boost::python::object
loadTypedVarList( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &listEntryName );

boost::python::object
loadTypedVarArray( ULONG64 address, const std::string &moduleName, const std::string &typeName, long number );

boost::python::object
containingRecord( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &fieldName );

boost::python::object
getTypeClass( const std::string &moduleName, const std::string &typeName );

ULONG
sizeofType( const std::string &moduleName, const std::string &typeName );


/////////////////////////////////////////////////////////////////////////////////

class TypeInfo {

public:

    template< typename TTypeInfo>
    struct TypeFieldT {
        ULONG           size;
        ULONG           offset;
        TTypeInfo       type;
        std::string     name;

        TypeFieldT( const std::string &name_, const TTypeInfo  &type_,  ULONG size_, ULONG offset_ ) :
            name( name_ ),
            size( size_ ),
            offset( offset_ ),
            type( type_ )               
            {}
    };
    
    struct TypeName {
        std::string   module;
        std::string   symbol;
        
        TypeName( const std::string &module_, const std::string  &symbol_ ) :
            module( module_ ),
            symbol( symbol_ )
            {}
            
        bool
        operator < ( const TypeName &typeName ) const {

              if ( typeName.module < module )
                    return true;

              if ( typeName.module > module )
                    return false;

              return typeName.symbol < symbol;
        }
  
    };
    
    typedef TypeFieldT<TypeInfo>                TypeField;
    
    typedef std::map<TypeName, TypeInfo>        TypeInfoMap;
    
    typedef std::list<TypeField>                TypeFieldList;

public:

    TypeInfo() :
        m_size( 0 ),
        m_baseType( false ),
        m_pointer( false ),
        m_offset(0)
        {}

    TypeInfo( const std::string  &moduleName, const std::string  &typeName );

    boost::python::object
    load( ULONG64 addr, ULONG offset = 0 ) const;

    boost::python::object
    build( ULONG offset = 0 ) const;


    ULONG64
    size() const 
    {
        return m_size;
    }

    const std::string&
    name() const 
    {
        return m_typeName;
    }

    static const TypeInfo&
    get( const std::string  &moduleName, const std::string  &typeName );  

    const TypeFieldList&
    getFields() const {
        return m_fields;
    }

    bool
    isComplex() const {
        return !m_baseType;
    }

    bool
    isPtr() const {
        return m_pointer;
    }

    // field offset getter/setter
    void setOffset(ULONG offset) { m_offset = offset; }
    ULONG getOffset() const { return m_offset; }

private:

    static TypeInfoMap                          g_typeInfoCache; 

    boost::python::object
    loadBaseType( ULONG64 addr ) const;

    boost::python::object
    ptrLoader( ULONG64 addr ) const  {
        return boost::python::object( loadPtrByPtr( addr ) );
    }

    void
    setupBaseType();

private:

    bool                                m_baseType;
    bool                                m_pointer;
    TypeFieldList                       m_fields;
    std::string                         m_typeName;
    ULONG                               m_size;
    ULONG                               m_offset;
};

/////////////////////////////////////////////////////////////////////////////////

class typeClass
{
public:

    typeClass()
    {
    }

    typeClass(
        const TypeInfo &typeInfo
    ) : m_typeInfo(typeInfo)
    {
    }

    // sizeof(TYPE)
    ULONG size() const
    {
        return (ULONG)m_typeInfo.size();
    }

    void setPyObj( const boost::python::object  &obj )
    {
        m_pyobj = obj;
    }

    // TypeInfo getter
    TypeInfo &getTypeInfo()
    {
        return m_typeInfo;
    }
    const TypeInfo &getTypeInfo() const
    {
        return m_typeInfo;
    }

    // boost::python::object getter
    boost::python::object &getPyObj()
    {
        return m_pyobj;
    }
    const boost::python::object &getPyObj() const
    {
        return m_pyobj;
    }

    std::string print() const;

    virtual void printField(
        const TypeInfo::TypeField &field,
        std::stringstream &sstr
    ) const 
    {
        // no data - nothing print
    }
    virtual void printSelf(
        std::stringstream &sstr
    ) const 
    {
        // no data - nothing print
    }

    ULONG getOffset() const { return m_typeInfo.getOffset(); }

private:
    TypeInfo m_typeInfo;
    boost::python::object m_pyobj;
};

/////////////////////////////////////////////////////////////////////////////////

class typedVarClass : public typeClass {

public:

    typedVarClass()
    {}
    
    typedVarClass( const TypeInfo  &typeInfo, ULONG64 addr)  :
        typeClass( typeInfo ),
        m_addr( addr )
        {}

    ULONG64
    getAddress() const {
        return m_addr;
    }

    virtual void 
    printField( const TypeInfo::TypeField &field, std::stringstream &sstr ) const override;

    virtual void
    printSelf( std::stringstream &sstr ) const override 
    {
        sstr << std::hex << "0x" << getAddress() << std::dec << "  ";
    }

private:

    ULONG64                     m_addr;
};

/////////////////////////////////////////////////////////////////////////////////