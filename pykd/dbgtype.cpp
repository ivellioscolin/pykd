#include "stdafx.h"

#include <sstream>

#include <boost/python/reference_existing_object.hpp>

#include "dbgext.h"
#include "dbgtype.h"
#include "dbgexcept.h"
#include "dbgmem.h"
#include "dbgsystem.h"
#include "dbgmem.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////

TypeInfo::TypeInfoMap    TypeInfo::g_typeInfoCache;

///////////////////////////////////////////////////////////////////////////////////

TypeInfo::TypeInfo( const std::string &moduleName, const std::string  &typeName ) 
{
    HRESULT     hres;
    bool        baseType = checkBaseType(typeName); 
    
    TypeInfoMap::iterator     findIt = baseType ? 
        g_typeInfoCache.find( std::make_pair("",typeName) ) :
        g_typeInfoCache.find( std::make_pair(moduleName,typeName) );
    
    if ( findIt != g_typeInfoCache.end() )
    {
        *this = findIt->second;
        return;
    }
    
    m_typeName = typeName;
    
    m_isPointer = false;
    m_isBaseType = false;
    m_isFreezed = true;
    m_parentOffset = 0;
    
    if ( !baseType )
        m_moduleName = moduleName;  
    
    do {
    
        if ( typeName.find("*") < typeName.size() )
        {
            m_isPointer = true;
            m_size = ptrSize();
            break;
        }       
    
        if ( baseType )
        {
            m_isBaseType = true;
            m_size = getBaseTypeSize( typeName );
            break;            
        }         
    
        ULONG64     moduleBase = 0;
        hres = dbgExt->symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &moduleBase );
        if ( FAILED( hres ) )
            throw TypeException();
            
        ULONG       typeId = 0;
        hres = dbgExt->symbols->GetTypeId( moduleBase, m_typeName.c_str(), &typeId );
        if ( FAILED( hres ) )
            throw TypeException();

        hres = dbgExt->symbols->GetTypeSize( moduleBase, typeId, &m_size );
        if ( FAILED( hres ) )
            throw TypeException();    
            
        for ( ULONG   i = 0; ; ++i )
        {
            char   fieldName[100];
            hres = dbgExt->symbols2->GetFieldName( moduleBase, typeId, i, fieldName, sizeof(fieldName), NULL );
            if ( FAILED( hres ) )
                break;  
            
            ULONG   fieldTypeId;
            ULONG   fieldOffset;
            hres = dbgExt->symbols3->GetFieldTypeAndOffset( moduleBase, typeId, fieldName, &fieldTypeId, &fieldOffset );

            if ( FAILED( hres ) )
                throw TypeException();

            ULONG   fieldSize;                
            hres = dbgExt->symbols->GetTypeSize( moduleBase, fieldTypeId, &fieldSize );
            if ( FAILED( hres ) )
                throw TypeException();

            char    fieldTypeName[100];
            hres = dbgExt->symbols->GetTypeName( moduleBase, fieldTypeId, fieldTypeName, sizeof(fieldTypeName), NULL );
            if ( FAILED( hres ) )
                throw TypeException();

            std::string     fieldTypeNameStr( fieldTypeName );
            
            if ( fieldTypeNameStr == "__unnamed" 
             ||  fieldTypeNameStr.find("<unnamed-tag>") < fieldTypeNameStr.size() )
            {
                m_fields.push_back( TypeField( fieldName, TypeInfo( moduleName, moduleBase, fieldTypeId ), fieldSize, fieldOffset ) );     
            }
            else
            {
                m_fields.push_back( TypeField( fieldName, TypeInfo( moduleName, fieldTypeName ), fieldSize, fieldOffset ) );                      
            }   
       }   
       
    } while( FALSE );
    
    m_arraySize = m_size;

    g_typeInfoCache.insert( std::make_pair( std::make_pair( m_moduleName, m_typeName), *this) ); 
}

///////////////////////////////////////////////////////////////////////////////////

TypeInfo::TypeInfo( const std::string &moduleName, ULONG64 moduleBase, ULONG typeId )
{
    HRESULT     hres;
    
    m_isPointer = false;
    m_isBaseType = false;    
    m_isFreezed = true;
    m_parentOffset = 0;

    hres = dbgExt->symbols->GetTypeSize( moduleBase, typeId, &m_size );
    if ( FAILED( hres ) )
        throw TypeException();    
        
    for ( ULONG   i = 0; ; ++i )
    {
        char   fieldName[100];
        hres = dbgExt->symbols2->GetFieldName( moduleBase, typeId, i, fieldName, sizeof(fieldName), NULL );
        if ( FAILED( hres ) )
            break;  
        
        ULONG   fieldTypeId;
        ULONG   fieldOffset;
        hres = dbgExt->symbols3->GetFieldTypeAndOffset( moduleBase, typeId, fieldName, &fieldTypeId, &fieldOffset );

        if ( FAILED( hres ) )
            throw TypeException();

        ULONG   fieldSize;                
        hres = dbgExt->symbols->GetTypeSize( moduleBase, fieldTypeId, &fieldSize );
        if ( FAILED( hres ) )
            throw TypeException();

        char    fieldTypeName[100];
        hres = dbgExt->symbols->GetTypeName( moduleBase, fieldTypeId, fieldTypeName, sizeof(fieldTypeName), NULL );
        if ( FAILED( hres ) )
            throw TypeException();

        std::string     fieldTypeNameStr( fieldTypeName );
        
        if ( fieldTypeNameStr == "__unnamed" 
         ||  fieldTypeNameStr.find("<unnamed-tag>") < fieldTypeNameStr.size() )
        {
            m_fields.push_back( TypeField( fieldName, TypeInfo( moduleName, moduleBase, fieldTypeId ), fieldSize, fieldOffset ) );     
        }
        else
        {
            m_fields.push_back( TypeField( fieldName, TypeInfo( moduleName, fieldTypeName ), fieldSize, fieldOffset ) );                      
        }
   }

   m_arraySize = m_size;
}

///////////////////////////////////////////////////////////////////////////////////

const TypeInfo&
TypeInfo::get( const std::string &moduleName, const std::string  &typeName )
{
    TypeInfoMap::iterator     findIt = g_typeInfoCache.find( std::make_pair(moduleName,typeName) );
    
    if ( findIt != g_typeInfoCache.end() )
        return  findIt->second;
        
    TypeInfo     type( moduleName, typeName );
        
    return  g_typeInfoCache.insert( std::make_pair( std::make_pair( moduleName, typeName), type) ).first->second;
}     

