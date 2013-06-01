#pragma once

#include <boost/python/list.hpp>
namespace python = boost::python;

#include "kdlib/memaccess.h"

#include "stladaptor.h"

namespace pykd {

inline int ptrSignByte( kdlib::MEMOFFSET_64 offset )
{
    return kdlib::ptrSignByte(offset);
}

inline python::list loadBytes( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false )
{
    return vectorToList( kdlib::loadBytes( offset, count, phyAddr ) );
}

inline python::list loadWords( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false )
{
    return vectorToList( kdlib::loadWords( offset, count, phyAddr ) );
}

inline python::list loadDWords( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false )
{
    return vectorToList( kdlib::loadDWords( offset, count, phyAddr ) );
}

inline python::list loadQWords( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false )
{
    return vectorToList( kdlib::loadQWords( offset, count, phyAddr ) );
}

inline python::list loadSignBytes( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false )
{
    return vectorToList( kdlib::loadSignBytes( offset, count, phyAddr ) );
}

inline python::list loadSignWords( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false )
{
    return vectorToList( kdlib::loadSignWords( offset, count, phyAddr ) );
}

inline python::list loadSignDWords( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false )
{
    return vectorToList( kdlib::loadSignDWords( offset, count, phyAddr ) );
}

inline python::list loadSignQWords( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false )
{
    return vectorToList( kdlib::loadSignQWords( offset, count, phyAddr ) );
}

inline python::list loadFloats( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false )
{
    return vectorToList( kdlib::loadFloats( offset, count, phyAddr ) );
}

inline python::list loadDoubles( kdlib::MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false )
{
    return vectorToList( kdlib::loadDoubles( offset, count, phyAddr ) );
}

inline python::list loadPtrList( kdlib::MEMOFFSET_64 offset )
{
    return vectorToList( kdlib::loadPtrList(offset) );
}

inline python::list loadPtrArray(  kdlib::MEMOFFSET_64 offset, unsigned long count )
{ 
    return vectorToList( kdlib::loadPtrs(offset, count) );
}

} // end namespace pykd

