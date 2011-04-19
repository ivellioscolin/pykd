#include "stdafx.h"

#include <sstream>

#include "dbgext.h"
#include "dbgtype.h"
#include "dbgexcept.h"
#include "dbgmem.h"
#include "dbgsystem.h"
#include "dbgmem.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadTypedVar( const std::string &moduleName, const std::string &typeName, ULONG64 address )
{
    const TypeInfo        *typeInfo = TypeInfo::get( moduleName, typeName );

    return typeInfo != NULL ? typeInfo->load( address ) : boost::python::object();
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG
sizeofType( const std::string &moduleName, const std::string &typeName )
{
    HRESULT     hres;
    ULONG       typeSize = ~0;

    try {

        ULONG64         moduleBase;

        hres = dbgExt->symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &moduleBase );
        if ( FAILED( hres ) )
            throw  DbgException( "IDebugSymbol::GetModuleByModuleName  failed" ); 
    
        const TypeInfo        *typeInfo = TypeInfo::get( moduleName, typeName );
        
        return typeInfo != NULL ? (ULONG)typeInfo->size() : 0L;
    }
    catch( std::exception  &e )
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
    }
    catch(...)
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
    }

    return typeSize;
}

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
containingRecord( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &fieldName )
{
    HRESULT      hres;

    try {
        
        ULONG64         moduleBase;

        hres = dbgExt->symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &moduleBase );
        if ( FAILED( hres ) )
            throw  DbgException( "IDebugSymbol::GetModuleByModuleName  failed" ); 

        ULONG        typeId;
        hres = dbgExt->symbols->GetTypeId( moduleBase, typeName.c_str(), &typeId );
        if ( FAILED( hres ) )
            throw  DbgException( "IDebugSymbol::GetTypeId  failed" );

        ULONG       fieldTypeId;
        ULONG       fieldOffset;
        hres = dbgExt->symbols3->GetFieldTypeAndOffset( moduleBase, typeId, fieldName.c_str(), &fieldTypeId, &fieldOffset );   
        
        return loadTypedVar( moduleName, typeName, address - fieldOffset );
    }
    catch( std::exception  &e )
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
    }
    catch(...)
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
    }

    return boost::python::object();
}

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
getTypeClass( const std::string &moduleName, const std::string &typeName )
{
    try
    {
        const TypeInfo        *typeInfo = TypeInfo::get( moduleName, typeName );

        return typeInfo != NULL ? typeInfo->build() : boost::python::object();
    }
    catch( std::exception  &e )
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
    }
    catch(...)
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
    }
    
    return boost::python::object();
}

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadTypedVarList( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &listEntryName )
{
    address = addr64(address); 
    
    ULONG64                 entryAddress = 0;

    const TypeInfo          *typeInfo = TypeInfo::get( moduleName, typeName );
    
    if ( !typeInfo )
        return boost::python::object();    
    
    boost::python::list     objList;
    
    for ( TypeInfo::TypeFieldList::const_iterator  field = typeInfo->getFields().begin(); field != typeInfo->getFields().end(); field++ )
    {
        if ( field->name == listEntryName )
        {
            if ( field->type.name() == ( typeName + "*" ) )
            {
                for( entryAddress = loadPtrByPtr( address ); entryAddress != address && entryAddress != NULL; entryAddress = loadPtrByPtr( entryAddress + field->offset  ) )
                    objList.append( loadTypedVar( moduleName, typeName, entryAddress ) );
            }                
            else
            {
                for( entryAddress = loadPtrByPtr( address ); entryAddress != address && entryAddress != NULL; entryAddress = loadPtrByPtr( entryAddress ) )
                    objList.append( containingRecord( entryAddress, moduleName, typeName, listEntryName ) );                  
            }
            
            return objList;   
        }
    }
    
    return boost::python::object();
}

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadTypedVarArray( ULONG64 address, const std::string &moduleName, const std::string &typeName, long number )
{
    boost::python::list     objList;
    
    const TypeInfo          *typeInfo = TypeInfo::get( moduleName, typeName );
    
    if ( !typeInfo )
        return boost::python::object();        
    
    for( long i = 0; i < number; ++i )
        objList.append( loadTypedVar( moduleName, typeName, address + i * typeInfo->size() ) );
    
    return objList;
}

/////////////////////////////////////////////////////////////////////////////////

bool
isBaseType( const std::string  &typeName );

template< typename valType>
boost::python::object
valueLoader( PVOID address, ULONG size );

template<>
boost::python::object
valueLoader<void*>( PVOID address, ULONG size )
{
    if ( is64bitSystem() )
        return valueLoader<__int64>( address, size );
    else
        return valueLoader<long>( address, size );
}

