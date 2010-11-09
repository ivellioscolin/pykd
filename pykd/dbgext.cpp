#include "stdafx.h"

#include <wdbgexts.h>

#include <vector>
#include <string>

#include <boost/python.hpp>
#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/tokenizer.hpp>
#include <boost/python/overloads.hpp>

#include "dbgext.h"
#include "dbgprint.h"
#include "dbgreg.h"
#include "dbgtype.h"    
#include "dbgmodule.h"
#include "dbgsym.h"
#include "dbgmem.h"
#include "dbgsystem.h"
#include "dbgcmd.h"
#include "dbgdump.h"
#include "dbgexcept.h"
#include "dbgsession.h"
#include "dbgcallback.h"
#include "dbgstack.h"
#include "dbgpath.h"

/////////////////////////////////////////////////////////////////////////////////

// указатель на текущйи интерфейс
DbgExt    *dbgExt = NULL;


/////////////////////////////////////////////////////////////////////////////////

class WindbgGlobalSession 
{
public:

    WindbgGlobalSession() {
        main = boost::python::import("__main__");
    }
    
    boost::python::object
    global() {
        return main.attr("__dict__");
    }

private:
   
    boost::python::object       main;
   
};   

WindbgGlobalSession     *windbgGlobalSession = NULL; 

/////////////////////////////////////////////////////////////////////////////////

BOOST_PYTHON_FUNCTION_OVERLOADS( loadBytes, loadArray<unsigned char>, 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadWords, loadArray<unsigned short>, 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadDWords, loadArray<unsigned long>, 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadQWords, loadArray<unsigned __int64> , 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignBytes, loadArray<char> , 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignWords, loadArray<short> , 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignDWords, loadArray<long> , 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignQWords, loadArray<__int64>, 2, 3 )

BOOST_PYTHON_FUNCTION_OVERLOADS( compareMemoryOver, compareMemory, 3, 4 )

