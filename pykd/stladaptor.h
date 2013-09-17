#pragma once

#include <vector>

#include <boost/python/list.hpp>
namespace python = boost::python;

namespace pykd {

template<typename T>
inline
python::list vectorToList( const std::vector<T>  &v ) {
    python::list lst;
    for ( std::vector<T>::const_iterator it = v.begin(); it != v.end(); ++it )
        lst.append( *it );
    return lst;
}

template<>
inline
python::list vectorToList<char>( const std::vector<char>  &v ) {
    python::list lst;
    for ( std::vector<char>::const_iterator it = v.begin(); it != v.end(); ++it )
        lst.append( int(*it) );
    return lst;
}


template<typename T>
inline
std::vector<T> listToVector( const python::list &lst )
{
    std::vector<T> vec( python::len(lst) );

    for (  long i = 0; i < python::len(lst); ++i )
    {
        T  v = python::extract<T>(lst[i]);
        vec[i] =v;
    }

    return vec;
}

} // end namespace pykd
