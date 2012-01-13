#pragma once

#include <exception>
#include <string>

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

template<typename TExcept>
class ExceptionTranslator {

public:

    static
    void
    exceptionTranslate(const TExcept &e ) {
        python::object pyExcept(e);
        PyErr_SetObject( exceptTypeObject, pyExcept.ptr() );
    }

    static void setTypeObject(PyObject *p) {
        exceptTypeObject = p;
        python::register_exception_translator<TExcept>( &exceptionTranslate );
    }

private:

    static PyObject *exceptTypeObject;

};

/////////////////////////////////////////////////////////////////////////////////

class PyException : public std::exception
{
public:

    PyException( PyObject*  pyObj, const std::string &desc ) :
        std::exception( desc.c_str() ),
        m_typeObj( pyObj )
        {}    

    static
    void
    exceptionTranslate(const PyException &e ) {
        PyErr_SetString( e.m_typeObj, e.what() );
    }

private:

    PyObject*       m_typeObj;
};

/////////////////////////////////////////////////////////////////////////////////

class DbgException : public std::exception
{
public:

    DbgException( const std::string  &desc ) :
        std::exception( desc.c_str() )
        {}    

    DbgException( const std::string  &methodName, HRESULT hres ) :
        std::exception( buildHresultDesc( methodName, hres ).c_str() )
        {}    

    const char* getDesc() const {
        return what();
    }

private:

    std::string buildHresultDesc( const std::string  &methodName, HRESULT hres ) 
    {
        std::stringstream sstream;
        sstream << "Call " << methodName << " failed\n";
        sstream << "HRESULT 0x" << std::hex << hres;
        return sstream.str();
    }

};

/////////////////////////////////////////////////////////////////////////////////

class WaitEventException : public DbgException
{
public:

    WaitEventException() 
       : DbgException( "None of the targets could generate events" )
        {}
};

/////////////////////////////////////////////////////////////////////////////////

class MemoryException : public DbgException
{
public:

    MemoryException( ULONG64 targetAddr, bool phyAddr = false ) :
        m_targetAddress( targetAddr ),
        DbgException( buildDesc( targetAddr, phyAddr ) )
        {}    
    
    ULONG64
    getAddress() const {
        return m_targetAddress;
    }
    
private:    
        
    ULONG64             m_targetAddress;

    std::string buildDesc( ULONG64 addr, bool phyAddr )
    {
        std::stringstream   sstr;
        if ( phyAddr )
            sstr << "Memory exception at 0x" << std::hex << addr << " target physical address";
        else
            sstr << "Memory exception at 0x" << std::hex << addr << " target virtual address";                            
       
        return sstr.str();
    }   
};

/////////////////////////////////////////////////////////////////////////////////

class AddSyntheticSymbolException : public DbgException
{
public:

    AddSyntheticSymbolException(HRESULT hres)
        : DbgException( buildDesc(hres) )
    {
    }

private:
    std::string buildDesc(HRESULT hres) {
        std::stringstream sstream;
        sstream << "Add synthetic symbol faield\n";
        sstream << "HRESULT 0x" << std::hex << hres;
        return sstream.str();
    }
};

/////////////////////////////////////////////////////////////////////////////////

}; // namespace pykd

