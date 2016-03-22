#pragma once

#include <exception>
#include <string>

#include "kdlib/exceptions.h"

///////////////////////////////////////////////////////////////////////////////

namespace pykd {

class OverflowException : public std::exception
{
public:

    OverflowException(const char* desc) : std::exception(desc)
    {}
};

class AttributeException : public std::exception
{
public:

    AttributeException(const char* desc) : std::exception(desc)
    {}
};


template< class TExcept >
struct exceptPyType{
    static python::handle<>     pyExceptType;
};

template< class TExcept, class TBaseExcept = python::detail::not_specified >
class exception  {

public:
    
    exception(const std::string& className, const std::string& classDesc)
    {
        std::string fullName = "pykd." + className;

        if (boost::is_same<TBaseExcept, python::detail::not_specified>::value)
        {
            exceptPyType<TExcept>::pyExceptType = 
                python::handle<>(PyErr_NewExceptionWithDoc(
                    const_cast<char*>(fullName.c_str()),
                    const_cast<char*>(classDesc.c_str()),
                    NULL,
                    NULL));
        }
        else
        {
            exceptPyType<TExcept>::pyExceptType = 
                python::handle<>(PyErr_NewExceptionWithDoc(
                    const_cast<char*>(fullName.c_str()), 
                    const_cast<char*>(classDesc.c_str()), 
                    exceptPyType<TBaseExcept>::pyExceptType.get(), 
                    NULL));
        }

        python::scope().attr(className.c_str()) = exceptPyType<TExcept>::pyExceptType;
    }
};


inline void dbgExceptionTranslate(const kdlib::DbgException &e)
{
    if ( typeid(e).hash_code() == typeid(kdlib::MemoryException).hash_code() )
    {
        python::object      exceptObj = python::object( exceptPyType<kdlib::MemoryException>::pyExceptType )( e.what() );
        PyErr_SetObject( exceptPyType<kdlib::MemoryException>::pyExceptType.get(), exceptObj.ptr());
        return;
    }

    if ( typeid(e).hash_code() == typeid(kdlib::SymbolException).hash_code() )
    {
        python::object      exceptObj = python::object( exceptPyType<kdlib::SymbolException>::pyExceptType )( e.what() );
        PyErr_SetObject( exceptPyType<kdlib::SymbolException>::pyExceptType.get(), exceptObj.ptr());
        return;
    }

    if ( typeid(e).hash_code() == typeid(kdlib::TypeException).hash_code() )
    {
        python::object      exceptObj = python::object( exceptPyType<kdlib::TypeException>::pyExceptType )( e.what() );
        PyErr_SetObject( exceptPyType<kdlib::TypeException>::pyExceptType.get(), exceptObj.ptr());
        return;
    }

    if ( typeid(e).hash_code() == typeid(kdlib::IndexException).hash_code() )
    {
        PyErr_SetString( PyExc_IndexError, "Index out of range");
        return;
    }

    python::object      exceptObj = python::object( exceptPyType<kdlib::DbgException>::pyExceptType )( e.what() );
    PyErr_SetObject( exceptPyType<kdlib::DbgException>::pyExceptType.get(), exceptObj.ptr());
}

inline void pykdExceptionTranslate(const std::exception &e)
{
    if (typeid(e).hash_code() == typeid(OverflowException).hash_code())
    {
        PyErr_SetString(PyExc_OverflowError, e.what());
        return;
    }

    if (typeid(e).hash_code() == typeid(AttributeException).hash_code())
    {
        PyErr_SetString(PyExc_AttributeError, e.what());
        return;
    }

}

inline void registerExceptions()
{
    pykd::exception<kdlib::DbgException>( "DbgException", "Pykd base exception class" );
    pykd::exception<kdlib::MemoryException,kdlib::DbgException>( "MemoryException", "Target memory access exception class" );
    pykd::exception<kdlib::SymbolException,kdlib::DbgException>( "SymbolException", "Symbol exception" );
    pykd::exception<kdlib::TypeException,kdlib::SymbolException>( "TypeException", "type exception" );

    python::register_exception_translator<kdlib::DbgException>( &dbgExceptionTranslate );
    python::register_exception_translator<OverflowException>( &pykdExceptionTranslate );
    python::register_exception_translator<AttributeException>(&pykdExceptionTranslate);
}

/////////////////////////////////////////////////////////////////////////////////////

void printException();

///////////////////////////////////////////////////////////////////////////////////

#define PYKD_NOT_IMPLEMENTED()   throw kdlib::ImplementException( __FILE__, __LINE__, "Not Implemented" );
#define PYKD_TODO(x) throw kdlib::ImplementException( __FILE__, __LINE__, x );

}; // namespace pykd

