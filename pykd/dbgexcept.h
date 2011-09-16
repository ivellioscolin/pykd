#pragma once

#include <exception>
#include <string>

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

class DbgException : public std::exception
{
public:

    DbgException( const std::string  &desc ) :
        std::exception( desc.c_str() )
        {}

    const char* getDesc() const {
        return what();
    }

    static
    void
    exceptionTranslate(const DbgException &e );

    static void setTypeObject(PyObject *p) {
        baseExceptTypeObject = p;
    }

    std::string print() {
         return what();
    }

private:
    static PyObject *baseExceptTypeObject;
};

///////////////////////////////////////////////////////////////////////////////////

}; // namespace pykd

///////////////////////////////////////////////////////////////////////////////////


//
//class WaitEventException : public DbgException
//{
//public:
//    WaitEventException()
//        : DbgException( "none of the targets could generate events" )
//    {
//    }
//
//    static void exceptionTranslate(const WaitEventException &e);
//};
//
///////////////////////////////////////////////////////////////////////////////////
//
//class TypeException : public DbgException
//{
//public:
//
//    TypeException() :
//        DbgException( "type operation invalid" )
//        {}
//
//    TypeException( const std::string  &desc ) :
//       DbgException( desc )
//       {}
//    
//    static
//    void
//    exceptionTranslate(const TypeException  &e );
//
//};
//
///////////////////////////////////////////////////////////////////////////////////
//
//class IndexException : public DbgException
//{
//public:
//
//    IndexException() :
//       DbgException( "Index out of range" )
//       {}        
//    
//    static
//    void
//    translate(const IndexException  &e ) {
//        PyErr_SetString(PyExc_IndexError, "Index out of range");
//    }
//};
//
///////////////////////////////////////////////////////////////////////////////////
//
//class MemoryException : public DbgException
//{
//public:
//
//    MemoryException( ULONG64 targetAddr ) :
//        m_targetAddress( targetAddr ),
//        DbgException( MemoryException::DescMaker( targetAddr, false ).desc() )
//        {}    
//
//    MemoryException( ULONG64 targetAddr, bool phyAddr ) :
//        m_targetAddress( targetAddr ),
//        DbgException( MemoryException::DescMaker( targetAddr, phyAddr ).desc() )
//        {}    
//       
//    static
//    void
//    translate( const MemoryException &e );
//    
//    ULONG64
//    getAddress() const {
//        return m_targetAddress;
//    }
//    
//private:    
//        
//    ULONG64     m_targetAddress;
//    
//    class DescMaker {
//    public:
//        DescMaker( ULONG64 addr, bool phyAddr )
//        {
//            std::stringstream   sstr;
//            if ( phyAddr )
//                sstr << "Memory exception at 0x" << std::hex << addr << " target physical address";
//            else
//                sstr << "Memory exception at 0x" << std::hex << addr << " target virtual address";                            
//            m_desc = sstr.str();
//        }   
//        
//        const std::string&
//        desc() const {
//            return m_desc;
//        }
//        
//    private:
//        std::string     m_desc;
//    };
//};
//
///////////////////////////////////////////////////////////////////////////////////
//
//extern  PyObject  *baseExceptionType;
//extern  PyObject  *eventExceptionType;
//extern  PyObject  *typeExceptionType;
//extern  PyObject  *memoryExceptionType;
//
///////////////////////////////////////////////////////////////////////////////////