///////////////////////////////////////////////////////////////////////////////////

std::string
TypeInfo::print() const
{
    std::stringstream       sstream;
    
    if ( m_typeName.size() != 0 )
    {
        if ( m_moduleName.size() != 0 )
            sstream << m_moduleName << "!";
            
        sstream << m_typeName;           
    }
    else
        sstream << "unnamed";        
        
    if ( m_arraySize > m_size )
    {
        sstream <<  "  size = " << dec << m_arraySize << "(0x" << hex << m_arraySize << ") " << dec << "[" <<  m_arraySize/m_size << "]";
    }
    else
    {
        sstream << "   size = " << dec << m_size << "(0x" << hex << m_size << ")" << endl;
    }        
   
    TypeFieldList::const_iterator     it = m_fields.begin();
    for(;it != m_fields.end(); ++it )
    {
        sstream << "\t" << it->print() << std::endl;
    }        
        
    return sstream.str();       
}

///////////////////////////////////////////////////////////////////////////////////

TypeInfo
TypeInfo::getField( const std::string  &fieldName ) const
{
    TypeFieldList::const_iterator     it = m_fields.begin();
    for(;it != m_fields.end(); ++it )
    {
        if ( it->name == fieldName )
        {
            TypeInfo    tinf = it->type;

            tinf.m_parentOffset = m_parentOffset + it->offset;
            tinf.m_arraySize = it->size;

            return  tinf;
        }
    }

    throw TypeException();  
}

///////////////////////////////////////////////////////////////////////////////////

TypeInfo
TypeInfo::getFieldAt( size_t index ) const 
{
    TypeInfo   tinf = m_fields[index].type;

    tinf.m_parentOffset = m_parentOffset + m_fields[index].offset;
    tinf.m_arraySize =  m_fields[index].size;

    return  tinf;
}

///////////////////////////////////////////////////////////////////////////////////

boost::python::object
TypeInfo::getFieldByIndex( boost::python::object  &obj ) const
{
    if ( PyInt_Check( obj.ptr() ) )
    {
        long index = PyInt_AsLong(obj.ptr());
        
        if ( index > (long)m_fields.size() )
            throw IndexException();
        
        if ( index < 0 )
        {
            index = (long)m_fields.size() - abs(index);
            if ( index < 0 )
                throw IndexException();
        }
           
        TypeInfo    tinf = m_fields[index].type;
        tinf.m_parentOffset = m_fields[index].offset;        
        return boost::python::object( tinf );             
    }
    else
    {
        boost::python::slice  sl( boost::python::detail::borrowed_reference( obj.ptr() ) );
        
        boost::python::slice::range< TypeFieldList::const_iterator >   bounds;
        
        bounds = sl.get_indicies( m_fields.begin(), m_fields.end() );
        
        boost::python::list   valList;
        
        do {
        
            TypeInfo    tinf = bounds.start->type;
            tinf.m_parentOffset = bounds.start->offset; 
        
            valList.append( tinf );
            
            if ( bounds.start == bounds.stop)
                break;
            
            std::advance( bounds.start, bounds.step);
            
        } while( true );       
        
        return valList;
    }

    throw IndexException();
    
    return boost::python::object();
}

///////////////////////////////////////////////////////////////////////////////////

void
TypeInfo::appendField( const TypeInfo &typeInfo, const std::string &fieldName, ULONG count )
{
    if ( m_isFreezed )
        throw TypeException();
        
    if ( count == 0 )
        throw TypeException();
   
    if ( count == 1 && typeInfo.m_typeName.find("[]") != std::string::npos )
        throw TypeException();
        
    TypeFieldList::const_iterator     it = m_fields.begin();
    for(;it != m_fields.end(); ++it )
    {
        if ( it->name == fieldName )
            throw TypeException(); 
    }
    
    if ( count > 1 && typeInfo.m_typeName.find("[]") == std::string::npos )
    {
        TypeInfo    arrayInfo( typeInfo );
        arrayInfo.m_typeName += "[]";
        appendField( arrayInfo, fieldName, count );
        return;
    }

    ULONG  offset = m_size;

    if ( typeInfo.isBaseType() )
    {
       offset += offset % min( typeInfo.size(), m_align );  
    }

    const ULONG addSize = typeInfo.size() * count;
    m_fields.push_back( TypeField( fieldName, typeInfo, addSize, offset ) );
    m_size = offset + addSize;
    m_arraySize = offset + addSize;
}

///////////////////////////////////////////////////////////////////////////////////

boost::python::object
TypeInfo::loadVar( ULONG64  targetOffset, ULONG count ) const
{
    if ( count == 0 )
        throw TypeException();   

    if ( count == 1 )
    {
       boost::python::object   obj( TypedVar( *this, targetOffset ) );
    
       return obj;
    }
    else
    {
        boost::python::list    lst;
        
        for( ULONG i = 0; i < count; ++i )
            lst.append( TypedVar( *this, targetOffset + m_size*i ) );           
    
        return  lst;
    }       
}

///////////////////////////////////////////////////////////////////////////////////

template< typename valType>
boost::python::object
valueLoader( void* address, size_t size )
{
    if ( size == sizeof(valType) )
    {
        valType     v = *(valType*)address;
        //return boost::python::long_( (unsigned __int64)v );
        
        return boost::python::object( v );
        
        
        //if ( loadMemory( address, &v, sizeof(v) ) )
        //{
        //    return boost::python::long_( (unsigned __int64)v );
        //}            
    }
    else
    {    
        boost::python::list     arr;

        for ( unsigned int i = 0; i < size / sizeof(valType); ++i )
        {
            valType  v = *( (valType*)address + i );
            arr.append( v );
            
            //arr[i] = boost::python::long_( (unsigned __int64)v );    
        }

        return arr;
    }
    
    throw TypeException(); 
}