boost::python::object
voidLoader( PVOID address, ULONG size ) {
    return boost::python::object();
}

static const char*   
basicTypeNames[] = {
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

typedef
boost::python::object
(*basicTypeLoader)( PVOID address, ULONG size );
    
basicTypeLoader     basicTypeLoaders[] = {
    valueLoader<unsigned char>,
    valueLoader<char>,
    valueLoader<unsigned short>,
    valueLoader<short>,
    valueLoader<unsigned long>,
    valueLoader<long>,
    valueLoader<int>,
    valueLoader<unsigned int>,
    valueLoader<void*>,
    voidLoader,
    valueLoader<double>,
    valueLoader<__int64>,
    valueLoader<unsigned __int64>
 };
 
size_t   basicTypeSizes[] = {
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
     
/////////////////////////////////////////////////////////////////////////////////

TypeInfo::TypeInfoMap    TypeInfo::g_typeInfoCache;

const TypeInfo*
TypeInfo::get( const std::string  &moduleName, const std::string  &typeName )
{   
    HRESULT      hres;

    try {
    
        TypeInfoMap::iterator     findIt = g_typeInfoCache.find( TypeName( moduleName, typeName ) );

        if ( findIt != g_typeInfoCache.end() )
            return  &findIt->second;        
            
        TypeInfo        typeInfo( typeName );    
        
        do {  
    
            if (  typeName.find("*") < typeName.size() )
            {
                typeInfo.m_pointer = true;
                typeInfo.m_size = ptrSize();
                break;
            }

            typeInfo.m_baseType = isBaseType( typeName );
            if ( typeInfo.m_baseType )
            {
                typeInfo.setupBaseType();
                break;
            }

            ULONG64     moduleBase = 0;
            hres = dbgExt->symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &moduleBase );
            if ( FAILED( hres ) )
                throw  DbgException( "IDebugSymbol::GetModuleByModuleName  failed" ); 

            ULONG       typeId = 0;
            hres = dbgExt->symbols->GetTypeId( moduleBase, typeInfo.m_typeName.c_str(), &typeId );
            if ( FAILED( hres ) )
                throw  DbgException( "IDebugSymbol::GetTypeId  failed" );

            hres = dbgExt->symbols->GetTypeSize( moduleBase, typeId, &typeInfo.m_size );
            if ( FAILED( hres ) )
                throw DbgException( "IDebugSymbol::GetTypeSize failed" );
                
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
                    throw  DbgException( "IDebugSymbol3::GetFieldTypeAndOffset  failed" ); 

                ULONG   fieldSize;                
                hres = dbgExt->symbols->GetTypeSize( moduleBase, fieldTypeId, &fieldSize );
                if ( FAILED( hres ) )
                    throw DbgException( "IDebugSymbol::GetTypeSize failed" );

                char    fieldTypeName[100];
                hres = dbgExt->symbols->GetTypeName( moduleBase, fieldTypeId, fieldTypeName, sizeof(fieldTypeName), NULL );

                std::string     fieldTypeNameStr( fieldTypeName );
                
                if ( fieldTypeNameStr == "__unnamed" 
                 ||  fieldTypeNameStr.find("<unnamed-tag>") < fieldTypeNameStr.size() )
                {
                    TypeInfo   unnamedType;
                    if ( !getById( moduleName, fieldTypeId, unnamedType ) )
                        return NULL;
                        
                     typeInfo.m_fields.push_back( TypeField( fieldName, unnamedType, fieldSize, fieldOffset ) );     
                }
                else
                {
                    const TypeInfo  *fieldTypeInfo = get( moduleName, fieldTypeName );
                    if ( !fieldTypeInfo )
                        return NULL;
                        
                    typeInfo.m_fields.push_back( TypeField( fieldName, *fieldTypeInfo, fieldSize, fieldOffset ) );                         
                }   
           }
           
        } while( FALSE );          
       
        return &g_typeInfoCache.insert( std::make_pair( TypeName( moduleName, typeName ), typeInfo) ).first->second;
    }
    catch( std::exception& )
    {
        //dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
        // это нормально: на вход был передан не верный тип
    }
    catch(...)
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
    }    
    
    return NULL;    
}

/////////////////////////////////////////////////////////////////////////////////

