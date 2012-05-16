#include "stdafx.h"

#include <iomanip>

#include "typeinfo.h"
#include "dbgclient.h"

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr  TypeInfo::getTypeInfoByName( const std::string &typeName )
{
    return g_dbgClient->getTypeInfoByName( typeName );
}

/////////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr  TypeInfo::getTypeInfo( pyDia::SymbolPtr &typeSym )
{
    const ULONG symTag = typeSym->getSymTag();
    switch( symTag )
    {
    case SymTagBaseType:
        return getBaseTypeInfo( typeSym );

    case SymTagUDT:
    case SymTagBaseClass:
        return TypeInfoPtr( new UdtTypeInfo( typeSym ) );

    case SymTagArrayType:
        return TypeInfoPtr( new ArrayTypeInfo( typeSym ) );

    case SymTagPointerType:   
        return TypeInfoPtr( new PointerTypeInfo( typeSym ) );

    case SymTagVTable:
        return TypeInfoPtr( new PointerTypeInfo( typeSym->getType() ) );

    case SymTagEnum:
        return TypeInfoPtr( new EnumTypeInfo( typeSym ) );

    case SymTagTypedef:
        return getTypeInfo( typeSym->getType() );
    }

    throw TypeException( typeSym->getName(), "this type is not supported" );
}

/////////////////////////////////////////////////////////////////////////////////////

BaseTypeVariant  TypeInfo::getValue() 
{
    if ( !m_constant )
        throw TypeException( getName(), "this type is not a constant and has not a value" );

    switch( m_constantValue.vt )
    {
    case VT_UI1:
        return (ULONG)m_constantValue.bVal;;

    case VT_I1:
        return (LONG)m_constantValue.cVal;

    case VT_UI2:
        return (ULONG)m_constantValue.uiVal;

    case VT_I2:
        return (LONG)m_constantValue.iVal;

    case VT_UI4:
        return (ULONG)m_constantValue.lVal;

    case VT_I4:
        return (LONG)m_constantValue.ulVal;

    case VT_UI8:
        return (ULONG64)m_constantValue.ullVal;

    case VT_I8:
        return (LONG64)m_constantValue.llVal;
    }

    throw TypeException( getName(), "Failed to convert constant type to any integer type" );
}

/////////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr  TypeInfo::getTypeInfo( pyDia::SymbolPtr &symScope, const std::string &symName )
{
    size_t pos = symName.find_first_of( "*[" );

    if ( pos == std::string::npos )
    {
        TypeInfoPtr    basePtr = getBaseTypeInfo( symName );
        if ( basePtr != 0 )
            return basePtr;

        return getTypeInfo( symScope, symScope->getChildByName( symName ) );
    }

    return  getComplexType( symScope, symName );
}

/////////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr TypeInfo::getTypeInfo( pyDia::SymbolPtr &symScope, pyDia::SymbolPtr &symChild )
{
    CComVariant constVal;

    pyDia::SymbolPtr symType = symChild;
    if ( symType->getSymTag() == SymTagData )
    {
        if ( symType->getLocType() == LocIsBitField )
        {
            return TypeInfoPtr( new BitFieldTypeInfo(symType) );
        }

        if ( symType->getDataKind() == DataIsConstant )
        {
            symType->getValue( constVal );
        }

        symType = symType->getType();
    }

    TypeInfoPtr ptr = getTypeInfo( symType );

    if ( constVal.vt != VT_EMPTY )
        ptr->setConstant( constVal );

    return ptr;
}

/////////////////////////////////////////////////////////////////////////////////////

static const boost::regex baseMatch("^(Char)|(WChar)|(Int1B)|(UInt1B)|(Int2B)|(UInt2B)|(Int4B)|(UInt4B)|(Int8B)|(UInt8B)|(Long)|(ULong)|(Float)|(Bool)|(Double)$" );

