#pragma once

#include <exception>
#include <string>

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

class SymbolException : public DbgException 
{
public:

    SymbolException( const std::string  &desc ) :
        DbgException( desc.c_str() )
        {}    

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

class TypeException : public SymbolException
{
public:

    TypeException( const std::string &typeName, const std::string  &errorStr )
        : SymbolException( buildDesc( typeName, errorStr ) )
        {}

private:

    std::string buildDesc( const std::string &typeName, const std::string  &errorStr )
    {
        std::stringstream   sstr;
        sstr << typeName << " : " << errorStr;
        return sstr.str();
    }
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

class ImplementException : public DbgException
{
public:

    ImplementException( const std::string &file, int line, const std::string &msg ) :
         DbgException( buildDesc(file,line, msg) )    
         {}

private:

    std::string buildDesc( const std::string &file, int line, const std::string &msg )
    {
        std::stringstream sstream;
        sstream << "File: " << file << " Line: " << line << "  " << msg;
        return sstream.str();
    }

};

/////////////////////////////////////////////////////////////////////////////////

}; // namespace pykd

