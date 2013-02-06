#include "stdafx.h"

#include <iomanip>

#include "typedvar.h"
#include "module.h"
#include "dbgmem.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

TypedVarPtr   TypedVar::getTypedVar( const TypeInfoPtr& typeInfo, VarDataPtr varData )
{
    if ( !typeInfo )
        throw DbgException( "typeInfo can not be None" );

    TypedVarPtr     tv;

    if ( typeInfo->isBasicType() )
    {
        tv.reset( new BasicTypedVar( typeInfo, varData) );
        return tv;
    }

    if ( typeInfo->isPointer() )
    {
        tv.reset( new PtrTypedVar(  typeInfo, varData ) );
        return tv;
    }

    if ( typeInfo->isArray() )
    {
        tv.reset( new ArrayTypedVar(  typeInfo, varData ) );
        return tv;
    }

    if ( typeInfo->isUserDefined() )
    {
        tv.reset( new UdtTypedVar( typeInfo, varData ) );
        return tv;
    }

    if ( typeInfo->isBitField() )
    {
        tv.reset( new BitFieldVar( typeInfo, varData ) );
        return tv;
    }

    if ( typeInfo->isEnum() )
    {
        tv.reset( new EnumTypedVar( typeInfo, varData ) );
        return tv;
    }

    throw DbgException( "can not create typedVar for this type" );

    return tv;
}

///////////////////////////////////////////////////////////////////////////////////

TypedVarPtr TypedVar::getTypedVarByName( const std::string &varName )
{
    std::string     moduleName;
    std::string     symName;

    splitSymName( varName, moduleName, symName );

    ModulePtr   module = Module::loadModuleByName( moduleName );

    return module->getTypedVarByName( symName );
}

///////////////////////////////////////////////////////////////////////////////////

TypedVarPtr TypedVar::getTypedVarByTypeName( const std::string &typeName, ULONG64 addr )
{
    addr = addr64( addr );

    std::string     moduleName;
    std::string     symName;

    splitSymName( typeName, moduleName, symName );

    ModulePtr   module = Module::loadModuleByName( moduleName );

    return module->getTypedVarByTypeName( symName, addr );
}

///////////////////////////////////////////////////////////////////////////////////

TypedVarPtr TypedVar::getTypedVarByTypeInfo( const TypeInfoPtr &typeInfo, ULONG64 addr )
{
    return getTypedVar( typeInfo, VarDataMemory::factory(addr) );
}

///////////////////////////////////////////////////////////////////////////////////

TypedVar::TypedVar ( const TypeInfoPtr& typeInfo, VarDataPtr varData ) :
    m_typeInfo( typeInfo ),
    m_varData( varData ),
    m_dataKind( DataIsGlobal )
{
    m_size = m_typeInfo->getSize();
}
///////////////////////////////////////////////////////////////////////////////////

BaseTypeVariant BasicTypedVar::getValue()
{
    ULONG64     val = 0;
    m_varData->read(&val, getSize());

    if ( m_typeInfo->getName() == "Char" )
        return (LONG)*(PCHAR)&val;

    if ( m_typeInfo->getName() == "WChar" )
        return (LONG)*(PWCHAR)&val;

    if ( m_typeInfo->getName() == "Int1B" )
        return (LONG)*(PCHAR)&val;

    if ( m_typeInfo->getName() == "UInt1B" )
        return (ULONG)*(PUCHAR)&val;

    if ( m_typeInfo->getName() == "Int2B" )
        return (LONG)*(PSHORT)&val;

    if ( m_typeInfo->getName() == "UInt2B" )
        return (ULONG)*(PUSHORT)&val;

    if ( m_typeInfo->getName() == "Int4B" )
        return *(PLONG)&val;

    if ( m_typeInfo->getName() == "UInt4B" )
        return *(PULONG)&val;

    if ( m_typeInfo->getName() == "Int8B" )
        return (LONG64)*(PLONG64)&val;

    if ( m_typeInfo->getName() == "UInt8B" )
        return (ULONG64)*(PULONG64)&val;

    if ( m_typeInfo->getName() == "Long" )
        return *(PLONG)&val;

    if ( m_typeInfo->getName() == "ULong" )
        return *(PULONG)&val;

    if ( m_typeInfo->getName() == "Bool" )
        return *(bool*)&val;
    
    throw DbgException( "failed get value " );
}

///////////////////////////////////////////////////////////////////////////////////

