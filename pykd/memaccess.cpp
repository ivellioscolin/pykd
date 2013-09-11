#include "stdafx.h"

#include "kdlib\exceptions.h"

#include "memaccess.h"

namespace pykd {

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
