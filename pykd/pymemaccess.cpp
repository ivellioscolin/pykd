#include "stdafx.h"

#include "kdlib\exceptions.h"

#include "pymemaccess.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

kdlib::MEMOFFSET_64 searchMemoryLst( kdlib::MEMOFFSET_64 beginOffset, unsigned long length, const python::list &pattern )
{
    std::vector<char>  p = listToVector<char>(pattern);
    do {
        AutoRestorePyState  pystate;
        return kdlib::searchMemory( beginOffset, length, p );
    } while(false);
}

///////////////////////////////////////////////////////////////////////////////

kdlib::MEMOFFSET_64 searchMemoryStr( kdlib::MEMOFFSET_64 beginOffset, unsigned long length, const std::string &pattern  )
{
    std::vector<char>  p( pattern.begin(), pattern.end() );
    do {
        AutoRestorePyState  pystate;
        return kdlib::searchMemory( beginOffset, length, p );
    } while(false);
}

///////////////////////////////////////////////////////////////////////////////

python::list loadBytes( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr )
{
    std::vector<unsigned char>  lst;
    
    do {
       AutoRestorePyState  pystate;
       lst = kdlib::loadBytes(offset, count, phyAddr);
    } while(false);

    return vectorToList(lst);
}

///////////////////////////////////////////////////////////////////////////////

python::list loadWords( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr )
{
    std::vector<unsigned short>  lst;
    
    do {
       AutoRestorePyState  pystate;
       lst = kdlib::loadWords(offset, count, phyAddr);
    } while(false);

    return vectorToList(lst);
}

///////////////////////////////////////////////////////////////////////////////

python::list loadDWords( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr )
{
    std::vector<unsigned long>  lst;
    
    do {
       AutoRestorePyState  pystate;
       lst = kdlib::loadDWords(offset, count, phyAddr);
    } while(false);

    return vectorToList(lst);
}

///////////////////////////////////////////////////////////////////////////////

python::list loadQWords( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr )
{
    std::vector<unsigned long long>  lst;
    
    do {
       AutoRestorePyState  pystate;
       lst = kdlib::loadQWords(offset, count, phyAddr);
    } while(false);

    return vectorToList(lst);
}

///////////////////////////////////////////////////////////////////////////////

python::list loadSignBytes( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr )
{
    std::vector<char>  lst;
    
    do {
       AutoRestorePyState  pystate;
       lst = kdlib::loadSignBytes(offset, count, phyAddr);
    } while(false);

    return vectorToList<char>(lst);
}

///////////////////////////////////////////////////////////////////////////////

python::list loadSignWords( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr )
{
    std::vector<short>  lst;
    
    do {
       AutoRestorePyState  pystate;
       lst = kdlib::loadSignWords(offset, count, phyAddr);
    } while(false);

    return vectorToList(lst);
}

///////////////////////////////////////////////////////////////////////////////

python::list loadSignDWords( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr )
{
    std::vector<long>  lst;
    
    do {
       AutoRestorePyState  pystate;
       lst = kdlib::loadSignDWords(offset, count, phyAddr);
    } while(false);

    return vectorToList(lst);
}

///////////////////////////////////////////////////////////////////////////////

python::list loadSignQWords( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr )
{
    std::vector<long long>  lst;
    
    do {
       AutoRestorePyState  pystate;
       lst = kdlib::loadSignQWords(offset, count, phyAddr);
    } while(false);

    return vectorToList(lst);
}

///////////////////////////////////////////////////////////////////////////////

python::list loadFloats( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr )
{
    std::vector<float>  lst;
    
    do {
       AutoRestorePyState  pystate;
       lst = kdlib::loadFloats(offset, count, phyAddr);
    } while(false);

    return vectorToList(lst);
}

///////////////////////////////////////////////////////////////////////////////

python::list loadDoubles( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr )
{
    std::vector<double>  lst;
    
    do {
       AutoRestorePyState  pystate;
       lst = kdlib::loadDoubles(offset, count, phyAddr);
    } while(false);

    return vectorToList(lst);
}

///////////////////////////////////////////////////////////////////////////////

void writeBytes(kdlib::MEMOFFSET_64 offset, const python::list &list, bool phyAddr)
{
    auto values = listToVector<unsigned char>(list);
    {
        AutoRestorePyState  pystate;
        kdlib::writeBytes(offset, values, phyAddr);
    }
}

///////////////////////////////////////////////////////////////////////////////

void writeWords( kdlib::MEMOFFSET_64 offset, const python::list &list, bool phyAddr )
{
    auto values = listToVector<unsigned short>(list);
    {
        AutoRestorePyState  pystate;
        kdlib::writeWords(offset, values, phyAddr);
    }
}