bool
TypeInfo::getById( const std::string  &moduleName, ULONG typeId, TypeInfo& typeInfo )
{
    HRESULT      hres;
    
    try {

        ULONG64     moduleBase = 0;
        hres = dbgExt->symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &moduleBase );
        if ( FAILED( hres ) )
            throw  DbgException( "IDebugSymbol::GetModuleByModuleName  failed" ); 

        hres = dbgExt->symbols->GetTypeSize( moduleBase, typeId, &typeInfo.m_size );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbol::GetTypeSize failed" );
            
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
                throw  DbgException( "IDebugSymbol3::GetFieldTypeAndOffset  failed" ); 

            ULONG   fieldSize;                
            hres = dbgExt->symbols->GetTypeSize( moduleBase, fieldTypeId, &fieldSize );
            if ( FAILED( hres ) )
                throw DbgException( "IDebugSymbol::GetTypeSize failed" );

            char    fieldTypeName[100];
            hres = dbgExt->symbols->GetTypeName( moduleBase, fieldTypeId, fieldTypeName, sizeof(fieldTypeName), NULL );

            std::string     fieldTypeNameStr( fieldTypeName );

            if ( fieldTypeNameStr == "__unnamed" 
             ||  fieldTypeNameStr.find("<unnamed-tag>") < fieldTypeNameStr.size() )
            {
                TypeInfo   unnamedType;
                if ( !getById( moduleName, fieldTypeId, unnamedType ) )
                    return false;
                    
                 typeInfo.m_fields.push_back( TypeField( fieldName, unnamedType, fieldSize, fieldOffset ) );     
            }
            else
            {
                const TypeInfo      *fieldTypeInfo = get( moduleName, fieldTypeName );
                if ( !fieldTypeInfo )
                    return false;
                    
                typeInfo.m_fields.push_back( TypeField( fieldName, *fieldTypeInfo, fieldSize, fieldOffset ) );                         
            }   
       }
       
       return true;
    }
    catch( std::exception& )
    {
        //dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
        // это нормально: на вход был передан не верный тип
    }
    catch(...)
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
    }   
    
    return false; 
}

/////////////////////////////////////////////////////////////////////////////////

void
TypeInfo::setupBaseType()
{
   for ( int i = 0; i < sizeof( basicTypeSizes ) / sizeof( size_t ); ++i )
   {
        if ( m_typeName == basicTypeNames[i] ||
            m_typeName == ( std::string( basicTypeNames[i] ) + "[]" ) )
        {
            m_size = (ULONG)basicTypeSizes[i];
            return;   
        }
    }            
}

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
TypeInfo::build( ULONG offset /* = 0 */ ) const
{
    boost::shared_ptr<typeClass> ptr( new typeClass( *this, offset ) );
    boost::python::object var( ptr );
    ptr->setPyObj( var );

    TypeFieldList::const_iterator    field = m_fields.begin();
    for ( field = m_fields.begin(); field != m_fields.end(); ++field )
    {

        if ( field->size == field->type.size() )
        {
            var.attr( field->name.c_str() ) = 
                field->type.build( offset + field->offset );
        }
        else
        {
            boost::python::dict     arr;

            for ( unsigned int i = 0; i < field->size / field->type.size(); ++i )
            {
                const ULONG locOffset = field->offset + i * (ULONG)field->type.size();
                arr[i] = field->type.build( offset + locOffset );
            }

            var.attr( field->name.c_str() ) = arr;
        }
    }

    return var;
}

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
TypeInfo::load( ULONG64 targetAddr, PVOID cacheBuffer , ULONG offset /* = 0 */ ) const
{
    if ( !isOffsetValid( targetAddr) )
        return boost::python::object();
        
    boost::scoped_array<char>       rawBuffer;    
        
    if ( cacheBuffer == NULL )
    {
        rawBuffer.reset( new char[ m_size ] );
        cacheBuffer = rawBuffer.get();
        
        if ( !loadMemory( targetAddr, cacheBuffer, m_size ) )
            return boost::python::object();
    }           
    
    if ( m_pointer )
        return ptrLoader( (PVOID)( (ULONG_PTR)cacheBuffer + offset ) );
        
    if ( m_baseType )
        return loadBaseType( (PVOID)( (ULONG_PTR)cacheBuffer + offset ) );   

    boost::shared_ptr<typedVarClass>    ptr( new typedVarClass( *this, offset, targetAddr ) );
    boost::python::object               var( ptr );
    ptr->setPyObj( var );

    TypeFieldList::const_iterator    field = m_fields.begin();
    for ( field = m_fields.begin(); field != m_fields.end(); ++field )
    {

        if ( field->size == field->type.size() )
        {
            var.attr( field->name.c_str() ) = 
                field->type.load( targetAddr + field->offset, cacheBuffer, offset + field->offset );
        }
        else
        {
            boost::python::list     arr;

            for ( unsigned int i = 0; i < field->size / field->type.size(); ++i )
            {
                const ULONG locOffset = field->offset + i * (ULONG)field->type.size();
                arr.append( field->type.load( targetAddr + locOffset, cacheBuffer, offset + locOffset ) );
            }

            var.attr( field->name.c_str() ) = arr;
        }
    }

    return var;
}

