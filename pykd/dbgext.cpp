#include "stdafx.h"

#include <engextcpp.hpp>

#include <boost/python.hpp>
#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

#include "dbgprint.h"
#include "dbgreg.h"
#include "dbgtype.h"    
#include "dbgmodule.h"
#include "dbgsym.h"
#include "dbgmem.h"
#include "dbgsystem.h"
#include "dbgcmd.h"

/////////////////////////////////////////////////////////////////////////////////

BOOST_PYTHON_MODULE( pykd )
{
    boost::python::def( "dprint", &DbgPrint::dprint );
    boost::python::def( "dprintln", &DbgPrint::dprintln );
    boost::python::def( "dbgCommand", &dbgCommand );
    boost::python::def( "is64bitSystem", is64bitSystem );
    boost::python::def( "reg", &loadRegister );
    boost::python::def( "typedVar", &loadTypedVar );
    boost::python::def( "containingRecord", &containingRecord );
    boost::python::def( "loadModule", &loadModule );
    boost::python::def( "findSymbol", &findSymbolForAddress );
    boost::python::def( "getOffset", &findAddressForSymbol );
    boost::python::def( "findModule", &findModule );
    boost::python::def( "addr64", &addr64 );
    boost::python::def( "compareMemory", &compareMemory );
    boost::python::class_<typedVarClass>( "typedVarClass" )
        .def("getAddress", &typedVarClass::getAddress );
    boost::python::class_<dbgModuleClass>( "dbgModuleClass" )
        .add_property("begin", &dbgModuleClass::getBegin )
        .add_property("end", &dbgModuleClass::getEnd )
        .def("contain", &dbgModuleClass::contain );
}    

/////////////////////////////////////////////////////////////////////////////////

class EXT_CLASS : public ExtExtension
{
public:

	virtual HRESULT Initialize(void) {
	
		HRESULT  hr = ExtExtension::Initialize();
		if ( FAILED( hr ) )
		    return hr;
		    
        Py_Initialize();

        PyImport_AppendInittab("pykd",initpykd );
        
        return   hr;
	}
	
    virtual void Uninitialize(void) {
    
        Py_Finalize();  
    }   
	

public:
	EXT_COMMAND_METHOD( info );
	EXT_COMMAND_METHOD( exec );
};

EXT_DECLARE_GLOBALS();

/////////////////////////////////////////////////////////////////////////////////

EXT_COMMAND(
	info,
	"Python Info",
	"" )
{
	Out( "Python Info" );
}

/////////////////////////////////////////////////////////////////////////////////

EXT_COMMAND(
    exec,
    "Execute python code",
    "{f;b;;quite mode}{;x}" )
{
    bool   fromFile = false;

	if ( HasArg( "f" ) )
			fromFile = true;
			
    try {
    

        boost::python::object       main =  boost::python::import("__main__");

        boost::python::object       global(main.attr("__dict__"));

        boost::python::object       result;
        
        if ( fromFile )
        {
            result =  boost::python::exec_file(  GetUnnamedArgStr( 0 ), global, global );
        }
        else
        {
            result =  boost::python::exec( GetUnnamedArgStr( 0 ), global, global );
        }     
    }
    catch( boost::python::error_already_set const & )
    {
        // ошибка в скрипте
        PyObject    *errtype = NULL, *errvalue = NULL, *traceback = NULL;
        
        PyErr_Fetch( &errtype, &errvalue, &traceback );
        
        if(errvalue != NULL) 
        {
            PyObject *s = PyObject_Str(errvalue);
            
            DbgPrint::dprintln( PyString_AS_STRING( s ) );

            Py_DECREF(s);
        }

        Py_XDECREF(errvalue);
        Py_XDECREF(errtype);
        Py_XDECREF(traceback);        
    }    
    catch(...)
    {           
    }       

  
 }  

/////////////////////////////////////////////////////////////////////////////////  