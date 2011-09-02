#include "stdafx.h"
#include "dbgexcept.h"

/////////////////////////////////////////////////////////////////////////////////

// типы исключений

PyObject    *baseExceptionType = NULL;
PyObject    *eventExceptionType = NULL;
PyObject    *typeExceptionType = NULL;
PyObject    *memoryExceptionType = NULL;



/////////////////////////////////////////////////////////////////////////////////

void DbgException::exceptionTranslate( const DbgException &e )
{
    boost::python::object                   pyExcept(e);

    PyErr_SetObject( baseExceptionType, pyExcept.ptr());
}

/////////////////////////////////////////////////////////////////////////////////

void WaitEventException::exceptionTranslate( const WaitEventException &e )
{
    boost::python::object                   pyExcept(e);

    PyErr_SetObject( eventExceptionType, pyExcept.ptr());
}

/////////////////////////////////////////////////////////////////////////////////

void TypeException::exceptionTranslate( const TypeException &e )
{
    boost::python::object                   pyExcept(e);
   
    PyErr_SetObject( typeExceptionType, pyExcept.ptr());    
}

/////////////////////////////////////////////////////////////////////////////////

void MemoryException::translate( const MemoryException &e )
{
    boost::python::object                   pyExcept(e);

    PyErr_SetObject( memoryExceptionType, pyExcept.ptr());    
}

/////////////////////////////////////////////////////////////////////////////////