std::string BasicTypedVar::print()
{
    std::stringstream       sstr;

    sstr << m_typeInfo->getName() << " " << m_varData->asString();
    sstr << " Value: " << printValue();

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////////

std::string  BasicTypedVar::printValue()
{
    std::stringstream  sstr;

    try {
    
        sstr << boost::apply_visitor( VariantToHex(), getValue() );
        sstr << " (" << boost::apply_visitor( VariantToStr(), getValue() ) << ")";

    } 
    catch( MemoryException& )
    {
        sstr << "????";
    }

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////////

BaseTypeVariant PtrTypedVar::getValue()
{
    return m_varData->readPtr( getSize() );
}

///////////////////////////////////////////////////////////////////////////////////

TypedVarPtr PtrTypedVar::deref()
{
    VarDataPtr varData = VarDataMemory::factory( m_varData->readPtr( getSize() ) );
    return TypedVar::getTypedVar( m_typeInfo->deref(), varData );
}

///////////////////////////////////////////////////////////////////////////////////

TypedVarPtr PtrTypedVar::getField( const std::string &fieldName )
{
    TypedVarPtr  derefPtr = deref();
    return derefPtr->getField( fieldName );
}

///////////////////////////////////////////////////////////////////////////////////

std::string PtrTypedVar::print()
{
    std::stringstream   sstr;

    sstr << "Ptr " << m_typeInfo->getName() << " " << m_varData->asString();
    sstr << " Value: " << printValue();

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////////

std::string  PtrTypedVar::printValue()
{
    std::stringstream   sstr;

    try {

        sstr << boost::apply_visitor( VariantToHex(), getValue() );

    }
    catch( MemoryException& )
    {
        sstr << "????";
    }

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////////

std::string ArrayTypedVar::print()
{
    std::stringstream   sstr;

    sstr << m_typeInfo->getName() << " " << m_varData->asString();

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////////

std::string  ArrayTypedVar::printValue()
{
    return "";
}

///////////////////////////////////////////////////////////////////////////////////

python::object ArrayTypedVar::getElementByIndex( ULONG  index ) 
{
    if ( index >= m_typeInfo->getCount() )
        throw PyException( PyExc_IndexError, "Index out of range" );

    TypeInfoPtr     elementType = m_typeInfo->getElementType();

    return python::object( TypedVar::getTypedVar( elementType, m_varData->fork(elementType->getSize()*index) ) );
}

///////////////////////////////////////////////////////////////////////////////////

TypedVarPtr
UdtTypedVar::getField( const std::string &fieldName ) 
{
    TypeInfoPtr fieldType = m_typeInfo->getField( fieldName );

    if ( m_typeInfo->isStaticMember(fieldName) )
    {
        ULONG64  staticOffset = m_typeInfo->getStaticOffsetByName( fieldName );

        if ( staticOffset == 0 )
            throw ImplementException( __FILE__, __LINE__, "Fix ME");

        return  TypedVar::getTypedVar( fieldType, VarDataMemory::factory(staticOffset) );
    }

    ULONG   fieldOffset = 0;

    fieldOffset = m_typeInfo->getFieldOffsetByNameRecursive(fieldName);

    if ( m_typeInfo->isVirtualMember( fieldName ) )
    {
        fieldOffset += getVirtualBaseDisplacement( fieldName );
    }

    return  TypedVar::getTypedVar( fieldType, m_varData->fork(fieldOffset) );
}

///////////////////////////////////////////////////////////////////////////////////

python::object
UdtTypedVar::getElementByIndex( ULONG  index )
{
    TypeInfoPtr fieldType = m_typeInfo->getFieldByIndex( index );

    if ( m_typeInfo->isStaticMemberByIndex(index) )
    {
        ULONG64  staticOffset = m_typeInfo->getStaticOffsetByIndex( index );

        if ( staticOffset == 0 )
            throw ImplementException( __FILE__, __LINE__, "Fix ME");

        return python::make_tuple( 
            m_typeInfo->getFieldNameByIndex(index), 
            TypedVar::getTypedVar( fieldType, VarDataMemory::factory(staticOffset) ) );
    }

    ULONG   fieldOffset = 0;

    fieldOffset = m_typeInfo->getFieldOffsetByIndex(index);

    if ( m_typeInfo->isVirtualMemberByIndex( index ) )
    {
        fieldOffset += getVirtualBaseDisplacementByIndex( index );
    }

    return python::make_tuple( 
            m_typeInfo->getFieldNameByIndex(index), 
            TypedVar::getTypedVar( fieldType, m_varData->fork(fieldOffset) ) );
}

///////////////////////////////////////////////////////////////////////////////////

LONG UdtTypedVar::getVirtualBaseDisplacement( const std::string &fieldName )
{
    ULONG virtualBasePtr, virtualDispIndex, virtualDispSize;
    m_typeInfo->getVirtualDisplacement( fieldName, virtualBasePtr, virtualDispIndex, virtualDispSize );

    ULONG64     vbtableOffset = m_varData->fork( virtualBasePtr )->readPtr( m_typeInfo->ptrSize() );

    VarDataPtr   vbtable = VarDataMemory::factory(vbtableOffset);

    LONG   displacement = 0;

    vbtable->read( &displacement, sizeof(displacement), virtualDispIndex*virtualDispSize );

    return virtualBasePtr + displacement;
}

///////////////////////////////////////////////////////////////////////////////

LONG UdtTypedVar::getVirtualBaseDisplacementByIndex( ULONG index )
{
    ULONG virtualBasePtr, virtualDispIndex, virtualDispSize;
    m_typeInfo->getVirtualDisplacementByIndex( index, virtualBasePtr, virtualDispIndex, virtualDispSize );

    ULONG64     vbtableOffset = m_varData->fork( virtualBasePtr )->readPtr( m_typeInfo->ptrSize() );

    VarDataPtr   vbtable = VarDataMemory::factory(vbtableOffset);

    LONG   displacement = 0;

    vbtable->read( &displacement, sizeof(displacement), virtualDispIndex*virtualDispSize );

    return virtualBasePtr + displacement;
}

///////////////////////////////////////////////////////////////////////////////////

std::string UdtTypedVar::print()
{
    std::stringstream  sstr;

    sstr << "struct/class: " << m_typeInfo->getName() << " " << m_varData->asString() << std::endl;
    
    for ( ULONG i = 0; i < m_typeInfo->getFieldCount(); ++i )
    {
        TypeInfoPtr     fieldType = m_typeInfo->getFieldByIndex(i);
        TypedVarPtr     fieldVar;

        if ( m_typeInfo->isStaticMemberByIndex(i) )
        {
            ULONG64  staticOffset = m_typeInfo->getStaticOffsetByIndex(i);

            if ( staticOffset != 0 )
                fieldVar = TypedVar::getTypedVar( fieldType, VarDataMemory::factory( staticOffset ) );

            sstr << "   =" << std::right << std::setw(10) << std::setfill('0') << std::hex << staticOffset;
            sstr << " " << std::left << std::setw(18) << std::setfill(' ') << m_typeInfo->getFieldNameByIndex(i) << ':';
        }
        else
        {
            ULONG   fieldOffset = m_typeInfo->getFieldOffsetByIndex(i);

            if ( m_typeInfo->isVirtualMemberByIndex( i ) )
            {
                fieldOffset += getVirtualBaseDisplacementByIndex( i );
            }

            fieldVar = TypedVar::getTypedVar( fieldType, m_varData->fork(fieldOffset) );
            sstr << "   +" << std::right << std::setw(4) << std::setfill('0') << std::hex << fieldOffset;
            sstr << " " << std::left << std::setw(24) << std::setfill(' ') << m_typeInfo->getFieldNameByIndex(i) << ':';
        }

        sstr << " " << std::left << fieldType->getName();

        if ( fieldVar )
            sstr << "   " << fieldVar->printValue();
        else
            sstr << "   failed to get value";

        sstr << std::endl;
    }

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////////

std::string  UdtTypedVar::printValue()
{
    return "";        
}

///////////////////////////////////////////////////////////////////////////////////

BaseTypeVariant BitFieldVar::getValue()
{
    ULONG64     val = 0;

    m_varData->read( &val, getSize() );

    val >>= m_typeInfo->getBitOffset();
    val &= ( 1 << m_typeInfo->getBitWidth() ) - 1;

    switch ( m_typeInfo->getSize() )
    {
    case 1:
        return (ULONG)*(PUCHAR)&val;

    case 2:
        return (ULONG)*(PUSHORT)&val;

    case 4:
        return *(PULONG)&val;

    case 8:
        return *(PULONG64)&val;
    }

    throw DbgException( "failed get value " );
}


///////////////////////////////////////////////////////////////////////////////////

std::string  BitFieldVar::printValue()
{
    std::stringstream       sstr;

    try
    {   
        sstr << boost::apply_visitor( VariantToHex(), getValue() );
        sstr << " (" << boost::apply_visitor( VariantToStr(), getValue() ) << ")";
    }
    catch( MemoryException& )
    {
        sstr << "????";
    }

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////////

BaseTypeVariant EnumTypedVar::getValue()
{
    ULONG       val = 0;

    m_varData->read( &val, getSize() );

    return val;
};

///////////////////////////////////////////////////////////////////////////////////

std::string EnumTypedVar::print()
{
    std::stringstream       sstr;

    sstr << "enum: " << m_typeInfo->getName() << " " << m_varData->asString();
    sstr << " Value: " << printValue();

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////////

std::string EnumTypedVar::printValue()
{  
    std::stringstream   sstr;    

    try {

        ULONG       val = boost::apply_visitor( VariantToULong(), getValue() );

        for ( ULONG i = 0; i < m_typeInfo->getFieldCount(); ++i )
        {
           ULONG       val1 = boost::apply_visitor( VariantToULong(), m_typeInfo->getFieldByIndex(i)->getValue() );

           if ( val == val1 )
           {
               sstr << m_typeInfo->getFieldNameByIndex(i);
               sstr << "(0x" << std::hex << val << ")";

               return sstr.str();
           }
        }

        sstr << "0x" << std::hex << val;
        sstr << " ( No matching name )";

    }
    catch( MemoryException& )
    {
        sstr << "????";
    }

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////////

TypedVarPtr containingRecordByName( ULONG64 offset, const std::string &typeName, const std::string &fieldName )
{
    std::string     moduleName;
    std::string     symName;

    splitSymName( typeName, moduleName, symName );

    ModulePtr   module = Module::loadModuleByName( moduleName );

    return module->containingRecordByName( offset, symName, fieldName );
}

///////////////////////////////////////////////////////////////////////////////////

TypedVarPtr containingRecordByType( ULONG64 addr, const TypeInfoPtr &typeInfo, const std::string &fieldName )
{
    if ( !typeInfo )
        throw DbgException( "typeInfo can not be None" );

    addr = addr64(addr); 

    VarDataPtr varData = VarDataMemory::factory( addr - typeInfo->getFieldOffsetByNameRecursive(fieldName) );

    return TypedVar::getTypedVar( typeInfo, varData );
}

///////////////////////////////////////////////////////////////////////////////////

python::list getTypedVarListByTypeName( ULONG64 listHeadAddress, const std::string &typeName, const std::string &listEntryName )
{
    std::string     moduleName;
    std::string     symName;

    splitSymName( typeName, moduleName, symName );

    ModulePtr   module = Module::loadModuleByName( moduleName );

    return module->getTypedVarListByTypeName( listHeadAddress, symName, listEntryName );
}

///////////////////////////////////////////////////////////////////////////////////

python::list getTypedVarListByType( ULONG64 listHeadAddress, const TypeInfoPtr &typeInfo, const std::string &listEntryName )
{
    if ( !typeInfo )
        throw DbgException( "typeInfo can not be None" );

    python::list    lst;

    listHeadAddress = addr64( listHeadAddress );

    ULONG64                 entryAddress = 0;

    TypeInfoPtr             fieldTypeInfo = typeInfo->getFieldRecursive( listEntryName );

    ULONG64 (*ptrFunc)(ULONG64) = fieldTypeInfo->ptrSize() == 4 ? &ptrDWord : &ptrQWord;

    if ( fieldTypeInfo->getName() == ( typeInfo->getName() + "*" ) )
    {
        for( entryAddress = ptrFunc( listHeadAddress ); addr64(entryAddress) != listHeadAddress && entryAddress != NULL; entryAddress = ptrFunc( entryAddress + typeInfo->getFieldOffsetByNameRecursive(listEntryName) ) )
            lst.append( TypedVar::getTypedVarByTypeInfo( typeInfo, entryAddress ) );
    }
    else
    {
        for( entryAddress = ptrFunc( listHeadAddress ); addr64(entryAddress) != listHeadAddress && entryAddress != NULL; entryAddress = ptrFunc( entryAddress ) )
            lst.append( containingRecordByType( entryAddress, typeInfo, listEntryName ) );
    }

    return lst;
}

///////////////////////////////////////////////////////////////////////////////////

python::list getTypedVarArrayByTypeName( ULONG64 addr, const std::string  &typeName, ULONG number )
{
    std::string     moduleName;
    std::string     symName;

    splitSymName( typeName, moduleName, symName );

    ModulePtr   module = Module::loadModuleByName( moduleName );

    return module->getTypedVarArrayByTypeName( addr, symName, number );
}

///////////////////////////////////////////////////////////////////////////////////

python::list getTypedVarArrayByType( ULONG64 offset, const TypeInfoPtr &typeInfo, ULONG number )
{
    if ( !typeInfo )
        throw DbgException( "typeInfo can not be None" );

    offset = addr64(offset); 
       
    python::list     lst;
    
    for( ULONG i = 0; i < number; ++i )
        lst.append( TypedVar::getTypedVarByTypeInfo( typeInfo, offset + i * typeInfo->getSize() ) );
   
    return lst;
}

///////////////////////////////////////////////////////////////////////////////////


} // end pykd namespace