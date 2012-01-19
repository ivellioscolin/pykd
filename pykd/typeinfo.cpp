#include "stdafx.h"

#include "typeinfo.h"

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr  TypeInfo::getTypeInfo( pyDia::SymbolPtr &typeSym )
{
    ULONG  tag = typeSym->getSymTag();

    const ULONG symTag = typeSym->getSymTag();
    switch( symTag )
    {
    case SymTagBaseType:
        return getBaseTypeInfo( typeSym );

    case SymTagUDT:
        return TypeInfoPtr( new UdtTypeInfo( typeSym ) );

    case SymTagArrayType:
        return TypeInfoPtr( new ArrayTypeInfo( typeSym ) );

    case SymTagPointerType:
        return TypeInfoPtr( new PointerTypeInfo( typeSym ) );

    case SymTagEnum:
        return TypeInfoPtr( new EnumTypeInfo( typeSym ) );
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

/////////////////////////////////////////////////////////////////////////////////////

PointerTypeInfo::PointerTypeInfo( pyDia::SymbolPtr &symbol  ) 
{
    try
    {
        m_derefType = TypeInfo::getTypeInfo( symbol->getType() );
    }
    catch (const SymbolException &)
    {
        m_derefType.swap( TypeInfoPtr() );
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

            typeInfo = dynamic_cast<PointerTypeInfo*>(typeInfo)->getDerefType().get();

            continue;
        }

        break;

    } while ( true );

    name.insert( 0, typeInfo->getName() );

    return name;
}

/////////////////////////////////////////////////////////////////////////////////////

static const boost::regex bracketMatch("^([^\\(]*)\\((.*)\\)([^\\)]*)$"); 

static const boost::regex typeMatch("^([^\\(\\)\\*\\[\\]]*)([\\(\\)\\*\\[\\]\\d]*)$"); 

static const boost::regex ptrMatch("^\\*(.*)$");

static const boost::regex arrayMatch("^(.*)\\[(\\d+)\\]$");

TypeInfoPtr TypeInfo::getComplexType( pyDia::SymbolPtr &symScope, const std::string &symName )
{
    ULONG  ptrSize = (symScope->getMachineType() == IMAGE_FILE_MACHINE_AMD64) ? 8 : 4;

    boost::cmatch    matchResult;

    if ( !boost::regex_match( symName.c_str(), matchResult, typeMatch  ) )
        throw TypeException( symName, "type name is invalid" );

    TypeInfoPtr     lowestTypeInfo = getTypeInfo( symScope, std::string( matchResult[1].first, matchResult[1].second ) );

    return getRecurciveComplexType( lowestTypeInfo, std::string( matchResult[2].first, matchResult[2].second ), ptrSize );
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
    pyDia::SymbolPtr field;
    LONG addOffset = 0;
    try
    {
        field = m_dia->getChildByName( fieldName );
    }
    catch (const pyDia::Exception &)
    {
    }

    if ( !field && !getBaseField( m_dia, fieldName, addOffset, field ) )
        throw TypeException( m_dia->getName(), fieldName + ": field not found" );

    TypeInfoPtr  ti = TypeInfo::getTypeInfo( m_dia, field );
    ti->setOffset( addOffset + field->getOffset() );
    return ti;
}

/////////////////////////////////////////////////////////////////////////////////////

bool 
UdtTypeInfo::getBaseField(
    pyDia::SymbolPtr symUdt,
    const std::string &fieldName,
    LONG &addOffset,
    pyDia::SymbolPtr &symField,
    ULONG currLevel /*= 0*/
)
{
    if (currLevel > 16)
        return false;

    LONG currOffset = 0;

    pyDia::SymbolPtrList lstBases = symUdt->findChildrenImpl(SymTagBaseClass);
    pyDia::SymbolPtrList::iterator it = lstBases.begin();
    for (; it != lstBases.end(); ++it)
    {
        // find in direct base
        try
        {
            symField = (*it)->getChildByName( fieldName );
            addOffset += currOffset;
            return true;
        }
        catch (const pyDia::Exception &)
        {
        }

        // find in base of base
        try
        {
            if (getBaseField(*it, fieldName, addOffset, symField, currLevel + 1))
            {
                addOffset += currOffset;
                return true;
            }
        }
        catch (const pyDia::Exception &)
        {
        }

        // correct offset
        currOffset += static_cast<ULONG>( (*it)->getSize() );
    }
    return false;
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

}; // end namespace pykd