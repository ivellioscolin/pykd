#pragma once

#include <string>
#include <map>

#include "dbgmem.h"
#include "dbgsystem.h"

#include <boost/python/slice.hpp>

///////////////////////////////////////////////////////////////////////////////////

class TypedVar;
class TypeInfo;

///////////////////////////////////////////////////////////////////////////////////

class TypeInfo {

public:

     TypeInfo() :
        m_size(0),
        m_arraySize( 0 ),
        m_parentOffset( 0 ), 
        m_align(  ptrSize() ),
        m_isFreezed( false ),
        m_isBaseType( false ),
        m_isPointer( false )
        {}

     TypeInfo( const std::string customName, ULONG align=0 ) :
        m_typeName( customName ),
        m_size( 0 ),
        m_arraySize( 0 ),
        m_parentOffset( 0 ),
        m_isFreezed( false ),
        m_align( align == 0 ? ptrSize() : align ),
        m_isBaseType( false ),
        m_isPointer( false )
        {}
     
     TypeInfo( const std::string &moduleName, const std::string  &typeName );
     
     TypeInfo( const std::string &moduleName, ULONG64 moduleBase, ULONG typeId );
     
     static
     const TypeInfo&
     get( const std::string &moduleName, const std::string  &typeName );
     
     ULONG
     size() const {
        return m_size;
     }
     
     ULONG
     count() const {
        assert( m_size != 0 && m_arraySize >= m_size );
        return m_arraySize / m_size;
     }
     
     ULONG
     fullSize() const {
        return m_arraySize;
     }
     
     const std::string
     name() const {
        return m_typeName;
     }
     
     const std::string
     moduleName() const {
        return m_moduleName;
     }
     
     boost::python::object
     load( void* buffer, size_t  bufferLength ) const;
     
     std::string
     printField( size_t index, void* buffer, size_t  bufferLength ) const;
     
     std::string
     print() const;
     
     TypeInfo
     getField( const std::string  &fieldName ) const;
     
     TypeInfo
     getFieldAt( size_t  index ) const;     
     
     ULONG
     getFieldOffset() const {
        return  m_parentOffset;
     }  
     
     boost::python::object
     getFieldByIndex( boost::python::object &index ) const;   
     
     size_t
     getFieldCount() const {
        return m_fields.size();
     }
     
     void
     appendField( const TypeInfo &typeInfo, const std::string &fieldName, ULONG count = 1 );
     
     bool
     isBaseType() const {
        return m_isBaseType;
     }
     
     bool
     isPtr() const {
        return m_isPointer;
     }
     
     boost::python::object
     loadVar( ULONG64  targetOffset, ULONG count = 1) const;

public:

    typedef std::map< std::pair<std::string, std::string>, TypeInfo>        TypeInfoMap;
    
    template< typename TTypeInfo>
    struct TypeFieldT {
    
        std::string     name;
        
        ULONG           offset;
        
        ULONG           size;
        
        TTypeInfo       type;
        
        TypeFieldT( const std::string &name_, const TTypeInfo  &type_,  ULONG size_, ULONG offset_ ) :
            name( name_ ),
            size( size_ ),
            offset( offset_ ),
            type( type_ )               
            {}        
            
        std::string print() const;
    };
    
    typedef TypeFieldT<TypeInfo>        TypeField;

    typedef std::vector<TypeField>      TypeFieldList;

private:

    typedef
    boost::python::object
    (*basicTypeLoader)( void* address, size_t size );
    
    typedef 
    std::string
    (*basicTypePrinter)( void* address, size_t size );
    
    static TypeInfoMap          g_typeInfoCache; 
    
    static const char*          basicTypeNames[];
    
    static size_t               basicTypeSizes[]; 
    
    static basicTypeLoader      basicTypeLoaders[];
    
    static basicTypePrinter     basicTypePrinters[];

    ULONG                       m_size;     
    
    ULONG                       m_arraySize;
    
    std::string                 m_typeName;
    
    std::string                 m_moduleName;
    
    TypeFieldList               m_fields;
    
    bool                        m_isPointer;
    
    bool                        m_isBaseType;
    
    bool                        m_isFreezed;
    
    ULONG                       m_align;
    
    ULONG                       m_parentOffset;    
    
    static bool  checkBaseType( const std::string  &typeName );
    
    static ULONG  getBaseTypeSize( const std::string  &typeName );
};


///////////////////////////////////////////////////////////////////////////////////

