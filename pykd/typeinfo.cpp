#include "stdafx.h"

#include "typeinfo.h"

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr  TypeInfo::getTypeInfo( pyDia::SymbolPtr &typeSym )
{
    ULONG  tag = typeSym->getSymTag();

    switch( typeSym->getSymTag() )
    {
    case SymTagBaseType:
        return getBaseTypeInfo( typeSym );

    case SymTagUDT:
        return TypeInfoPtr( new UdtTypeInfo( typeSym ) );

    case SymTagArrayType:
        return TypeInfoPtr( new ArrayTypeInfo( typeSym ) );

    case SymTagPointerType:
        return TypeInfoPtr( new PointerTypeInfo( typeSym ) );
    }

    throw DbgException( "type name invalid" );
}

/////////////////////////////////////////////////////////////////////////////////////

static const boost::regex arrayMatch("^(.*)\\[(\\d+)\\]$"); 

TypeInfoPtr  TypeInfo::getTypeInfo( pyDia::SymbolPtr &symScope, const std::string &symName )
{
    size_t pos = symName.find_first_of( "*[" );

    if ( pos == std::string::npos )
    {
        TypeInfoPtr    basePtr = getBaseTypeInfo( symName );
        if ( basePtr != 0 )
            return basePtr;

        pyDia::SymbolPtr  typeSym = symScope->getChildByName( symName );

        if ( typeSym->getSymTag() == SymTagData )
            typeSym = typeSym->getType();

        return getTypeInfo( typeSym );
    }
    
    if ( symName[ symName.size() - 1 ] == '*' )
        return TypeInfoPtr( new PointerTypeInfo( symScope,symName.substr( 0, symName.size() - 1 )  ) );

    boost::cmatch    matchResult;

    if ( boost::regex_match( symName.c_str(), matchResult, arrayMatch ) )
    {
        std::string     sym = std::string( matchResult[1].first, matchResult[1].second );

        return TypeInfoPtr( new ArrayTypeInfo( symScope, sym, std::atoi( matchResult[2].first ) ) );
    }


    throw DbgException( "type name invalid" );
}

/////////////////////////////////////////////////////////////////////////////////////

static const boost::regex baseMatch("^(Char)|(WChar)|(Int2B)|(UInt2B)|(Int4B)|(UInt4B)|(Int8B)|(UInt8B)|(Long)|(ULong)|(Float)|(Bool)$" );

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
            return TypeInfoPtr( new TypeInfoWrapper<short>("Int2B") );

        if ( baseMatchResult[4].matched )
            return TypeInfoPtr( new TypeInfoWrapper<unsigned short>("UInt2B") );

        if ( baseMatchResult[5].matched )
            return TypeInfoPtr( new TypeInfoWrapper<long>("Int4B") );

        if ( baseMatchResult[6].matched )
            return TypeInfoPtr( new TypeInfoWrapper<unsigned long>("UInt4B") );

        if ( baseMatchResult[7].matched )
            return TypeInfoPtr( new TypeInfoWrapper<__int64>("Int8B") );

        if ( baseMatchResult[8].matched )
            return TypeInfoPtr( new TypeInfoWrapper<unsigned __int64>("UInt8B") );

        if ( baseMatchResult[9].matched )
            return TypeInfoPtr( new TypeInfoWrapper<long>("Long") );

        if ( baseMatchResult[10].matched )
            return TypeInfoPtr( new TypeInfoWrapper<unsigned long>("ULong") );

        if ( baseMatchResult[11].matched )
            return TypeInfoPtr( new TypeInfoWrapper<float>("Float") );

        if ( baseMatchResult[12].matched )
            return TypeInfoPtr( new TypeInfoWrapper<bool>("Bool") );
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

    TypeInfoPtr     ptr = getBaseTypeInfo( symName );

    if ( ptr == 0 )
        ptr = TypeInfoPtr( new BaseTypeInfo( symbol ) );

    return ptr;
}

/////////////////////////////////////////////////////////////////////////////////////

PointerTypeInfo::PointerTypeInfo( pyDia::SymbolPtr &symbol  ) 
{
    m_derefType = TypeInfo::getTypeInfo( symbol->getType() );
    m_size = (ULONG)symbol->getSize();
}

/////////////////////////////////////////////////////////////////////////////////////

PointerTypeInfo::PointerTypeInfo( pyDia::SymbolPtr &symScope, const std::string &symName ) 
{
    m_derefType = TypeInfo::getTypeInfo( symScope, symName );
    m_size = (symScope->getMachineType() == IMAGE_FILE_MACHINE_AMD64) ? 8 : 4;
}

/////////////////////////////////////////////////////////////////////////////////////

std::string PointerTypeInfo::getName()
{
    //return m_derefType->getName() + '*';
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
    //std::stringstream       sstr;

    //sstr  << m_derefType->getName() << '[' << m_count << ']';

    //return sstr.str();
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

}; // end namespace pykd