BOOST_PYTHON_MODULE( pykd )
{
    boost::python::def( "createSession", &dbgCreateSession );
    boost::python::def( "isSessionStart", &dbgIsSessionStart );
    boost::python::def( "symbolsPath", &dbgSymPath );
    boost::python::def( "dprint", &DbgPrint::dprint );
    boost::python::def( "dprintln", &DbgPrint::dprintln );
    boost::python::def( "loadDump", &dbgLoadDump );
    boost::python::def( "dbgCommand", &dbgCommand );
    boost::python::def( "is64bitSystem", is64bitSystem );
    boost::python::def( "ptrSize", ptrSize );
    boost::python::def( "reg", &loadRegister );
    boost::python::def( "typedVar", &loadTypedVar );
    boost::python::def( "typedVarList", &loadTypedVarList );
    boost::python::def( "containingRecord", &containingRecord );
    boost::python::def( "sizeof", &sizeofType );
    boost::python::def( "loadModule", &loadModule );
    boost::python::def( "findSymbol", &findSymbolForAddress );
    boost::python::def( "getOffset", &findAddressForSymbol );
    boost::python::def( "findModule", &findModule );
    boost::python::def( "addr64", &addr64 );
    boost::python::def( "loadBytes", loadArray<unsigned char>, loadBytes( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadWords", &loadArray<unsigned short>, loadWords( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadDWords", &loadArray<unsigned long>, loadDWords( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadQWords", &loadArray<unsigned __int64>, loadQWords( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadSignBytes", &loadArray<char>, loadSignBytes( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadSignWords", &loadArray<short>, loadSignWords( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadSignDWords", &loadArray<long>, loadSignDWords( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadSignQWords", &loadArray<__int64>, loadSignQWords( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadPtrs", &loadPtrArray );
    boost::python::def( "loadUnicodeString", &loadUnicodeStr );
    boost::python::def( "loadAnsiString", &loadAnsiStr );   
    boost::python::def( "loadCStr", &loadCStr );
    boost::python::def( "loadWStr", &loadWStr );
    boost::python::def( "loadLinkedList", &loadLinkedList ); 
    boost::python::def( "ptrByte", &loadByPtr<unsigned char> );
    boost::python::def( "ptrSignByte", &loadByPtr<char> );
    boost::python::def( "ptrWord", &loadByPtr<unsigned short> );
    boost::python::def( "ptrSignWord", &loadByPtr<short> );
    boost::python::def( "ptrDWord", &loadByPtr<unsigned long> );
    boost::python::def( "ptrSignDWord", &loadByPtr<long> );
    boost::python::def( "ptrQWord", &loadByPtr<unsigned __int64> );
    boost::python::def( "ptrSignQWord", &loadByPtr<__int64> );
    boost::python::def( "ptrPtr", &loadPtrByPtr );    
    boost::python::def( "compareMemory", &compareMemory, compareMemoryOver( boost::python::args( "addr1", "addr2", "length", "phyAddr" ), "" ) );
    boost::python::def( "getCurrentStack", &getCurrentStack );
    boost::python::def( "reloadSymbols", &reloadSymbols );
    boost::python::def( "getPdbFile", &getPdbFile );
    boost::python::def( "getImplicitThread", &getImplicitThread );
    boost::python::def( "setImplicitThread", &setImplicitThread );
    boost::python::class_<typedVarClass>( "typedVarClass" )
        .def("getAddress", &typedVarClass::getAddress )
        .def("sizeof", &typedVarClass::size );
    boost::python::class_<dbgModuleClass>( "dbgModuleClass" )
        .def("begin", &dbgModuleClass::getBegin )
        .def("end", &dbgModuleClass::getEnd )
        .def("name", &dbgModuleClass::getName )
        .def("contain", &dbgModuleClass::contain );
    boost::python::class_<dbgExtensionClass>( 
            "dbgExtensionClass",
            "dbgExtensionClass",
             boost::python::init<const char*>( boost::python::args("path"), "__init__  dbgExtensionClass" ) ) 
        .def("call", &dbgExtensionClass::call );    
    boost::python::class_<dbgStackFrameClass>( "dbgStackFrameClass", "dbgStackFrameClass" )
        .def_readonly( "instructionOffset", &dbgStackFrameClass::InstructionOffset )
        .def_readonly( "returnOffset", &dbgStackFrameClass::ReturnOffset )
        .def_readonly( "frameOffset", &dbgStackFrameClass::FrameOffset )
        .def_readonly( "stackOffset", &dbgStackFrameClass::StackOffset )
        .def_readonly( "frameNumber", &dbgStackFrameClass::FrameNumber );
}    

/////////////////////////////////////////////////////////////////////////////////

HRESULT
CALLBACK
DebugExtensionInitialize(
    OUT PULONG  Version,
    OUT PULONG  Flags )
{
    *Version = DEBUG_EXTENSION_VERSION( 1, 0 );
    *Flags = 0;
     
    PyImport_AppendInittab("pykd", initpykd ); 
       
    Py_Initialize();
    
    windbgGlobalSession = new WindbgGlobalSession();
    
    dbgSessionStarted = true;                
    
    return S_OK; 
}
    
    
VOID
CALLBACK
DebugExtensionUninitialize()
{
    delete windbgGlobalSession;
    windbgGlobalSession = NULL;

    Py_Finalize();  
}


void
SetupDebugEngine( IDebugClient4 *client, DbgExt *dbgExt  )
{
    client->QueryInterface( __uuidof(IDebugClient), (void **)&dbgExt->client );
    client->QueryInterface( __uuidof(IDebugClient4), (void **)&dbgExt->client4 );
    
    
    client->QueryInterface( __uuidof(IDebugControl), (void **)&dbgExt->control );
    
    client->QueryInterface( __uuidof(IDebugRegisters), (void **)&dbgExt->registers );
    
    client->QueryInterface( __uuidof(IDebugSymbols), (void ** )&dbgExt->symbols );
    client->QueryInterface( __uuidof(IDebugSymbols2), (void ** )&dbgExt->symbols2 );    
    client->QueryInterface( __uuidof(IDebugSymbols3), (void ** )&dbgExt->symbols3 );      
    
    client->QueryInterface( __uuidof(IDebugDataSpaces), (void **)&dbgExt->dataSpaces );
    client->QueryInterface( __uuidof(IDebugDataSpaces4), (void **)&dbgExt->dataSpaces4 );
    
    client->QueryInterface( __uuidof(IDebugAdvanced2), (void **)&dbgExt->advanced2 );
    
    client->QueryInterface( __uuidof(IDebugSystemObjects2), (void**)&dbgExt->system2 );
}
    
/////////////////////////////////////////////////////////////////////////////////    
    
HRESULT 
CALLBACK
py( PDEBUG_CLIENT4 client, PCSTR args)
{
    try {
    
        DbgExt      ext = { 0 };
    
        SetupDebugEngine( client, &ext );  
        dbgExt = &ext;        
        
        boost::python::object       main =  boost::python::import("__main__");

        boost::python::object       global(main.attr("__dict__"));

        boost::python::object       result;
       
        // разбор параметров
        typedef  boost::escaped_list_separator<char>    char_separator_t;
        typedef  boost::tokenizer< char_separator_t >   char_tokenizer_t;  
        
        std::string                 argsStr( args );
        
        char_tokenizer_t            token( argsStr , char_separator_t( "", " \t", "\"" ) );
        std::vector<std::string>    argsList;
        
        for ( char_tokenizer_t::iterator   it = token.begin(); it != token.end(); ++it )
        {
            if ( *it != "" )
                argsList.push_back( *it );
        }            
            
        if ( argsList.size() == 0 )
            return S_OK;            
            
        if ( argsList.size() > 1 )     
        {
            char    **pythonArgs = new char* [ argsList.size() - 1 ];
         
            for ( size_t  i = 0; i < argsList.size() - 1; ++i )
                pythonArgs[i] = const_cast<char*>( argsList[i+1].c_str() );
                
            PySys_SetArgv( (int)argsList.size() - 1, pythonArgs );

            delete[]  pythonArgs;
        }            
        else
        {
            char   *emptyParam = "";
        
            PySys_SetArgv( 1, &emptyParam );
        } 
        
        // найти путь к файлу
        std::string     fullFileName;
        std::string     filePath;
        
        if ( dbgPythonPath.findPath( argsList[0], fullFileName, filePath ) )
        {
            DWORD       oldCurDirLen = GetCurrentDirectoryA( 0, NULL );

            char        *oldCurDirCstr = new char[ oldCurDirLen ];
            
            GetCurrentDirectoryA( oldCurDirLen, oldCurDirCstr );
            
            SetCurrentDirectoryA( filePath.c_str() );
            
            try {                  
        
                result =  boost::python::exec_file( fullFileName.c_str(), global, global );
                
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
            
            SetCurrentDirectoryA( oldCurDirCstr );               
            
            delete[] oldCurDirCstr;
                
        }
        else
        {
            DbgPrint::dprintln( "script file not found" ); 
        }           
    }
   
    catch(...)
    {           
    }     
    
    return S_OK;  
}

/////////////////////////////////////////////////////////////////////////////////  

HRESULT 
CALLBACK
pycmd( PDEBUG_CLIENT4 client, PCSTR args )
{
    try {
    
        DbgExt      ext = { 0 };
    
        SetupDebugEngine( client, &ext );  
        dbgExt = &ext;   
        
        if ( !std::string( args ).empty() )
        {
            try {
                boost::python::exec( args, windbgGlobalSession->global(), windbgGlobalSession->global() );
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
        }
        else
        {
            char        str[100];
            ULONG       inputSize;
            bool        stopInput = false;
            
            do {
            
                std::string     output;
                
                dbgExt->control->Output( DEBUG_OUTPUT_NORMAL, ">>>" );
                    
                do {    
                                
                    OutputReader        outputReader( dbgExt->client );                    
                    
                    HRESULT   hres = dbgExt->control->Input( str, sizeof(str), &inputSize );
                
                    if ( FAILED( hres ) || std::string( str ) == "" )
                    {
                       stopInput = true;
                       break;
                    }                       
                    
                } while( FALSE );                    
                
                if ( !stopInput )
                    try {
                        boost::python::exec( str, windbgGlobalSession->global(), windbgGlobalSession->global() );
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
                    
            } while( !stopInput );                                
        }
    }
  
    catch(...)
    {           
    }     
    
    return S_OK;          
            
}

///////////////////////////////////////////////////////////////////////////////// 

HRESULT 
CALLBACK
pythonpath( PDEBUG_CLIENT4 client, PCSTR args )
{
    DbgExt      ext = { 0 };
    
    SetupDebugEngine( client, &ext );  
    dbgExt = &ext;   

    DbgPrint::dprintln( dbgPythonPath.getStr() );
   
    return S_OK;      
}

///////////////////////////////////////////////////////////////////////////////// 