template<>
boost::python::object
valueLoader<void*>( void* address, size_t size )
{

    if ( is64bitSystem() )
    {
        if ( size == sizeof(ULONG64) )
        {
            return boost::python::object( *(PULONG64)address );
        }
        else
        { 
            boost::python::list     arr;

            for ( unsigned int i = 0; i < size / sizeof(ULONG64); ++i )
            {
                ULONG64     v = *( (PULONG64)address + i );
                arr.append( v );
            }

            return arr;        
        }
    }
    else
    {    
        if ( size == sizeof(ULONG) )
        {
            return boost::python::object( addr64( *(PULONG)address ) );
        }
        else
        { 
            boost::python::list     arr;

            for ( unsigned int i = 0; i < size / sizeof(ULONG); ++i )
            {
                ULONG64     v = addr64(*( (PULONG)address + i ) );
                arr.append( v );
            }

            return arr;        
        }
    }
    
    throw TypeException();     
}

template<>
boost::python::object
valueLoader<void>( void* address, size_t size )
{
    return boost::python::object();
}



template <typename valType>
std::string
valuePrinter( void* address, size_t size )
{
    std::stringstream   sstr;

    if ( size == sizeof(valType) )
    {
        valType     v = *(valType*)address;

        sstr << v << hex << " (0x" << v << ")";
    }
    else
    {    

        for ( unsigned int i = 0; i < size / sizeof(valType); ++i )
        {
            valType  v = *( (valType*)address + i );
            sstr << endl << "\t[" << dec << i << "]  " << dec << v << " (" << hex << v << ")";
        }
    }

    return sstr.str();
}  

template<>
std::string
valuePrinter<void*>( void* address, size_t size )
{
    if ( is64bitSystem() )
    {
        return valuePrinter<ULONG64>( address, size  );
    }

    return valuePrinter<ULONG>( address, size  );
}

template <>
std::string
valuePrinter<unsigned char>( void* address, size_t size )
{
    std::stringstream   sstr;

    if ( size == sizeof(unsigned char) )
    {
        unsigned int     v = *(unsigned char*)address;

        sstr << v;
    }
    else
    {    

        for ( unsigned int i = 0; i < size / sizeof(unsigned char); ++i )
        {
            unsigned int  v = *( (unsigned char*)address + i );
            sstr << endl << "\t[" << dec << i << "]  " << dec << v << " (" << hex << v << ")";
        }
    }

    return sstr.str();
}


template <>
std::string
valuePrinter<char>( void* address, size_t size )
{
    std::stringstream   sstr;

    if ( size == sizeof(unsigned char) )
    {
        int     v = *(char*)address;

        sstr << v;
    }
    else
    {    

        for ( unsigned int i = 0; i < size / sizeof(unsigned char); ++i )
        {
            int  v = *( (char*)address + i );
            sstr << endl << "\t[" << dec << i << "]  " << dec << v << " (" << hex << v << ")";
        }
    }

    return sstr.str();
}
    

template<>
std::string
valuePrinter<void>( void* address, size_t size )
{
    return "";
}


const char*   TypeInfo::basicTypeNames[] = {
        "unsigned char",
        "char",
        "unsigned short",
        "short", 
        "unsigned long",
        "long",
        "int",
        "unsigned int",
        "<function>",
        "void",
        "double",
        "int64",
        "unsigned int64",
        "ptr"
};

    
size_t   TypeInfo::basicTypeSizes[] = {
    sizeof( unsigned char ),
    sizeof( char ),
    sizeof( unsigned short ),
    sizeof( short ),
    sizeof( unsigned long ),
    sizeof( long ),
    sizeof( int ),
    sizeof( unsigned int ),
    sizeof( void* ),
    0,
    sizeof( double ),
    sizeof( __int64 ),
    sizeof( unsigned __int64 )
};


TypeInfo::basicTypeLoader     TypeInfo::basicTypeLoaders[] = {
    valueLoader<unsigned char>,
    valueLoader<char>,
    valueLoader<unsigned short>,
    valueLoader<short>,
    valueLoader<unsigned long>,
    valueLoader<long>,
    valueLoader<int>,
    valueLoader<unsigned int>,
    valueLoader<void*>,
    valueLoader<void>,
    valueLoader<double>,
    valueLoader<__int64>,
    valueLoader<unsigned __int64>
 };
 
TypeInfo::basicTypePrinter    TypeInfo::basicTypePrinters[] = {
    valuePrinter<unsigned char>,
    valuePrinter<char>,
    valuePrinter<unsigned short>,
    valuePrinter<short>,
    valuePrinter<unsigned long>,
    valuePrinter<long>,
    valuePrinter<int>,
    valuePrinter<unsigned int>,
    valuePrinter<void*>,
    valuePrinter<void>,
    valuePrinter<double>,
    valuePrinter<__int64>,
    valuePrinter<unsigned __int64>
};

