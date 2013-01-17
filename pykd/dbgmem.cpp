#include "stdafx.h"
#include "dbgengine.h"

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////////

bool compareMemory( ULONG64 addr1, ULONG64 addr2, ULONG length, bool phyAddr )
{
    bool        result = false;

    addr1 = addr64( addr1 );
    addr2 = addr64( addr2 );

    std::vector<char>   m1(length);
    std::vector<char>   m2(length);

    readMemory( addr1, &m1[0], length, phyAddr );
    readMemory( addr2, &m2[0], length, phyAddr );

    return std::equal( m1.begin(), m1.end(), m2.begin() );
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG64 ptrByte( ULONG64 offset )
{
    unsigned char     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG64 ptrWord( ULONG64 offset )
{
    unsigned short     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG64 ptrDWord( ULONG64 offset )
{
    unsigned long     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG64 ptrQWord( ULONG64 offset )
{
    unsigned __int64     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG64 ptrMWord( ULONG64 offset )
{
    return ptrSize() == 8 ? ptrQWord( offset ) : ptrDWord(offset );
}

ULONG64 ptrPtr( ULONG64 offset )
{
    return ptrMWord( offset );
}

/////////////////////////////////////////////////////////////////////////////////////

LONG64 ptrSignByte( ULONG64 offset )
{
    char     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

/////////////////////////////////////////////////////////////////////////////////////

LONG64 ptrSignWord( ULONG64 offset )
{
    short     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

/////////////////////////////////////////////////////////////////////////////////////

LONG64 ptrSignDWord( ULONG64 offset )
{
    long     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

/////////////////////////////////////////////////////////////////////////////////////

LONG64 ptrSignQWord( ULONG64 offset )
{
    __int64     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

/////////////////////////////////////////////////////////////////////////////////////

LONG64 ptrSignMWord( ULONG64 offset )
{
    return  ptrSize() == 8 ? ptrSignQWord( offset ) : ptrSignDWord(offset);
}

/////////////////////////////////////////////////////////////////////////////////////

double ptrSingleFloat( ULONG64 offset )
{
    float  val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

/////////////////////////////////////////////////////////////////////////////////////

double ptrDoubleFloat( ULONG64 offset )
{
    double  val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

/////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct PyListType
{
    typedef T       ElementType;
};

template<>
struct PyListType<char>
{
    typedef int     ElementType;
};

template<typename T>
python::list
loadArray( ULONG64 offset, ULONG count, bool phyAddr )
{
    std::vector<T>   buffer(count);

    if (count)
        readMemory( offset, &buffer[0], count*sizeof(T), phyAddr );

    python::list        lst;

    for( ULONG  i = 0; i < count; ++i )
        lst.append( static_cast<PyListType<T>::ElementType> (buffer[i]) );

    return lst;
}

/////////////////////////////////////////////////////////////////////////////////////

python::list loadBytes( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<unsigned char>( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list loadWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<unsigned short>( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list loadDWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<unsigned long>( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list loadQWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<unsigned __int64>( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list loadSignBytes( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<char>( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list loadSignWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<short>( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list loadSignDWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<long>( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list loadSignQWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<__int64>( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list loadFloats(  ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<float>( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list loadDoubles(  ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<double>( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

std::string loadChars( ULONG64 offset, ULONG  number, bool phyAddr )
{
    std::vector<char>   buffer(number);

    ULONG  bufferSize = (ULONG)( sizeof(std::vector<char>::value_type)*buffer.size() );
    
    if (number)
        readMemory( offset, &buffer[0], bufferSize, phyAddr );

    return std::string( buffer.begin(), buffer.end() );
}

/////////////////////////////////////////////////////////////////////////////////////

std::wstring loadWChars( ULONG64 offset, ULONG  number, bool phyAddr )
{
    std::vector<wchar_t>   buffer(number);

    ULONG  bufferSize = (ULONG)( sizeof(std::vector<wchar_t>::value_type)*buffer.size() );
    
    if (number)
        readMemory( offset, &buffer[0], bufferSize, phyAddr );

    return std::wstring( buffer.begin(), buffer.end() );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list loadPtrList( ULONG64 offset )
{
    offset = addr64( offset );

    ULONG64     entryAddress = 0;
   
    python::list    lst;
    
    for( entryAddress = ptrPtr( offset ); entryAddress != offset && entryAddress != 0; entryAddress = ptrPtr( entryAddress ) )
        lst.append( entryAddress );
    
    return lst;
}

/////////////////////////////////////////////////////////////////////////////////////

python::list loadPtrArray( ULONG64 offset, ULONG  number )
{
    offset = addr64( offset );

    python::list    lst;

    for ( ULONG i = 0; i < number; ++i )
        lst.append( ptrPtr( offset + i*ptrSize() ) );

    return lst;
}

/////////////////////////////////////////////////////////////////////////////////////

std::wstring loadUnicodeStr( ULONG64 offset )
{
    USHORT  length;
    USHORT  maximumLength;
    ULONG64  buffer = 0;
    ULONG  ptrsize = ptrSize();

    offset = addr64( offset );
    
    readMemory( offset, &length, sizeof( length ) );
        
    if ( length == 0 )
        return L"";       
        
    offset += sizeof( length );
        
    readMemory(  offset, &maximumLength, sizeof( maximumLength ) );
        
    offset += sizeof( maximumLength );
    
    offset += offset % ptrsize ? ( ptrsize - offset % ptrsize ) : 0 ; // выравнивание
    
    buffer = addr64( ptrPtr( offset ) );
            
    offset += ptrsize;
 
    std::vector<wchar_t> str(length / 2);
    
    readMemory( buffer, &str[0], length );
        
    return  std::wstring (&str[0], length/2);
}

/////////////////////////////////////////////////////////////////////////////////////

std::string loadAnsiStr( ULONG64 offset )
{
    USHORT  length;
    USHORT  maximumLength;
    ULONG64  buffer = 0;
    ULONG  ptrsize = ptrSize();

    offset = addr64( offset );
    
    readMemory( offset, &length, sizeof( length ) );
        
    if ( length == 0 )
        return "";
        
    offset += sizeof( length );
        
    readMemory( offset, &maximumLength, sizeof( maximumLength ) );
        
    offset += sizeof( maximumLength );
    
    offset += offset % ptrsize ? ( ptrsize - offset % ptrsize ) : 0 ; // выравнивание
    
    buffer = addr64( ptrPtr( offset ) );
            
    offset += ptrsize;
 
    std::vector<char> str(length);
    
    readMemory( buffer, &str[0], length );
        
    return  std::string (&str[0], length);
}

/////////////////////////////////////////////////////////////////////////////////////

python::tuple findMemoryRegionPy( ULONG64 beginOffset )
{
    ULONG64 startOffset, length;
    findMemoryRegion( beginOffset, &startOffset, &length );
    return python::make_tuple( startOffset, length );
}

/////////////////////////////////////////////////////////////////////////////////////

}; // end pykd namespace

