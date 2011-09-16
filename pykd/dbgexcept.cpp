#include "stdafx.h"
#include "dbgexcept.h"

using namespace pykd;

////////////////////////////////////////////////////////////////////////////////

PyObject *DbgException::baseExceptTypeObject = NULL;

/////////////////////////////////////////////////////////////////////////////////

void DbgException::exceptionTranslate( const DbgException &e )
{
    boost::python::object pyExcept(e);

    PyErr_SetObject( baseExceptTypeObject, pyExcept.ptr() );
}

/////////////////////////////////////////////////////////////////////////////////


//PyObject    *eventExceptionType = NULL;
//PyObject    *typeExceptionType = NULL;
//PyObject    *memoryExceptionType = NULL;
//
//void WaitEventException::exceptionTranslate( const WaitEventException &e )
//{
//    boost::python::object                   pyExcept(e);
//
//    PyErr_SetObject( eventExceptionType, pyExcept.ptr());
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//void TypeException::exceptionTranslate( const TypeException &e )
//{
//    boost::python::object                   pyExcept(e);
//   
//    PyErr_SetObject( typeExceptionType, pyExcept.ptr());    
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//void MemoryException::translate( const MemoryException &e )
//{
//    boost::python::object                   pyExcept(e);
//
//    PyErr_SetObject( memoryExceptionType, pyExcept.ptr());    
//}
//
///////////////////////////////////////////////////////////////////////////////////