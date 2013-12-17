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
        throw kdlib::DbgException("Corrupted UNICODE_STRING structure");

    if ( length == 0 )
        return std::string();

    return kdlib::loadChars( buffer, length );
}

///////////////////////////////////////////////////////////////////////////////


}; // end pykd namespace
