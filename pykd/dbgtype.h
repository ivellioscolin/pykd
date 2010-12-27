#pragma once

#include <string>
#include <map>

#include <boost/python.hpp>
#include <boost/python/object.hpp>

#include "dbgmem.h"

/////////////////////////////////////////////////////////////////////////////////

class typedVarClass {

public:

    typedVarClass()
    {}
    
    typedVarClass( ULONG64 addr, ULONG size ) :
        m_addr( addr ), 
        m_size( size )
    {}
    
    ULONG64
    getAddress() const {
        return m_addr;
    }
    
    ULONG
    size() const {
        return m_size;
    }
    
private:

    ULONG64     m_addr;
    
    ULONG       m_size;
        
};

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadTypedVar( const std::string &moduleName, const std::string &typeName, ULONG64 address );

boost::python::object
loadTypedVarList( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &listEntryName );

boost::python::object
containingRecord( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &fieldName );

ULONG
sizeofType( const std::string &moduleName, const std::string &typeName );


/////////////////////////////////////////////////////////////////////////////////

class TypeInfo {

public:

    TypeInfo() :
        m_size( 0 ),
        m_baseType( false ),
        m_pointer( false )  
        {}
    
    TypeInfo( const std::string  &moduleName, const std::string  &typeName );
    
    boost::python::object
    load( ULONG64 addr ) const;
    
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
        
    
private:  

    template< typename TTypeInfo>
    struct TypeFieldT {
        ULONG         size;  
        ULONG         offset;
        TTypeInfo     type; 
          
        TypeFieldT( const TTypeInfo  &type_,  ULONG size_, ULONG offset_ ) :
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
            return ( typeName.module <= module ) && ( typeName.symbol < symbol );
        }            
    };
    
    
    
    typedef TypeFieldT<TypeInfo>                TypeField;
    
    typedef std::map<TypeName, TypeInfo>     TypeInfoMap;
    
    typedef std::map<std::string, TypeField>    TypeFieldMap;
       
    static TypeInfoMap                  g_typeInfoCache; 

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
    
    TypeFieldMap                        m_fields;

    std::string                         m_typeName;
    
    ULONG                               m_size;
};

/////////////////////////////////////////////////////////////////////////////////