TypeInfoPtr 
TypeInfo::getBaseTypeInfo( const std::string &symName )
{
    boost::cmatch    baseMatchResult;

    if ( boost::regex_match( symName.c_str(), baseMatchResult, baseMatch ) )
    {
        if ( baseMatchResult[1].matched )
            return TypeInfoPtr( new TypeInfoWrapper<char>("Char") );

        if ( baseMatchResult[2].matched )
            return TypeInfoPtr( new TypeInfoWrapper<wchar_t>("WChar") );

        if ( baseMatchResult[3].matched )
            return TypeInfoPtr( new TypeInfoWrapper<char>("Int1B") );

        if ( baseMatchResult[4].matched )
            return TypeInfoPtr( new TypeInfoWrapper<unsigned char>("UInt1B") );

        if ( baseMatchResult[5].matched )
            return TypeInfoPtr( new TypeInfoWrapper<short>("Int2B") );

        if ( baseMatchResult[6].matched )
            return TypeInfoPtr( new TypeInfoWrapper<unsigned short>("UInt2B") );

        if ( baseMatchResult[7].matched )
            return TypeInfoPtr( new TypeInfoWrapper<long>("Int4B") );

        if ( baseMatchResult[8].matched )
            return TypeInfoPtr( new TypeInfoWrapper<unsigned long>("UInt4B") );

        if ( baseMatchResult[9].matched )
            return TypeInfoPtr( new TypeInfoWrapper<__int64>("Int8B") );

        if ( baseMatchResult[10].matched )
            return TypeInfoPtr( new TypeInfoWrapper<unsigned __int64>("UInt8B") );

        if ( baseMatchResult[11].matched )
            return TypeInfoPtr( new TypeInfoWrapper<long>("Long") );

        if ( baseMatchResult[12].matched )
            return TypeInfoPtr( new TypeInfoWrapper<unsigned long>("ULong") );

        if ( baseMatchResult[13].matched )
            return TypeInfoPtr( new TypeInfoWrapper<float>("Float") );

        if ( baseMatchResult[14].matched )
            return TypeInfoPtr( new TypeInfoWrapper<bool>("Bool") );

        if ( baseMatchResult[15].matched )
            return TypeInfoPtr( new TypeInfoWrapper<double>("Double") );
   }

    return TypeInfoPtr();
}

/////////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr 
TypeInfo::getBaseTypeInfo( pyDia::SymbolPtr &symbol )  
{
    std::string     symName = symbol->getBasicTypeName( symbol->getBaseType() );

    if ( symName == "Int" || symName == "UInt" ) 
    {
        std::stringstream   sstr;
        sstr << symName << symbol->getSize() << "B";

        return getBaseTypeInfo( sstr.str() );
    }

    if ( symName == "Float" && symbol->getSize() == 8  )
    {
        symName = "Double";
    }

    return getBaseTypeInfo( symName );
}

/////////////////////////////////////////////////////////////////////////////////////

BitFieldTypeInfo::BitFieldTypeInfo(  pyDia::SymbolPtr &symbol )
{
    m_bitWidth = (ULONG)symbol->getSize();
    m_bitPos = (ULONG)symbol->getBitPosition();

    TypeInfoPtr    typeInfo = TypeInfo::getBaseTypeInfo( symbol->getType() );

    m_size = (ULONG)typeInfo->getSize();

    std::stringstream   sstr;

    sstr << typeInfo->getName() << ":" << (ULONG)m_bitWidth;
    m_name = sstr.str();
}

///////////////////////////////////////////////////////////////////////////////////

PointerTypeInfo::PointerTypeInfo( pyDia::SymbolPtr &symbol  ) 
{
    pyDia::SymbolPtr pointTo = symbol->getType();
    try
    {
        m_derefType = TypeInfo::getTypeInfo( pointTo );
    }
    catch (const SymbolException &)
    {
        m_derefType.swap( TypeInfoPtr() );
    }
    if (!derefPossible())
    {
        // special cases:
        const ULONG symTag = pointTo->getSymTag();
        switch (symTag)
        {
        //  * pointer to function
        case SymTagFunctionType:
            m_derefName = "<function>";
            break;

        case SymTagBaseType:
            //  * pointer to Void
            if (btVoid == static_cast<BasicType>(pointTo->getBaseType()))
                m_derefName = "Void";
            break;

        case SymTagVTableShape:
            m_derefName = "VTable";
            break;
        }
    }
    m_size = (ULONG)symbol->getSize();
}

/////////////////////////////////////////////////////////////////////////////////////