/////////////////////////////////////////////////////////////////////////////////

bool
isBaseType( const std::string  &typeName )
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

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
TypeInfo::loadBaseType( PVOID address ) const
{
   for ( int i = 0; i < sizeof( basicTypeNames ) / sizeof( char* ); ++i )
   {
        if ( m_typeName == basicTypeNames[i] )
            return basicTypeLoaders[i]( address, m_size );

        if ( m_typeName == ( std::string( basicTypeNames[i] ) + "*" ) )
            return valueLoader<void*>( address, ptrSize() );

        if ( m_typeName == ( std::string( basicTypeNames[i] ) + "[]" ) )
            return basicTypeLoaders[i]( address, m_size ); 

        if ( m_typeName == ( std::string( basicTypeNames[i] ) + "*[]" ) )
             return valueLoader<void*>( address, ptrSize() );
    }

    return boost::python::object();
}

/////////////////////////////////////////////////////////////////////////////////

template< typename valType>
boost::python::object
valueLoader( PVOID address, ULONG size )
{
    if ( size == sizeof(valType) )
    {
        valType     v = *(valType*)address;
        return boost::python::long_( (unsigned __int64)v );
        
        
        //if ( loadMemory( address, &v, sizeof(v) ) )
        //{
        //    return boost::python::long_( (unsigned __int64)v );
        //}            
    }
    else
    {    
        boost::python::dict     arr;

        for ( unsigned int i = 0; i < size / sizeof(valType); ++i )
        {
            valType  v = *( (valType*)address + i );
            //if ( !loadMemory( address + i * sizeof(valType), &v, sizeof(v) ) )

            arr[i] = boost::python::long_( (unsigned __int64)v );    
        }

        return arr;
    }
    
    return boost::python::object();
}

/////////////////////////////////////////////////////////////////////////////////

std::string typeClass::print() const
{
    stringstream sstr;

    sstr << getTypeInfo().name() << "  ";
    printSelf(sstr);
    sstr << std::endl;

    TypeInfo::TypeFieldList::const_iterator itField = 
        getTypeInfo().getFields().begin();
    while (itField != getTypeInfo().getFields().end())
    {
        sstr << "\t" << hex << "+" << itField->offset << "  ";
        sstr << itField->type.name() << "  ";
        sstr << itField->name << "  ";

        printField(*itField, sstr);

        sstr << std::endl;

        ++itField;
    }

    return sstr.str();
}

/////////////////////////////////////////////////////////////////////////////////

void
typedVarClass::printField(const TypeInfo::TypeField &field, stringstream &sstr) const
{
    if ( field.type.isComplex() && !field.type.isPtr())
       sstr << field.type.name();
    else
    {
        boost::python::object     attr = getPyObj().attr( field.name.c_str() );

        if ( field.size == field.type.size() )
        {
            if ( attr.ptr() == Py_None )
            {
                sstr << "memory error";
            }
            else
            {
                unsigned __int64  val = boost::python::extract<unsigned __int64>( attr );

                sstr << hex << "0x" << val;

                if ( field.type.name() == "char*" )
                {
                    char  buf[0x100];
                    if ( loadCStrToBuffer( val, buf, sizeof(buf) ) )
                        sstr << "  (" << buf << " )";
                    else
                        sstr << "  ( read string error )";
                }
                else if ( field.type.name() == "unsigned short*" )
                {
                    wchar_t   wbuf[0x100];
                    if ( loadWStrToBuffer( val, wbuf, sizeof(wbuf) ) )
                    {
                        char  mbBuf[0x100] = { 0 };

                        WideCharToMultiByte(
                            CP_ACP,
                            0,
                            wbuf,
                            (ULONG)wcslen(wbuf) + 1,
                            mbBuf,
                            sizeof(mbBuf),
                            NULL,
                            NULL);

                        sstr << "  (" << mbBuf << " )";
                    }
                    else
                        sstr << "  ( read string error )";
                }
                else
                {
                    sstr << dec << " ( " << val << " )";
                }   
            }
        }
        else
        {
            for ( size_t i = 0; i <  field.size/field.type.size(); ++i )
            {
                unsigned __int64  val = boost::python::extract<unsigned __int64>( attr[i] );
            
                sstr << "\n\t\t\t[" << i << "]  " << hex << "0x" << val << dec << " ( " << val << " )";
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////