class TypedVar {

public:
    
    TypedVar() :
        m_targetOffset ( 0 )
        {}

    TypedVar( const TypeInfo  &typeInfo, ULONG64 targetOffset ) :
        m_typeInfo( typeInfo ),
        m_targetOffset( addr64(targetOffset) )
        {}
        
    TypedVar( const std::string &moduleName, const std::string &typeName, ULONG64 targetOffset ) :
        m_typeInfo( moduleName, typeName ),
        m_targetOffset( addr64(targetOffset) )
        {}        
        
    ULONG64
    getAddress() const {
        return m_targetOffset;
    }     

    ULONG
    getSize() const {
        return m_typeInfo.fullSize();
    }
    
    static
    boost::python::object
    getFieldWrap( PyObject* self, const std::string  &fieldName );
    
    boost::python::object
    getField( boost::python::object  &pyobj, const std::string  &fieldName ); 
    
    ULONG64 getTargetOffset() const {
        return m_targetOffset;
    }      
    
    std::string data();
    
    std::string print();
        
private:

    void reallocBuffer();

    TypedVar( const TypeInfo &typeInfo, ULONG64 targetOffset, char* buffer, size_t bufferLength );
  
    ULONG64                 m_targetOffset;  

    TypeInfo                m_typeInfo;    

    std::vector<char>       m_buffer;
};

///////////////////////////////////////////////////////////////////////////////////    

boost::python::object
loadTypedVarList( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &listEntryName );

boost::python::object
loadTypedVarArray( ULONG64 address, const std::string &moduleName, const std::string &typeName, long number );

boost::python::object
containingRecord( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &fieldName );

ULONG
sizeofType( const std::string &moduleName, const std::string &typeName );

///////////////////////////////////////////////////////////////////////////////////    


    
    
//public:
//
//    typedVarClass() : m_addr(0) 
//    {}
//
//    typedVarClass( const TypeInfo  &typeInfo, ULONG64 addr)  :
//        m_typeInfo( typeInfo ),
//        m_addr( addr )
//        {}
//
//    ULONG64
//    getAddress() const {
//        return m_addr;
//    }
//
//    //virtual void 
//    //printField( const TypeInfo::TypeField &field, std::stringstream &sstr ) const override;
//
//    //virtual void
//    //printSelf( std::stringstream &sstr ) const override 
//    //{
//    //    sstr << std::hex << "0x" << getAddress() << std::dec << "  ";
//    //}
//
//private:
//
//    TypeInfo            m_typeInfo;
//    
//    ULONG64             m_addr;
//};

///////////////////////////////////////////////////////////////////////////////////