PointerTypeInfo::PointerTypeInfo( pyDia::SymbolPtr &symScope, const std::string &symName ) 
{
    try
    {
        m_derefType = TypeInfo::getTypeInfo( symScope, symName );
    }
    catch (const SymbolException &)
    {
        m_derefType.swap( TypeInfoPtr() );
    }
    m_size = (symScope->getMachineType() == IMAGE_FILE_MACHINE_AMD64) ? 8 : 4;
}

/////////////////////////////////////////////////////////////////////////////////////

std::string PointerTypeInfo::getName()
{
    return getComplexName();
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG PointerTypeInfo::getSize()
{
    return m_size;
}

/////////////////////////////////////////////////////////////////////////////////////

ArrayTypeInfo::ArrayTypeInfo( pyDia::SymbolPtr &symbol  ) 
{
    m_derefType = TypeInfo::getTypeInfo( symbol->getType() );
    m_count = symbol->getCount();
}

/////////////////////////////////////////////////////////////////////////////////////

ArrayTypeInfo::ArrayTypeInfo( pyDia::SymbolPtr &symScope, const std::string &symName, ULONG count ) 
{
    m_derefType = TypeInfo::getTypeInfo( symScope, symName );
    m_count = count;
}

/////////////////////////////////////////////////////////////////////////////////////

std::string ArrayTypeInfo::getName()
{
    return getComplexName();
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG ArrayTypeInfo::getSize()
{
    return m_derefType->getSize() * m_count;
}

/////////////////////////////////////////////////////////////////////////////////////

std::string TypeInfo::getComplexName()
{
    std::string       name;
    TypeInfo          *typeInfo = this;

    std::string tiName;
    do {

        if ( typeInfo->isArray() )
        {
            std::vector<ULONG>  indices;

            do {
                indices.push_back( typeInfo->getCount() );
            }
            while( ( typeInfo = dynamic_cast<ArrayTypeInfo*>(typeInfo)->getDerefType().get() )->isArray() );

            if ( !name.empty() )
            {
                name.insert( 0, 1, '(' );    
                name.insert( name.size(), 1, ')' );
            }

            std::stringstream       sstr;

            for ( std::vector<ULONG>::iterator  it = indices.begin(); it != indices.end(); ++it )
                sstr << '[' << *it << ']';

            name += sstr.str();

            continue;
        }
        else
        if ( typeInfo->isPointer() )
        {
            name.insert( 0, 1, '*' );

            PointerTypeInfo *ptrTypeInfo = dynamic_cast<PointerTypeInfo*>(typeInfo);
            if (!ptrTypeInfo->derefPossible())
            {
                tiName = ptrTypeInfo->getDerefName();
                break;
            }

            typeInfo = ptrTypeInfo->getDerefType().get();

            continue;
        }

        tiName = typeInfo->getName();
        break;

    } while ( true );

    name.insert( 0, tiName );

    return name;
}

/////////////////////////////////////////////////////////////////////////////////////

static const boost::regex bracketMatch("^([^\\(]*)\\((.*)\\)([^\\)]*)$"); 

static const boost::regex typeMatch("^([^\\(\\)\\*\\[\\]]*)([\\(\\)\\*\\[\\]\\d]*)$"); 

static const boost::regex ptrMatch("^\\*(.*)$");

static const boost::regex arrayMatch("^(.*)\\[(\\d+)\\]$");

static const boost::regex symbolMatch("^([\\*]*)([^\\(\\)\\*\\[\\]]*)([\\(\\)\\*\\[\\]\\d]*)$");

TypeInfoPtr TypeInfo::getComplexType( pyDia::SymbolPtr &symScope, const std::string &symName )
{
    ULONG  ptrSize = (symScope->getMachineType() == IMAGE_FILE_MACHINE_AMD64) ? 8 : 4;

    boost::cmatch    matchResult;

    if ( !boost::regex_match( symName.c_str(), matchResult, symbolMatch ) )
        throw TypeException( symName, "symbol name is invalid" );

    std::string  innerSymName = std::string( matchResult[2].first, matchResult[2].second );

    TypeInfoPtr    basePtr = getBaseTypeInfo( innerSymName );
    if ( basePtr != 0 )
    {
        return getRecurciveComplexType( basePtr, std::string( matchResult[3].first, matchResult[3].second ), ptrSize );
    }
            
    pyDia::SymbolPtr lowestSymbol = symScope->getChildByName( innerSymName );

    if ( lowestSymbol->getSymTag() == SymTagData )
    {
        throw TypeException( symName, "symbol name can not be an expresion" );
    }
   
    return getRecurciveComplexType( getTypeInfo( lowestSymbol ), std::string( matchResult[3].first, matchResult[3].second ), ptrSize );
}

/////////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr TypeInfo::getRecurciveComplexType( TypeInfoPtr &lowestType, std::string &suffix, ULONG ptrSize )
{
    boost::cmatch    matchResult;

    std::string     bracketExpr;

    if ( boost::regex_match( suffix.c_str(), matchResult, bracketMatch  ) )
    {
        bracketExpr = std::string( matchResult[2].first, matchResult[2].second );
        
        suffix = "";

        if ( matchResult[1].matched )
            suffix += std::string( matchResult[1].first, matchResult[1].second );

        if ( matchResult[3].matched )
            suffix += std::string( matchResult[3].first, matchResult[3].second );        
    }

    while( !suffix.empty() )
    {
        if ( boost::regex_match( suffix.c_str(), matchResult, ptrMatch  ) )
        {
            lowestType = TypeInfoPtr( new PointerTypeInfo( lowestType, ptrSize ) );
            suffix = std::string(matchResult[1].first, matchResult[1].second );
            continue;
        }

        if ( boost::regex_match( suffix.c_str(), matchResult, arrayMatch  ) )
        {
            lowestType = TypeInfoPtr( new ArrayTypeInfo( lowestType, std::atoi( matchResult[2].first ) ) );
            suffix = std::string(matchResult[1].first, matchResult[1].second );
            continue;
        }
    }
    
    if ( !bracketExpr.empty() )
        return getRecurciveComplexType( lowestType, bracketExpr, ptrSize );

    return lowestType;
}

/////////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr UdtTypeInfo::getField( const std::string &fieldName )
{
    if ( m_fields.empty() )
        getFields( m_dia );

    FieldList::reverse_iterator  it;
        
    it = std::find_if( m_fields.rbegin(), m_fields.rend(), boost::bind( &FieldType::first, _1) == fieldName );

    if ( it == m_fields.rend() )
        throw TypeException( m_dia->getName(), fieldName + ": unknown field type" );

    return it->second;
}

/////////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr UdtTypeInfo::getFieldByIndex( ULONG index )
{
    if ( m_fields.empty() )
        getFields( m_dia );

    return m_fields[ index ].second;
}

/////////////////////////////////////////////////////////////////////////////////////

std::string UdtTypeInfo::getFieldNameByIndex( ULONG index )
{
    if ( m_fields.empty() )
        getFields( m_dia);

    return m_fields[ index ].first;
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG UdtTypeInfo::getFieldCount()
{
    if ( m_fields.empty() )
        getFields( m_dia );

    return (ULONG)m_fields.size();
}

/////////////////////////////////////////////////////////////////////////////////////

void UdtTypeInfo::getFields( pyDia::SymbolPtr &rootSym, ULONG startOffset )
{
    ULONG   childCount = rootSym->getChildCount();  

    for ( ULONG i = 0; i < childCount; ++i )
    {
        pyDia::SymbolPtr  childSym = rootSym->getChildByIndex( i );

        ULONG  symTag = childSym->getSymTag();

        if ( symTag == SymTagBaseClass )
        {
            getFields( childSym, startOffset + childSym->getOffset() );
        }
        else
        if ( symTag == SymTagData )
        {
            TypeInfoPtr     ti = TypeInfo::getTypeInfo( m_dia, childSym );

            switch ( childSym->getDataKind() )
            {
            case DataIsMember:
                ti->setOffset( startOffset + childSym->getOffset() );
                break;

            case DataIsStaticMember:
                ti->setStaticOffset( childSym->getVa() );
                break;                
            }

            m_fields.push_back( std::make_pair( childSym->getName(), ti ) );
        }
        else
        if ( symTag == SymTagVTable )
        {
            TypeInfoPtr     ti = TypeInfo::getTypeInfo( m_dia, childSym );

            m_fields.push_back( std::make_pair( "__VFN_table", ti ) );            
        }
    }  
}

/////////////////////////////////////////////////////////////////////////////////////

std::string UdtTypeInfo::print()
{
    std::stringstream  sstr;

    sstr << "struct/class: " << getName() << " Size: 0x" << std::hex << getSize() << " (" << std::dec << getSize() << ")" << std::endl;
    
    ULONG       fieldCount = getFieldCount();

    for ( ULONG i = 0; i < fieldCount; ++i )
    { 
        TypeInfoPtr     fieldType = getFieldByIndex(i);

        if ( fieldType->isStaticMember() )
        {   
            sstr << "   =" << std::right << std::setw(10) << std::setfill('0') << std::hex << fieldType->getStaticOffset();
            sstr << " " << std::left << std::setw(18) << std::setfill(' ') << getFieldNameByIndex(i) << ':';
        }
        else
        {
            sstr << "   +" << std::right << std::setw(4) << std::setfill('0') << std::hex << fieldType->getOffset();
            sstr << " " << std::left << std::setw(24) << std::setfill(' ') << getFieldNameByIndex(i) << ':';
        }

        sstr << " " << std::left << fieldType->getName();
        sstr << std::endl;
    }

    return sstr.str();
}

/////////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr EnumTypeInfo::getField( const std::string &fieldName ) {
    pyDia::SymbolPtr  field = m_dia->getChildByName( fieldName );
    TypeInfoPtr  ti = TypeInfo::getTypeInfo( m_dia, fieldName );
    ti->setOffset( 0 );
    return ti;
}

/////////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr EnumTypeInfo::getFieldByIndex( ULONG index ) 
{
    if ( index >= m_dia->getChildCount() )
        throw TypeException( m_dia->getName(), ": field not found" );

    pyDia::SymbolPtr field = m_dia->getChildByIndex(index);

    if ( !field )
        throw TypeException( m_dia->getName(), ": field not found" );   
    
    TypeInfoPtr  ti = TypeInfo::getTypeInfo( m_dia, field->getName() );
    ti->setOffset( 0 );
    return ti;
}

/////////////////////////////////////////////////////////////////////////////////////

std::string EnumTypeInfo::getFieldNameByIndex( ULONG index )
{
    if ( index >= m_dia->getChildCount() )
        throw TypeException( m_dia->getName(), ": field not found" );

    pyDia::SymbolPtr field = m_dia->getChildByIndex(index);

    if ( !field )
        throw TypeException( m_dia->getName(), ": field not found" );   

    return field->getName();
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG EnumTypeInfo::getFieldCount()
{
    return m_dia->getChildCount();    
}

/////////////////////////////////////////////////////////////////////////////////////

python::dict EnumTypeInfo::asMap()
{
    python::dict            dct;

    pyDia::SymbolPtrList    symbolsList = m_dia->findChildrenImpl(SymTagData, "", nsfCaseSensitive  );

    for ( pyDia::SymbolPtrList::iterator  it = symbolsList.begin(); it != symbolsList.end(); it++ )
    {
         CComVariant     val;

         (*it)->getValue( val );

         dct[val.ulVal] = (*it)->getName();
    }

    return dct;
}

/////////////////////////////////////////////////////////////////////////////////////

std::string EnumTypeInfo::print()
{
    std::stringstream  sstr;

    sstr << "enum: " << getName() << std::endl;

    pyDia::SymbolPtrList    symbolsList = m_dia->findChildrenImpl(SymTagData, "", nsfCaseSensitive  );

    for ( pyDia::SymbolPtrList::iterator  it = symbolsList.begin(); it != symbolsList.end(); it++ )
    {
         CComVariant     val;
         (*it)->getValue( val );

         sstr << "   " << (*it)->getName();
         sstr << " = " << std::hex << val.ulVal << " (" << std::dec << val.ulVal << ')';
         sstr << std::endl;
    }

    return sstr.str();
}

/////////////////////////////////////////////////////////////////////////////////////

}; // end namespace pykd