///////////////////////////////////////////////////////////////////////////////

void writeDWords( kdlib::MEMOFFSET_64 offset, const python::list &list, bool phyAddr )
{
    auto values = listToVector<unsigned long>(list);
    {
        AutoRestorePyState  pystate;
        kdlib::writeDWords(offset, values, phyAddr);
    }
}

///////////////////////////////////////////////////////////////////////////////

void writeQWords( kdlib::MEMOFFSET_64 offset, const python::list &list, bool phyAddr )
{
    auto values = listToVector<unsigned long long>(list);
    {
        AutoRestorePyState  pystate;
        kdlib::writeQWords(offset, values, phyAddr);
    }
}

///////////////////////////////////////////////////////////////////////////////

void writeSignBytes( kdlib::MEMOFFSET_64 offset, const python::list &list, bool phyAddr )
{
    auto values = listToVector<char, signed char>(list);
    {
        AutoRestorePyState  pystate;
        kdlib::writeSignBytes(offset, values, phyAddr);
    }
}

///////////////////////////////////////////////////////////////////////////////

void writeSignWords( kdlib::MEMOFFSET_64 offset, const python::list &list, bool phyAddr )
{
    auto values = listToVector<short>(list);
    {
        AutoRestorePyState  pystate;
        kdlib::writeSignWords(offset, values, phyAddr);
    }
}

///////////////////////////////////////////////////////////////////////////////

void writeSignDWords( kdlib::MEMOFFSET_64 offset, const python::list &list, bool phyAddr )
{
    auto values = listToVector<long>(list);
    {
        AutoRestorePyState  pystate;
        kdlib::writeSignDWords(offset, values, phyAddr);
    }
}

///////////////////////////////////////////////////////////////////////////////

void writeSignQWords( kdlib::MEMOFFSET_64 offset, const python::list &list, bool phyAddr )
{
    auto values = listToVector<long long>(list);
    {
        AutoRestorePyState  pystate;
        kdlib::writeSignQWords(offset, values, phyAddr);
    }
}

///////////////////////////////////////////////////////////////////////////////

void writeFloats( kdlib::MEMOFFSET_64 offset, const python::list &list, bool phyAddr )
{
    auto values = listToVector<float>(list);
    {
        AutoRestorePyState  pystate;
        kdlib::writeFloats(offset, values, phyAddr);
    }
}

///////////////////////////////////////////////////////////////////////////////

void writeDoubles( kdlib::MEMOFFSET_64 offset, const python::list &list, bool phyAddr )
{
    auto values = listToVector<double>(list);
    {
        AutoRestorePyState  pystate;
        kdlib::writeDoubles(offset, values, phyAddr);
    }
}

///////////////////////////////////////////////////////////////////////////////

python::list loadPtrList( kdlib::MEMOFFSET_64 offset )
{
    std::vector<kdlib::MEMOFFSET_64>  lst;

    do {
       AutoRestorePyState  pystate;
       lst = kdlib::loadPtrList(offset);
    } while(false);

    return vectorToList(lst);
}

///////////////////////////////////////////////////////////////////////////////

python::list loadPtrArray( kdlib::MEMOFFSET_64 offset, unsigned long count )
{ 
    std::vector<kdlib::MEMOFFSET_64>  lst;

    do {
       AutoRestorePyState  pystate;
       lst = kdlib::loadPtrs(offset, count);
    } while(false);

    return vectorToList(lst);
}

///////////////////////////////////////////////////////////////////////////////

std::wstring loadUnicodeStr(kdlib::MEMOFFSET_64 offset)
{
    unsigned short  length = kdlib::ptrWord( offset );
    unsigned short  maximumLength = kdlib::ptrWord( offset + 2 );
    kdlib::MEMOFFSET_64  buffer = kdlib::ptrPtr( offset + kdlib::ptrSize() );

    if ( maximumLength < length )
        throw kdlib::DbgException("Corrupted UNICODE_STRING structure");

    if ( length == 0 )
        return std::wstring();

    return kdlib::loadWChars( buffer, length/2 );
}

///////////////////////////////////////////////////////////////////////////////

std::string loadAnsiStr(kdlib::MEMOFFSET_64 offset)
{
    unsigned short  length = kdlib::ptrWord( offset );
    unsigned short  maximumLength = kdlib::ptrWord( offset + 2 );
    kdlib::MEMOFFSET_64  buffer = kdlib::ptrPtr( offset + kdlib::ptrSize() );

    if ( maximumLength < length )
        throw kdlib::DbgException("Corrupted ANSI_STRING structure");

    if ( length == 0 )
        return std::string();

    return kdlib::loadChars( buffer, length );
}

///////////////////////////////////////////////////////////////////////////////


}; // end pykd namespace
