#pragma once

#include <exception>
#include <string>

#include "kdlib/exceptions.h"

///////////////////////////////////////////////////////////////////////////////

namespace pykd {

template< class TExcept >
struct exceptPyType{
    static python::handle<>     pyExceptType;
};

template< class TExcept, class TBaseExcept = python::detail::not_specified >
class exception  {

public:

    exception( const std::string& className, const std::string& classDesc ) 
    {
        python::handle<>   basedtype;      

        if ( boost::is_same<TBaseExcept, python::detail::not_specified>::value )
        {
            basedtype = python::handle<>(PyExc_Exception);
        }
        else
        {
            basedtype = exceptPyType<TBaseExcept>::pyExceptType;
        }

        python::dict       ob_dict;
       
        ob_dict["__doc__"] = classDesc;

        python::tuple      ob_bases = python::make_tuple( basedtype );

        python::object     ob = python::object( python::handle<>(Py_TYPE(basedtype.get()) ) )( className, ob_bases, ob_dict );

        python::scope().attr( className.c_str() ) = ob;

        exceptPyType<TExcept>::pyExceptType = python::handle<>( ob.ptr() );

        python::register_exception_translator<TExcept>( &exceptionTranslate );
    }

    static
    void
    exceptionTranslate(const TExcept &e ) {

        python::object      exceptObj = python::object( exceptPyType<TExcept>::pyExceptType )( e.what() );

        PyErr_SetObject( exceptPyType<TExcept>::pyExceptType.get(), exceptObj.ptr());
    }

};

//////////////////////////////////////////////////////////////////////////////

struct ExceptionTranslator {

    static
    void
    indexTranslate(const kdlib::IndexException &e ) {
        PyErr_SetString( PyExc_IndexError, "Index out of range");
    }

};

///////////////////////////////////////////////////////////////////////////////////

void printException();

///////////////////////////////////////////////////////////////////////////////////

}; // namespace pykd

