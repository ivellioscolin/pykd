#include "stdafx.h"
#include "dbgexcept.h"

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

PyObject *DbgException::baseExceptTypeObject = NULL;
PyObject *MemoryException::memoryExceptionTypeObject = NULL;
PyObject *WaitEventException::waitEventExceptTypeObject = NULL;

/////////////////////////////////////////////////////////////////////////////////

void DbgException::exceptionTranslate( const DbgException &e )
{
    python::object pyExcept(e);

    PyErr_SetObject( baseExceptTypeObject, pyExcept.ptr() );
}

/////////////////////////////////////////////////////////////////////////////////

void MemoryException::exceptionTranslate( const MemoryException &e )
{
    python::object pyExcept(e);

    PyErr_SetObject( memoryExceptionTypeObject, pyExcept.ptr() );
}

/////////////////////////////////////////////////////////////////////////////////

std::string buildExceptDesc(PCSTR routineName, HRESULT hres)
{
    std::stringstream sstream;
    sstream << "Call " << routineName << " failed\n";
    sstream << "HRESULT 0x" << std::hex << hres;
    return sstream.str();
}

/////////////////////////////////////////////////////////////////////////////////

void WaitEventException::exceptionTranslate( const WaitEventException &e )
{
    python::object pyExcept(e);

    PyErr_SetObject( waitEventExceptTypeObject, pyExcept.ptr() );
}


/////////////////////////////////////////////////////////////////////////////////

}; // end namespace pykd





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