//
//boost::python::object
//loadTypedVar( const std::string &moduleName, const std::string &typeName, ULONG64 address );
//
//boost::python::object
//loadTypedVarList( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &listEntryName );
//
//boost::python::object
//loadTypedVarArray( ULONG64 address, const std::string &moduleName, const std::string &typeName, long number );
//
//boost::python::object
//containingRecord( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &fieldName );
//
//boost::python::object
//getTypeClass( const std::string &moduleName, const std::string &typeName );
//
//ULONG
//sizeofType( const std::string &moduleName, const std::string &typeName );
//
//
///////////////////////////////////////////////////////////////////////////////////
//
//class TypeInfo {
//
//public:
//
//    template< typename TTypeInfo>
//    struct TypeFieldT {
//        ULONG           size;
//        ULONG           offset;
//        TTypeInfo       type;
//        std::string     name;
//
//        TypeFieldT( const std::string &name_, const TTypeInfo  &type_,  ULONG size_, ULONG offset_ ) :
//            name( name_ ),
//            size( size_ ),
//            offset( offset_ ),
//            type( type_ )               
//            {}
//    };
//    
//    struct TypeName {
//        std::string   module;
//        std::string   symbol;
//        
//        TypeName( const std::string &module_, const std::string  &symbol_ ) :
//            module( module_ ),
//            symbol( symbol_ )
//            {}
//            
//        bool
//        operator < ( const TypeName &typeName ) const {
//
//              if ( typeName.module < module )
//                    return true;
//
//              if ( typeName.module > module )
//                    return false;
//
//              return typeName.symbol < symbol;
//        }
//  
//    };
//    
//    typedef TypeFieldT<TypeInfo>                TypeField;
//    
//    typedef std::map<TypeName, TypeInfo>        TypeInfoMap;
//    
//    typedef std::list<TypeField>                TypeFieldList;
//
//public:
//
//    TypeInfo() :
//        m_size( 0 ),
//        m_baseType( false ),
//        m_pointer( false )
//        {}
//        
//    TypeInfo( const std::string  &typeName ) :
//        m_size( 0 ),
//        m_baseType( false ),
//        m_pointer( false ),
//        m_typeName( typeName )
//        {}  
//            
//    boost::python::object
//    load( ULONG64 targetAddr, PVOID cacheBuffer = NULL, ULONG offset = 0 ) const;
//
//    boost::python::object
//    build( ULONG offset = 0 ) const;
//
//    ULONG64
//    size() const 
//    {
//        return m_size;
//    }
//
//    const std::string&
//    name() const 
//    {
//        return m_typeName;
//    }
//
//    const TypeFieldList&
//    getFields() const {
//        return m_fields;
//    }
//
//    bool
//    isComplex() const {
//        return !m_baseType;
//    }
//
//    bool
//    isPtr() const {
//        return m_pointer;
//    }
//    
//    static const TypeInfo*
//    get( const std::string  &moduleName, const std::string  &typeName );     
//
//private:
//
//    static TypeInfoMap                          g_typeInfoCache; 
//
//    boost::python::object
//    loadBaseType( PVOID addr ) const;
//
//    boost::python::object
//    ptrLoader( PVOID addr ) const  {
//        if ( is64bitSystem() )
//            return boost::python::object( *(PULONG64)addr );
//        else
//            return boost::python::object( addr64( *(PULONG)addr ) );
//    }
//
//    void
//    setupBaseType();
//    
//    static bool
//    getById( const std::string  &moduleName, ULONG typeId, TypeInfo& typeInfo );
//
//private:
//
//    bool                                m_baseType;
//    bool                                m_pointer;
//    TypeFieldList                       m_fields;
//    std::string                         m_typeName;
//    ULONG                               m_size;
//};
//
///////////////////////////////////////////////////////////////////////////////////
//
//class typeClass
//{
//public:
//
//    typeClass()
//      : m_offset(0)
//    {
//    }
//
//    typeClass(
//        const TypeInfo &typeInfo,
//        ULONG offset
//    ) : m_typeInfo(typeInfo)
//      , m_offset(offset)
//    {
//    }
//
//    // sizeof(TYPE)
//    ULONG size() const
//    {
//        return (ULONG)m_typeInfo.size();
//    }
//
//    void setPyObj( const boost::python::object  &obj )
//    {
//        m_pyobj = obj;
//    }
//
//    // TypeInfo getter
//    TypeInfo &getTypeInfo()
//    {
//        return m_typeInfo;
//    }
//    const TypeInfo &getTypeInfo() const
//    {
//        return m_typeInfo;
//    }
//
//    // boost::python::object getter
//    boost::python::object &getPyObj()
//    {
//        return m_pyobj;
//    }
//    const boost::python::object &getPyObj() const
//    {
//        return m_pyobj;
//    }
//
//    std::string print() const;
//
//    virtual void printField(
//        const TypeInfo::TypeField &field,
//        std::stringstream &sstr
//    ) const 
//    {
//        // no data - nothing print
//    }
//    virtual void printSelf(
//        std::stringstream &sstr
//    ) const 
//    {
//        // no data - nothing print
//    }
//
//    // field offset getter/setter
//    ULONG getOffset() const { return m_offset; }
//
//private:
//    TypeInfo m_typeInfo;
//    ULONG  m_offset;
//    boost::python::object m_pyobj;
//};
//
///////////////////////////////////////////////////////////////////////////////////
//
//class typedVarClass : public typeClass {
//
//public:
//
//    typedVarClass() : m_addr(0) 
//    {}
//
//    typedVarClass( const TypeInfo  &typeInfo, ULONG offset, ULONG64 addr)  :
//        typeClass( typeInfo, offset ),
//        m_addr( addr )
//        {}
//
//    ULONG64
//    getAddress() const {
//        return m_addr;
//    }
//
//    virtual void 
//    printField( const TypeInfo::TypeField &field, std::stringstream &sstr ) const override;
//
//    virtual void
//    printSelf( std::stringstream &sstr ) const override 
//    {
//        sstr << std::hex << "0x" << getAddress() << std::dec << "  ";
//    }
//
//private:
//
//    ULONG64                     m_addr;
//};
//
///////////////////////////////////////////////////////////////////////////////////