bool
TypeInfo::checkBaseType( const std::string  &typeName  )
{
    for ( int i = 0; i < sizeof( basicTypeNames ) / sizeof( char* ); ++i )
    {
        if ( typeName == basicTypeNames[i] )
            return true;

        if ( typeName == ( std::string( basicTypeNames[i] ) + "*" ) )
            return true;

        if ( typeName == ( std::string( basicTypeNames[i] ) + "[]" ) )
            return true;    

        if ( typeName == ( std::string( basicTypeNames[i] ) + "*[]" ) )
            return true;    
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////

ULONG
TypeInfo::getBaseTypeSize( const std::string  &typeName )
{
   for ( int i = 0; i < sizeof( basicTypeSizes ) / sizeof( size_t ); ++i )
   {
        if ( typeName == basicTypeNames[i] ||
            typeName == ( std::string( basicTypeNames[i] ) + "[]" ) )
        {
            return (ULONG)basicTypeSizes[i];
        }
   }    
   
   return 0;
}

///////////////////////////////////////////////////////////////////////////////////

boost::python::object
TypeInfo::load( void* buffer, size_t  bufferLength ) const
{
    if ( bufferLength < m_size )
         throw TypeException();    

    if ( m_isPointer )
    {
        return valueLoader<void*>( buffer, bufferLength );
    }
    
    if ( m_isBaseType )
    {
       for ( int i = 0; i < sizeof( basicTypeNames ) / sizeof( char* ); ++i )
       {
            if ( m_typeName == basicTypeNames[i] )
                return basicTypeLoaders[i]( buffer, m_size );

            if ( m_typeName == ( std::string( basicTypeNames[i] ) + "*" ) )
                return valueLoader<void*>( buffer, ptrSize() );

            if ( m_typeName == ( std::string( basicTypeNames[i] ) + "[]" ) )
                return basicTypeLoaders[i]( buffer, m_arraySize ); 

            if ( m_typeName == ( std::string( basicTypeNames[i] ) + "*[]" ) )
                 return valueLoader<void*>( buffer, ptrSize()*count() );
        }   
    }

    throw TypeException();
}

///////////////////////////////////////////////////////////////////////////////////

template<>
std::string
TypeInfo::TypeField::print() const 
{
    std::stringstream       sstr;
    
    sstr << hex << "+" << offset <<  "  ";
    sstr << type.m_typeName <<  "   ";
    sstr << name;
    
    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////////

std::string
TypeInfo::printField( size_t index, void* buffer, size_t  bufferLength ) const
{
    stringstream        sstr;
    const TypeField     &field = m_fields[index];
    const TypeInfo      &fieldType = field.type; 
    ULONG               offset = field.offset;   
    ULONG               count = field.size / fieldType.size();
    
    if ( fieldType.m_isPointer )
    {
        sstr << hex << "+" << offset;
        sstr << "   " <<  field.name;
        sstr << "   " << fieldType.name();
        sstr << "   " << hex << valuePrinter<void*>( (char*)buffer + offset, field.size );
        sstr << endl;
        return sstr.str();
    }
    
    if ( fieldType.m_isBaseType )
    {
        for ( int i = 0; i < sizeof( basicTypeNames ) / sizeof( char* ); ++i )
        {
            if ( fieldType.m_typeName == basicTypeNames[i] || 
                fieldType.m_typeName == ( std::string( basicTypeNames[i] ) + "[]" ) )
            {
                sstr << hex << "+" << offset;
                sstr << "   " <<  field.name;
                sstr << "   " << fieldType.name();
                sstr << "   " << basicTypePrinters[i]( (char*)buffer + offset, field.size );
                sstr << endl;
                return sstr.str();     
            }                
         }  
        
        throw TypeException();          
    }    

    if ( count == 1 )
    {
        sstr << hex << "+" << offset;
        sstr << "   " <<  field.name;
        sstr << "   " << fieldType.name();
        sstr << endl;
    }
    else
    {                        
        sstr << hex << "+" << offset;
        sstr << "   " <<  field.name;
        sstr << "   " << fieldType.name() << "[" << dec << count << "]";
        sstr << endl;  
    }          
    
    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////////

TypedVar::TypedVar( const TypeInfo &typeInfo, ULONG64 targetOffset, char* buffer, size_t bufferLength ) :
    m_typeInfo( typeInfo ),
    m_targetOffset( addr64(targetOffset) )
{
    if ( bufferLength < typeInfo.size() )
        throw TypeException();
        
    m_buffer.insert( m_buffer.begin(), buffer, buffer + bufferLength );
}
  
///////////////////////////////////////////////////////////////////////////////////

boost::python::object
TypedVar::getFieldWrap( PyObject* self, const std::string  &fieldName )
{
    boost::python::object     pyobj( boost::python::handle<>( boost::python::borrowed(self) ) );
    
    TypedVar    &tv = boost::python::extract<TypedVar&>( pyobj );
    
    return tv.getField( pyobj, fieldName );
}

void TypedVar::reallocBuffer()
{
    const size_t fullSize = m_typeInfo.fullSize();
    if (m_buffer.size() < fullSize)
    {
        assert(fullSize);
        m_buffer.resize( fullSize );
        loadMemory( m_targetOffset, (PVOID)&m_buffer[0],  (ULONG)m_buffer.size() );
    }
}

boost::python::object  
TypedVar::getField( boost::python::object  &pyobj, const std::string  &fieldName )
{
    reallocBuffer();

    TypeInfo   typeInfo = m_typeInfo.getField( fieldName );
    
    // относительный оффсет
    ULONG      offset = typeInfo.getFieldOffset() - m_typeInfo.getFieldOffset();
    
    if ( typeInfo.isBaseType() || typeInfo.isPtr() )
    {
        pyobj.attr(fieldName.c_str()) = typeInfo.load( &(m_buffer.begin() + offset)[0], typeInfo.fullSize() );
    }
    else
    {
        if ( typeInfo.count() == 1 )
        {
            if (m_buffer.size())
            {
                pyobj.attr(fieldName.c_str()) =  
                    boost::python::object( 
                        TypedVar( 
                            typeInfo, 
                            m_targetOffset + offset,
                            &m_buffer[0] + offset,
                            typeInfo.size() ) );
            }
        }
        else
        {                        
            boost::python::list     arr;
    
            for ( unsigned int i = 0; i < typeInfo.count(); ++i )
            {
                if (m_buffer.size())
                {
                    arr.append(    
                        boost::python::object( 
                            TypedVar( 
                                typeInfo, 
                                m_targetOffset + offset + i*typeInfo.size(),
                                &m_buffer[0] + offset + i*typeInfo.size(),
                                typeInfo.size() ) ) );
                }
            }
            
            pyobj.attr(fieldName.c_str()) = arr;
        }
    }

    return pyobj.attr(fieldName.c_str());
}

/////////////////////////////////////////////////////////////////////////////////////

std::string 
TypedVar::data()
{
    reallocBuffer();
    return std::string( getVectorBuffer(m_buffer), m_buffer.size() );
}

/////////////////////////////////////////////////////////////////////////////////////

std::string 
TypedVar::print() 
{
    reallocBuffer();

    stringstream sstr;
    
    if ( m_typeInfo.name().size() != 0 )
    {
        if ( m_typeInfo.moduleName().size() )
            sstr << m_typeInfo.moduleName() << "!";
            
        sstr << m_typeInfo.name();           
    }
    else
        sstr << "unnamed";
        
    sstr << "  address = " << hex << "0x" << getTargetOffset();       
    sstr << "  size = " << dec << m_typeInfo.fullSize() << " (0x" << hex <<  m_typeInfo.fullSize() << ")" << endl;      
       
   
    for ( size_t  i = 0; i < m_typeInfo.getFieldCount(); ++i )
    {
        sstr << m_typeInfo.printField( i, (PVOID)getVectorBuffer(m_buffer), (ULONG)m_buffer.size() );
    
        //TypeInfo   fieldType = m_typeInfo.getFieldAt( i );
    
        //// относительный оффсет
        //ULONG      offset = fieldType.getFieldOffset() - m_typeInfo.getFieldOffset();
    
        //if ( fieldType.isBaseType() || fieldType.isPtr() )
        //{
        //    sstr << hex << "+" << offset << "   " << fieldType.name();
        //    sstr << fieldType.printValue( &(m_buffer.begin() + offset)[0], fieldType.fullSize() );
        //}
        //else
        //{
        //    if ( fieldType.count() == 1 )
        //    {
        //        sstr << hex << "+" << offset << "   " << fieldType.name() << endl;
        //    }
        //    else
        //    {                        
        //        sstr << hex << "+" << offset << "   " << fieldType.name() << "[" << dec << "]" << endl;
        //    }
        //}            
    }

    return sstr.str();
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG
sizeofType( const std::string &moduleName, const std::string &typeName )
{
    return TypeInfo::get( moduleName, typeName ).size(); 
}

///////////////////////////////////////////////////////////////////////////////////

boost::python::object
containingRecord( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &fieldName )
{
    address = addr64(address); 

    HRESULT         hres;
    ULONG64         moduleBase;

    hres = dbgExt->symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &moduleBase );
    if ( FAILED( hres ) )
         throw TypeException();   

    ULONG        typeId;
    hres = dbgExt->symbols->GetTypeId( moduleBase, typeName.c_str(), &typeId );
    if ( FAILED( hres ) )
         throw TypeException();   

    ULONG       fieldTypeId;
    ULONG       fieldOffset;
    hres = dbgExt->symbols3->GetFieldTypeAndOffset( moduleBase, typeId, fieldName.c_str(), &fieldTypeId, &fieldOffset );   
    if ( FAILED( hres ) )
         throw TypeException(); 
    
    TypedVar   var( moduleName, typeName, address - fieldOffset );
    
    return boost::python::object( var );
}

///////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadTypedVarList( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &listEntryName )
{
    address = addr64(address); 
    
    ULONG64                 entryAddress = 0;

    const TypeInfo          &typeInfo = TypeInfo::get( moduleName, typeName );
    
    boost::python::list     objList;
    
    
    TypeInfo                fieldType = typeInfo.getField( listEntryName );
    
    if ( fieldType.name() == ( typeName + "*" ) )
    {
        for( entryAddress = loadPtrByPtr( address ); entryAddress != address && entryAddress != NULL; entryAddress = loadPtrByPtr( entryAddress + fieldType.getFieldOffset()  ) )
            objList.append( TypedVar( moduleName, typeName, entryAddress ) );    
    }
    else
    {
        for( entryAddress = loadPtrByPtr( address ); entryAddress != address && entryAddress != NULL; entryAddress = loadPtrByPtr( entryAddress ) )
            objList.append( containingRecord( entryAddress, moduleName, typeName, listEntryName ) );   
    }
    
     return objList;  
}

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadTypedVarArray( ULONG64 address, const std::string &moduleName, const std::string &typeName, long number )
{
    address = addr64(address); 

    const TypeInfo          &typeInfo = TypeInfo::get( moduleName, typeName );
    
    boost::python::list     objList;    
    
    for( long i = 0; i < number; ++i )
        objList.append( TypedVar(typeInfo, address + i * typeInfo.size() ) );
    
    return objList;
}

/////////////////////////////////////////////////////////////////////////////////


















































//
//boost::python::object
//loadTypedVar( const std::string &moduleName, const std::string &typeName, ULONG64 address )
//{
//    const TypeInfo        *typeInfo = TypeInfo::get( moduleName, typeName );
//
//    return typeInfo != NULL ? typeInfo->load( address ) : boost::python::object();
//}
//
///////////////////////////////////////////////////////////////////////////////////////
//
//ULONG
//sizeofType( const std::string &moduleName, const std::string &typeName )
//{
//    HRESULT     hres;
//    ULONG       typeSize = ~0;
//
//    try {
//
//        ULONG64         moduleBase;
//
//        hres = dbgExt->symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &moduleBase );
//        if ( FAILED( hres ) )
//            throw  DbgException( "IDebugSymbol::GetModuleByModuleName  failed" ); 
//    
//        const TypeInfo        *typeInfo = TypeInfo::get( moduleName, typeName );
//        
//        return typeInfo != NULL ? (ULONG)typeInfo->size() : 0L;
//    }
//    catch( std::exception  &e )
//    {
//        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
//    }
//    catch(...)
//    {
//        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
//    }
//
//    return typeSize;
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//containingRecord( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &fieldName )
//{
//    HRESULT      hres;
//
//    try {
//        
//        ULONG64         moduleBase;
//
//        hres = dbgExt->symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &moduleBase );
//        if ( FAILED( hres ) )
//            throw  DbgException( "IDebugSymbol::GetModuleByModuleName  failed" ); 
//
//        ULONG        typeId;
//        hres = dbgExt->symbols->GetTypeId( moduleBase, typeName.c_str(), &typeId );
//        if ( FAILED( hres ) )
//            throw  DbgException( "IDebugSymbol::GetTypeId  failed" );
//
//        ULONG       fieldTypeId;
//        ULONG       fieldOffset;
//        hres = dbgExt->symbols3->GetFieldTypeAndOffset( moduleBase, typeId, fieldName.c_str(), &fieldTypeId, &fieldOffset );   
//        
//        return loadTypedVar( moduleName, typeName, address - fieldOffset );
//    }
//    catch( std::exception  &e )
//    {
//        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
//    }
//    catch(...)
//    {
//        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
//    }
//
//    return boost::python::object();
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//getTypeClass( const std::string &moduleName, const std::string &typeName )
//{
//    try
//    {
//        const TypeInfo        *typeInfo = TypeInfo::get( moduleName, typeName );
//
//        return typeInfo != NULL ? typeInfo->build() : boost::python::object();
//    }
//    catch( std::exception  &e )
//    {
//        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
//    }
//    catch(...)
//    {
//        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
//    }
//    
//    return boost::python::object();
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//loadTypedVarList( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &listEntryName )
//{
//    address = addr64(address); 
//    
//    ULONG64                 entryAddress = 0;
//
//    const TypeInfo          *typeInfo = TypeInfo::get( moduleName, typeName );
//    
//    if ( !typeInfo )
//        return boost::python::object();    
//    
//    boost::python::list     objList;
//    
//    for ( TypeInfo::TypeFieldList::const_iterator  field = typeInfo->getFields().begin(); field != typeInfo->getFields().end(); field++ )
//    {
//        if ( field->name == listEntryName )
//        {
//            if ( field->type.name() == ( typeName + "*" ) )
//            {
//                for( entryAddress = loadPtrByPtr( address ); entryAddress != address && entryAddress != NULL; entryAddress = loadPtrByPtr( entryAddress + field->offset  ) )
//                    objList.append( loadTypedVar( moduleName, typeName, entryAddress ) );
//            }                
//            else
//            {
//                for( entryAddress = loadPtrByPtr( address ); entryAddress != address && entryAddress != NULL; entryAddress = loadPtrByPtr( entryAddress ) )
//                    objList.append( containingRecord( entryAddress, moduleName, typeName, listEntryName ) );                  
//            }
//            
//            return objList;   
//        }
//    }
//    
//    return boost::python::object();
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//loadTypedVarArray( ULONG64 address, const std::string &moduleName, const std::string &typeName, long number )
//{
//    boost::python::list     objList;
//    
//    const TypeInfo          *typeInfo = TypeInfo::get( moduleName, typeName );
//    
//    if ( !typeInfo )
//        return boost::python::object();        
//    
//    for( long i = 0; i < number; ++i )
//        objList.append( loadTypedVar( moduleName, typeName, address + i * typeInfo->size() ) );
//    
//    return objList;
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//bool
//isBaseType( const std::string  &typeName );
//
//template< typename valType>
//boost::python::object
//valueLoader( PVOID address, ULONG size );
//
//template<>
//boost::python::object
//valueLoader<void*>( PVOID address, ULONG size )
//{
//    if ( is64bitSystem() )
//        return valueLoader<__int64>( address, size );
//    else
//        return valueLoader<long>( address, size );
//}
//
//boost::python::object
//voidLoader( PVOID address, ULONG size ) {
//    return boost::python::object();
//}
//
//static const char*   
//basicTypeNames[] = {
//    "unsigned char",
//    "char",
//    "unsigned short",
//    "short", 
//    "unsigned long",
//    "long",
//    "int",
//    "unsigned int",
//    "<function>",
//    "void",
//    "double",
//    "int64",
//    "unsigned int64",
//    "ptr"
//};
//
//typedef
//boost::python::object
//(*basicTypeLoader)( PVOID address, ULONG size );
//    
//basicTypeLoader     basicTypeLoaders[] = {
//    valueLoader<unsigned char>,
//    valueLoader<char>,
//    valueLoader<unsigned short>,
//    valueLoader<short>,
//    valueLoader<unsigned long>,
//    valueLoader<long>,
//    valueLoader<int>,
//    valueLoader<unsigned int>,
//    valueLoader<void*>,
//    voidLoader,
//    valueLoader<double>,
//    valueLoader<__int64>,
//    valueLoader<unsigned __int64>
// };
// 
//size_t   basicTypeSizes[] = {
//    sizeof( unsigned char ),
//    sizeof( char ),
//    sizeof( unsigned short ),
//    sizeof( short ),
//    sizeof( unsigned long ),
//    sizeof( long ),
//    sizeof( int ),
//    sizeof( unsigned int ),
//    sizeof( void* ),
//    0,
//    sizeof( double ),
//    sizeof( __int64 ),
//    sizeof( unsigned __int64 )
//};
//     
///////////////////////////////////////////////////////////////////////////////////
//
//TypeInfo::TypeInfoMap    TypeInfo::g_typeInfoCache;
//
//const TypeInfo*
//TypeInfo::get( const std::string  &moduleName, const std::string  &typeName )
//{   
//    HRESULT      hres;
//
//    try {
//    
//        TypeInfoMap::iterator     findIt = g_typeInfoCache.find( TypeName( moduleName, typeName ) );
//
//        if ( findIt != g_typeInfoCache.end() )
//            return  &findIt->second;        
//            
//        TypeInfo        typeInfo( typeName );    
//        
//        do {  
//    
//            if (  typeName.find("*") < typeName.size() )
//            {
//                typeInfo.m_pointer = true;
//                typeInfo.m_size = ptrSize();
//                break;
//            }
//
//            typeInfo.m_baseType = isBaseType( typeName );
//            if ( typeInfo.m_baseType )
//            {
//                typeInfo.setupBaseType();
//                break;
//            }
//
//            ULONG64     moduleBase = 0;
//            hres = dbgExt->symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &moduleBase );
//            if ( FAILED( hres ) )
//                throw  DbgException( "IDebugSymbol::GetModuleByModuleName  failed" ); 
//
//            ULONG       typeId = 0;
//            hres = dbgExt->symbols->GetTypeId( moduleBase, typeInfo.m_typeName.c_str(), &typeId );
//            if ( FAILED( hres ) )
//                throw  DbgException( "IDebugSymbol::GetTypeId  failed" );
//
//            hres = dbgExt->symbols->GetTypeSize( moduleBase, typeId, &typeInfo.m_size );
//            if ( FAILED( hres ) )
//                throw DbgException( "IDebugSymbol::GetTypeSize failed" );
//                
//            for ( ULONG   i = 0; ; ++i )
//            {
//                char   fieldName[100];
//                hres = dbgExt->symbols2->GetFieldName( moduleBase, typeId, i, fieldName, sizeof(fieldName), NULL );
//                
//                if ( FAILED( hres ) )
//                    break;  
//                
//                ULONG   fieldTypeId;
//                ULONG   fieldOffset;
//                hres = dbgExt->symbols3->GetFieldTypeAndOffset( moduleBase, typeId, fieldName, &fieldTypeId, &fieldOffset );
//
//                if ( FAILED( hres ) )
//                    throw  DbgException( "IDebugSymbol3::GetFieldTypeAndOffset  failed" ); 
//
//                ULONG   fieldSize;                
//                hres = dbgExt->symbols->GetTypeSize( moduleBase, fieldTypeId, &fieldSize );
//                if ( FAILED( hres ) )
//                    throw DbgException( "IDebugSymbol::GetTypeSize failed" );
//
//                char    fieldTypeName[100];
//                hres = dbgExt->symbols->GetTypeName( moduleBase, fieldTypeId, fieldTypeName, sizeof(fieldTypeName), NULL );
//
//                std::string     fieldTypeNameStr( fieldTypeName );
//                
//                if ( fieldTypeNameStr == "__unnamed" 
//                 ||  fieldTypeNameStr.find("<unnamed-tag>") < fieldTypeNameStr.size() )
//                {
//                    TypeInfo   unnamedType;
//                    if ( !getById( moduleName, fieldTypeId, unnamedType ) )
//                        return NULL;
//                        
//                     typeInfo.m_fields.push_back( TypeField( fieldName, unnamedType, fieldSize, fieldOffset ) );     
//                }
//                else
//                {
//                    const TypeInfo  *fieldTypeInfo = get( moduleName, fieldTypeName );
//                    if ( !fieldTypeInfo )
//                        return NULL;
//                        
//                    typeInfo.m_fields.push_back( TypeField( fieldName, *fieldTypeInfo, fieldSize, fieldOffset ) );                         
//                }   
//           }
//           
//        } while( FALSE );          
//       
//        return &g_typeInfoCache.insert( std::make_pair( TypeName( moduleName, typeName ), typeInfo) ).first->second;
//    }
//    catch( std::exception& )
//    {
//        //dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
//        // это нормально: на вход был передан не верный тип
//    }
//    catch(...)
//    {
//        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
//    }    
//    
//    return NULL;    
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//bool
//TypeInfo::getById( const std::string  &moduleName, ULONG typeId, TypeInfo& typeInfo )
//{
//    HRESULT      hres;
//    
//    try {
//
//        ULONG64     moduleBase = 0;
//        hres = dbgExt->symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &moduleBase );
//        if ( FAILED( hres ) )
//            throw  DbgException( "IDebugSymbol::GetModuleByModuleName  failed" ); 
//
//        hres = dbgExt->symbols->GetTypeSize( moduleBase, typeId, &typeInfo.m_size );
//        if ( FAILED( hres ) )
//            throw DbgException( "IDebugSymbol::GetTypeSize failed" );
//            
//        for ( ULONG   i = 0; ; ++i )
//        {
//            char   fieldName[100];
//            hres = dbgExt->symbols2->GetFieldName( moduleBase, typeId, i, fieldName, sizeof(fieldName), NULL );
//            
//            if ( FAILED( hres ) )
//                break;  
//            
//            ULONG   fieldTypeId;
//            ULONG   fieldOffset;
//            hres = dbgExt->symbols3->GetFieldTypeAndOffset( moduleBase, typeId, fieldName, &fieldTypeId, &fieldOffset );
//
//            if ( FAILED( hres ) )
//                throw  DbgException( "IDebugSymbol3::GetFieldTypeAndOffset  failed" ); 
//
//            ULONG   fieldSize;                
//            hres = dbgExt->symbols->GetTypeSize( moduleBase, fieldTypeId, &fieldSize );
//            if ( FAILED( hres ) )
//                throw DbgException( "IDebugSymbol::GetTypeSize failed" );
//
//            char    fieldTypeName[100];
//            hres = dbgExt->symbols->GetTypeName( moduleBase, fieldTypeId, fieldTypeName, sizeof(fieldTypeName), NULL );
//            if ( FAILED( hres ) )
//                throw DbgException( "IDebugSymbol::GetTypeName failed" );            
//
//            std::string     fieldTypeNameStr( fieldTypeName );
//
//            if ( fieldTypeNameStr == "__unnamed" 
//             ||  fieldTypeNameStr.find("<unnamed-tag>") < fieldTypeNameStr.size() )
//            {
//                TypeInfo   unnamedType;
//                if ( !getById( moduleName, fieldTypeId, unnamedType ) )
//                    return false;
//                    
//                 typeInfo.m_fields.push_back( TypeField( fieldName, unnamedType, fieldSize, fieldOffset ) );     
//            }
//            else
//            {
//                const TypeInfo      *fieldTypeInfo = get( moduleName, fieldTypeName );
//                if ( !fieldTypeInfo )
//                    return false;
//                    
//                typeInfo.m_fields.push_back( TypeField( fieldName, *fieldTypeInfo, fieldSize, fieldOffset ) );                         
//            }   
//       }
//       
//       return true;
//    }
//    catch( std::exception& )
//    {
//        //dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
//        // это нормально: на вход был передан не верный тип
//    }
//    catch(...)
//    {
//        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
//    }   
//    
//    return false; 
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//void
//TypeInfo::setupBaseType()
//{
//   for ( int i = 0; i < sizeof( basicTypeSizes ) / sizeof( size_t ); ++i )
//   {
//        if ( m_typeName == basicTypeNames[i] ||
//            m_typeName == ( std::string( basicTypeNames[i] ) + "[]" ) )
//        {
//            m_size = (ULONG)basicTypeSizes[i];
//            return;   
//        }
//    }            
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//TypeInfo::build( ULONG offset /* = 0 */ ) const
//{
//    boost::shared_ptr<typeClass> ptr( new typeClass( *this, offset ) );
//    boost::python::object var( ptr );
//    ptr->setPyObj( var );
//
//    TypeFieldList::const_iterator    field = m_fields.begin();
//    for ( field = m_fields.begin(); field != m_fields.end(); ++field )
//    {
//
//        if ( field->size == field->type.size() )
//        {
//            var.attr( field->name.c_str() ) = 
//                field->type.build( offset + field->offset );
//        }
//        else
//        {
//            boost::python::dict     arr;
//
//            for ( unsigned int i = 0; i < field->size / field->type.size(); ++i )
//            {
//                const ULONG locOffset = field->offset + i * (ULONG)field->type.size();
//                arr[i] = field->type.build( offset + locOffset );
//            }
//
//            var.attr( field->name.c_str() ) = arr;
//        }
//    }
//
//    return var;
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//TypeInfo::load( ULONG64 targetAddr, PVOID cacheBuffer , ULONG offset /* = 0 */ ) const
//{
//    if ( !isOffsetValid( targetAddr) )
//        return boost::python::object();
//        
//    boost::scoped_array<char>       rawBuffer;    
//        
//    if ( cacheBuffer == NULL )
//    {
//        rawBuffer.reset( new char[ m_size ] );
//        cacheBuffer = rawBuffer.get();
//        
//        if ( !loadMemory( targetAddr, cacheBuffer, m_size ) )
//            return boost::python::object();
//    }           
//    
//    if ( m_pointer )
//        return ptrLoader( (PVOID)( (ULONG_PTR)cacheBuffer + offset ) );
//        
//    if ( m_baseType )
//        return loadBaseType( (PVOID)( (ULONG_PTR)cacheBuffer + offset ) );   
//
//    boost::shared_ptr<typedVarClass>    ptr( new typedVarClass( *this, offset, targetAddr ) );
//    boost::python::object               var( ptr );
//    ptr->setPyObj( var );
//
//    TypeFieldList::const_iterator    field = m_fields.begin();
//    for ( field = m_fields.begin(); field != m_fields.end(); ++field )
//    {
//
//        if ( field->size == field->type.size() )
//        {
//            var.attr( field->name.c_str() ) = 
//                field->type.load( targetAddr + field->offset, cacheBuffer, offset + field->offset );
//        }
//        else
//        {
//            boost::python::list     arr;
//
//            for ( unsigned int i = 0; i < field->size / field->type.size(); ++i )
//            {
//                const ULONG locOffset = field->offset + i * (ULONG)field->type.size();
//                arr.append( field->type.load( targetAddr + locOffset, cacheBuffer, offset + locOffset ) );
//            }
//
//            var.attr( field->name.c_str() ) = arr;
//        }
//    }
//
//    return var;
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//bool
//isBaseType( const std::string  &typeName )
//{
//    for ( int i = 0; i < sizeof( basicTypeNames ) / sizeof( char* ); ++i )
//    {
//        if ( typeName == basicTypeNames[i] )
//            return true;
//
//        if ( typeName == ( std::string( basicTypeNames[i] ) + "*" ) )
//            return true;
//
//        if ( typeName == ( std::string( basicTypeNames[i] ) + "[]" ) )
//            return true;    
//
//        if ( typeName == ( std::string( basicTypeNames[i] ) + "*[]" ) )
//            return true;    
//    }
//
//    return false;
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//TypeInfo::loadBaseType( PVOID address ) const
//{
//   for ( int i = 0; i < sizeof( basicTypeNames ) / sizeof( char* ); ++i )
//   {
//        if ( m_typeName == basicTypeNames[i] )
//            return basicTypeLoaders[i]( address, m_size );
//
//        if ( m_typeName == ( std::string( basicTypeNames[i] ) + "*" ) )
//            return valueLoader<void*>( address, ptrSize() );
//
//        if ( m_typeName == ( std::string( basicTypeNames[i] ) + "[]" ) )
//            return basicTypeLoaders[i]( address, m_size ); 
//
//        if ( m_typeName == ( std::string( basicTypeNames[i] ) + "*[]" ) )
//             return valueLoader<void*>( address, ptrSize() );
//    }
//
//    return boost::python::object();
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//template< typename valType>
//boost::python::object
//valueLoader( PVOID address, ULONG size )
//{
//    if ( size == sizeof(valType) )
//    {
//        valType     v = *(valType*)address;
//        return boost::python::long_( (unsigned __int64)v );
//        
//        
//        //if ( loadMemory( address, &v, sizeof(v) ) )
//        //{
//        //    return boost::python::long_( (unsigned __int64)v );
//        //}            
//    }
//    else
//    {    
//        boost::python::dict     arr;
//
//        for ( unsigned int i = 0; i < size / sizeof(valType); ++i )
//        {
//            valType  v = *( (valType*)address + i );
//            //if ( !loadMemory( address + i * sizeof(valType), &v, sizeof(v) ) )
//
//            arr[i] = boost::python::long_( (unsigned __int64)v );    
//        }
//
//        return arr;
//    }
//    
//    return boost::python::object();
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//std::string typeClass::print() const
//{
//    stringstream sstr;
//
//    sstr << getTypeInfo().name() << "  ";
//    printSelf(sstr);
//    sstr << std::endl;
//
//    TypeInfo::TypeFieldList::const_iterator itField = 
//        getTypeInfo().getFields().begin();
//    while (itField != getTypeInfo().getFields().end())
//    {
//        sstr << "\t" << hex << "+" << itField->offset << "  ";
//        sstr << itField->type.name() << "  ";
//        sstr << itField->name << "  ";
//
//        printField(*itField, sstr);
//
//        sstr << std::endl;
//
//        ++itField;
//    }
//
//    return sstr.str();
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//void
//typedVarClass::printField(const TypeInfo::TypeField &field, stringstream &sstr) const
//{
//    if ( field.type.isComplex() && !field.type.isPtr())
//       sstr << field.type.name();
//    else
//    {
//        boost::python::object     attr = getPyObj().attr( field.name.c_str() );
//
//        if ( field.size == field.type.size() )
//        {
//            if ( attr.ptr() == Py_None )
//            {
//                sstr << "memory error";
//            }
//            else
//            {
//                unsigned __int64  val = boost::python::extract<unsigned __int64>( attr );
//
//                sstr << hex << "0x" << val;
//
//                if ( field.type.name() == "char*" )
//                {
//                    char  buf[0x100];
//                    if ( loadCStrToBuffer( val, buf, sizeof(buf) ) )
//                        sstr << "  (" << buf << " )";
//                    else
//                        sstr << "  ( read string error )";
//                }
//                else if ( field.type.name() == "unsigned short*" )
//                {
//                    wchar_t   wbuf[0x100];
//                    if ( loadWStrToBuffer( val, wbuf, sizeof(wbuf) ) )
//                    {
//                        char  mbBuf[0x100] = { 0 };
//
//                        WideCharToMultiByte(
//                            CP_ACP,
//                            0,
//                            wbuf,
//                            (ULONG)wcslen(wbuf) + 1,
//                            mbBuf,
//                            sizeof(mbBuf),
//                            NULL,
//                            NULL);
//
//                        sstr << "  (" << mbBuf << " )";
//                    }
//                    else
//                        sstr << "  ( read string error )";
//                }
//                else
//                {
//                    sstr << dec << " ( " << val << " )";
//                }   
//            }
//        }
//        else
//        {
//            for ( size_t i = 0; i <  field.size/field.type.size(); ++i )
//            {
//                unsigned __int64  val = boost::python::extract<unsigned __int64>( attr[i] );
//            
//                sstr << "\n\t\t\t[" << i << "]  " << hex << "0x" << val << dec << " ( " << val << " )";
//            }
//        }
//    }
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//
