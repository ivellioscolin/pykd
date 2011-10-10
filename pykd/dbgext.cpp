#include "stdafx.h"

#include <dbgeng.h>
#include <dia2.h>

#include <boost/tokenizer.hpp>

#include "windbg.h"
#include "module.h"
#include "diawrapper.h"
#include "dbgclient.h"
#include "dbgio.h"
#include "dbgpath.h"
#include "dbgcmd.h"

using namespace pykd;

////////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(
  __in  HINSTANCE /*hinstDLL*/,
  __in  DWORD fdwReason,
  __in  LPVOID /*lpvReserved*/
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        CoInitialize(NULL);
        break;

    case DLL_PROCESS_DETACH:
        CoUninitialize();
        break;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

static python::dict genDict(const pyDia::Symbol::ValueNameEntry srcValues[], size_t cntValues)
{
    python::dict resDict;
    for (size_t i = 0; i < cntValues; ++i)
        resDict[srcValues[i].first] = srcValues[i].second;
    return resDict;
}

////////////////////////////////////////////////////////////////////////////////

BOOST_PYTHON_FUNCTION_OVERLOADS( dprint_, dprint, 1, 2 )
BOOST_PYTHON_FUNCTION_OVERLOADS( dprintln_, dprintln, 1, 2 )

#define DEF_PY_CONST_ULONG(x)    \
    python::scope().attr(#x) = ULONG(##x)

BOOST_PYTHON_MODULE( pykd )
{
    python::class_<pykd::DebugClient, pykd::DebugClientPtr>("dbgClient", "Class representing a debugging session", python::no_init  )
        .def( "loadDump", &pykd::DebugClient::loadDump,
            "Load crash dump" )
        .def( "startProcess", &pykd::DebugClient::startProcess, 
            "Start process for debugging" )
        .def( "attachProcess", &pykd::DebugClient::attachProcess,
            "Attach debugger to a exsisting process" )
        .def( "attachKernel", &pykd::DebugClient::attachKernel, 
            "Attach debugger to a target's kernel" )
        .def( "expr", &pykd::DebugClient::evaluate,
            "Evaluate windbg expression" )
        .def ( "loadExt", &pykd::DebugClient::loadExtension,
            "Load a debuger extension" )
        .def( "loadModule", &pykd::DebugClient::loadModule, 
            "Return instance of Module class" )
        .def( "findModule", &pykd::DebugClient::findModule, 
            "Return instance of the Module class which posseses specified address" )
        .def( "dbgCommand", &pykd::DebugClient::dbgCommand,
             "Run a debugger's command and return it's result as a string" )
        .def( "dprint", &pykd::DebugClient::dprint,
            "Print out string. If dml = True string is printed with dml highlighting ( only for windbg )" )
        .def( "dprintln", &pykd::DebugClient::dprintln,
            "Print out string and insert end of line symbol. If dml = True string is printed with dml highlighting ( only for windbg )" );

    python::def( "createDbgClient", (DebugClientPtr(*)())&pykd::DebugClient::createDbgClient, 
        "create a new instance of the dbgClient class" );
    python::def( "loadDump", &pykd::loadDump,
        "Load crash dump (only for console)");
    python::def( "startProcess", &pykd::startProcess,
        "Start process for debugging (only for console)"); 
    python::def( "attachProcess", &pykd::attachProcess,
        "Attach debugger to a exsisting process" );
    python::def( "attachKernel", &pykd::attachKernel,
        "Attach debugger to a kernel target" );
    python::def( "expr", &pykd::evaluate,
        "Evaluate windbg expression" );
    python::def( "loadExt", &pykd::loadExtension,
        "Load a debuger extension" );
    python::def( "loadModule", &pykd::loadModule,
        "Return instance of Module class"  );
    python::def( "findModule", &pykd::findModule,
        "Return instance of the Module class which posseses specified address" );
    python::def( "dbgCommand", &pykd::dbgCommand,    
        "Run a debugger's command and return it's result as a string" ),
    python::def( "dprint", &pykd::dprint, dprint_( boost::python::args( "str", "dml" ), 
        "Print out string. If dml = True string is printed with dml highlighting ( only for windbg )" ) );
    python::def( "dprintln", &pykd::dprintln, dprintln_( boost::python::args( "str", "dml" ), 
        "Print out string and insert end of line symbol. If dml = True string is printed with dml highlighting ( only for windbg )" ) );
    
    python::class_<pykd::TypeInfo>("typeInfo", "Class representing typeInfo", python::no_init )
        .def( "name", &pykd::TypeInfo::getName )
        .def( "size", &pykd::TypeInfo::getSize )
        .def( "offset", &pykd::TypeInfo::getOffset )
        .def( "field", &pykd::TypeInfo::getField )
        .def( "__getattr__", &pykd::TypeInfo::getField );

    python::class_<pykd::Module>("module", "Class representing executable module", python::no_init )
        .def("begin", &pykd::Module::getBase,
             "Return start address of the module" )
        .def("end", &pykd::Module::getEnd,
             "Return end address of the module" )
        .def("size", &pykd::Module::getSize,
              "Return size of the module" )
        .def("name", &pykd::Module::getName,
             "Return name of the module" )      
        .def("image", &pykd::Module::getImageName,
             "Return name of the image of the module" )
        .def("pdb", &pykd::Module::getPdbName,
             "Return the full path to the module's pdb file ( symbol information )" )
        .def("reload", &pykd::Module::reloadSymbols,
            "(Re)load symbols for the module" )
        .def("offset", &pykd::Module::getSymbol,
            "Return offset of the symbol" )
        .def("rva", &pykd::Module::getSymbolRva,
            "Return rva of the symbol" )
        .def("type", &pykd::Module::getTypeByName,
            "Return typeInfo class by type name" )
        .def("__getattr__", &pykd::Module::getSymbol,
            "Return address of the symbol" );

    python::class_<DbgOut>( "dout", "dout", python::no_init )
        .def( "write", &pykd::DbgOut::write );
        
    python::class_<DbgIn>( "din", "din", python::no_init )
        .def( "readline", &pykd::DbgIn::readline );

    python::class_<DbgExtension, pykd::DbgExtensionPtr>("ext", python::no_init )
        .def( "call", &pykd::DbgExtension::call,
            "Call debug extension command end return it's result as a string" );
        
    python::def( "diaLoadPdb", &pyDia::GlobalScope::loadPdb, 
        "Open pdb file for quering debug symbols. Return DiaSymbol of global scope");

    python::class_<pyDia::Symbol, pyDia::SymbolPtr>(
        "DiaSymbol", "class wrapper for MS DIA Symbol", python::no_init )
        .def( "findEx", &pyDia::Symbol::findChildrenEx, 
            "Retrieves the children of the symbol" )
        .def( "find", &pyDia::Symbol::findChildren, 
            "Retrieves the children of the symbol" )
        .def( "size", &pyDia::Symbol::getSize, 
            "Retrieves the number of bits or bytes of memory used by the object represented by this symbol" )
        .def( "name", &pyDia::Symbol::getName, 
            "Retrieves the name of the symbol" )
        .def( "type", &pyDia::Symbol::getType, 
            "Retrieves the symbol that represents the type for this symbol" )
        .def( "indexType", &pyDia::Symbol::getIndexType, 
            "Retrieves a reference to the class parent of the symbol" )
        .def( "rva", &pyDia::Symbol::getRva,
            "Retrieves the relative virtual address (RVA) of the location")
        .def( "symTag", &pyDia::Symbol::getSymTag, 
            "Retrieves the symbol type classifier: SymTagXxx" )
        .def( "locType", &pyDia::Symbol::getLocType, 
            "Retrieves the location type of a data symbol: LocIsXxx" )
        .def( "offset", &pyDia::Symbol::getOffset, 
            "Retrieves the offset of the symbol location" )
        .def( "count", &pyDia::Symbol::getCount, 
            "Retrieves the number of items in a list or array" )
        .def( "value", &pyDia::Symbol::getValue,
            "Retrieves the value of a constant")
        .def( "isBasic", &pyDia::Symbol::isBasicType,
            "Retrieves a flag of basic type for symbol")
        .def( "baseType", &pyDia::Symbol::getBaseType,
            "Retrieves the base type for this symbol")
        .def( "bitPos", &pyDia::Symbol::getBitPosition,
            "Retrieves the base type for this symbol")
        .def( "indexId", &pyDia::Symbol::getIndexId,
            "Retrieves the unique symbol identifier")
        .def( "udtKind", &pyDia::Symbol::getUdtKind,
            "Retrieves the variety of a user-defined type")
        .def("registerId", &pyDia::Symbol::getRegisterId,
            "Retrieves the register designator of the location:\n"
            "CV_REG_XXX (for IMAGE_FILE_MACHINE_I386) or CV_AMD64_XXX (for IMAGE_FILE_MACHINE_AMD64)")
        .def("machineType", &pyDia::Symbol::getMachineType, 
            "Retrieves the type of the target CPU: IMAGE_FILE_MACHINE_XXX")
        .def( "__str__", &pyDia::Symbol::print)
        .def("__getitem__", &pyDia::Symbol::getChildByName)
        .def("__len__", &pyDia::Symbol::getChildCount )
        .def("__getitem__", &pyDia::Symbol::getChildByIndex);

    python::class_<pyDia::GlobalScope, pyDia::GlobalScopePtr, python::bases<pyDia::Symbol> >(
        "DiaScope", "class wrapper for MS DIA Symbol", python::no_init )
        .def("findByRva", &pyDia::GlobalScope::findByRva, 
            "Find symbol by RVA. Return tuple: (DiaSymbol, offset)")
        .def("symbolById", &pyDia::GlobalScope::getSymbolById, 
            "Retrieves a symbol by its unique identifier: DiaSymbol::indexId()");

    // CPU type:
    DEF_PY_CONST_ULONG(IMAGE_FILE_MACHINE_I386);
    DEF_PY_CONST_ULONG(IMAGE_FILE_MACHINE_IA64);
    DEF_PY_CONST_ULONG(IMAGE_FILE_MACHINE_AMD64);

    // type of symbol
    DEF_PY_CONST_ULONG(SymTagNull);
    DEF_PY_CONST_ULONG(SymTagExe);
    DEF_PY_CONST_ULONG(SymTagCompiland);
    DEF_PY_CONST_ULONG(SymTagCompilandDetails);
    DEF_PY_CONST_ULONG(SymTagCompilandEnv);
    DEF_PY_CONST_ULONG(SymTagFunction);
    DEF_PY_CONST_ULONG(SymTagBlock);
    DEF_PY_CONST_ULONG(SymTagData);
    DEF_PY_CONST_ULONG(SymTagAnnotation);
    DEF_PY_CONST_ULONG(SymTagLabel);
    DEF_PY_CONST_ULONG(SymTagPublicSymbol);
    DEF_PY_CONST_ULONG(SymTagUDT);
    DEF_PY_CONST_ULONG(SymTagEnum);
    DEF_PY_CONST_ULONG(SymTagFunctionType);
    DEF_PY_CONST_ULONG(SymTagPointerType);
    DEF_PY_CONST_ULONG(SymTagArrayType);
    DEF_PY_CONST_ULONG(SymTagBaseType);
    DEF_PY_CONST_ULONG(SymTagTypedef);
    DEF_PY_CONST_ULONG(SymTagBaseClass);
    DEF_PY_CONST_ULONG(SymTagFriend);
    DEF_PY_CONST_ULONG(SymTagFunctionArgType);
    DEF_PY_CONST_ULONG(SymTagFuncDebugStart);
    DEF_PY_CONST_ULONG(SymTagFuncDebugEnd);
    DEF_PY_CONST_ULONG(SymTagUsingNamespace);
    DEF_PY_CONST_ULONG(SymTagVTableShape);
    DEF_PY_CONST_ULONG(SymTagVTable);
    DEF_PY_CONST_ULONG(SymTagCustom);
    DEF_PY_CONST_ULONG(SymTagThunk);
    DEF_PY_CONST_ULONG(SymTagCustomType);
    DEF_PY_CONST_ULONG(SymTagManagedType);
    DEF_PY_CONST_ULONG(SymTagDimension);
    python::scope().attr("diaSymTagName") = 
        genDict(pyDia::Symbol::symTagName, _countof(pyDia::Symbol::symTagName));

    // search options for symbol and file names
    DEF_PY_CONST_ULONG(nsfCaseSensitive);
    DEF_PY_CONST_ULONG(nsfCaseInsensitive);
    DEF_PY_CONST_ULONG(nsfFNameExt);
    DEF_PY_CONST_ULONG(nsfRegularExpression);
    DEF_PY_CONST_ULONG(nsfUndecoratedName);
    DEF_PY_CONST_ULONG(nsCaseSensitive);
    DEF_PY_CONST_ULONG(nsCaseInsensitive);
    DEF_PY_CONST_ULONG(nsFNameExt);
    DEF_PY_CONST_ULONG(nsRegularExpression);
    DEF_PY_CONST_ULONG(nsCaseInRegularExpression);

    // location type
    DEF_PY_CONST_ULONG(LocIsNull);
    DEF_PY_CONST_ULONG(LocIsStatic);
    DEF_PY_CONST_ULONG(LocIsTLS);
    DEF_PY_CONST_ULONG(LocIsRegRel);
    DEF_PY_CONST_ULONG(LocIsThisRel);
    DEF_PY_CONST_ULONG(LocIsEnregistered);
    DEF_PY_CONST_ULONG(LocIsBitField);
    DEF_PY_CONST_ULONG(LocIsSlot);
    DEF_PY_CONST_ULONG(LocIsIlRel);
    DEF_PY_CONST_ULONG(LocInMetaData);
    DEF_PY_CONST_ULONG(LocIsConstant);
    python::scope().attr("diaLocTypeName") = 
        genDict(pyDia::Symbol::locTypeName, _countof(pyDia::Symbol::locTypeName));

    DEF_PY_CONST_ULONG(btNoType);
    DEF_PY_CONST_ULONG(btVoid);
    DEF_PY_CONST_ULONG(btChar);
    DEF_PY_CONST_ULONG(btWChar);
    DEF_PY_CONST_ULONG(btInt);
    DEF_PY_CONST_ULONG(btUInt);
    DEF_PY_CONST_ULONG(btFloat);
    DEF_PY_CONST_ULONG(btBCD);
    DEF_PY_CONST_ULONG(btBool);
    DEF_PY_CONST_ULONG(btLong);
    DEF_PY_CONST_ULONG(btULong);
    DEF_PY_CONST_ULONG(btCurrency);
    DEF_PY_CONST_ULONG(btDate);
    DEF_PY_CONST_ULONG(btVariant);
    DEF_PY_CONST_ULONG(btComplex);
    DEF_PY_CONST_ULONG(btBit);
    DEF_PY_CONST_ULONG(btBSTR);
    DEF_PY_CONST_ULONG(btHresult);
    python::scope().attr("diaBasicType") = 
        genDict(pyDia::Symbol::basicTypeName, pyDia::Symbol::cntBasicTypeName);

    DEF_PY_CONST_ULONG(UdtStruct);
    DEF_PY_CONST_ULONG(UdtClass);
    DEF_PY_CONST_ULONG(UdtUnion);
    python::scope().attr("diaUdtKind") = 
        genDict(pyDia::Symbol::udtKindName, pyDia::Symbol::cntUdtKindName);

    // i386/amd64 cpu registers
#include "diaregs.h"
    python::scope().attr("diaI386Regs") = 
        genDict(pyDia::Symbol::i386RegName, pyDia::Symbol::cntI386RegName);
    python::scope().attr("diaAmd64Regs") = 
        genDict(pyDia::Symbol::amd64RegName, pyDia::Symbol::cntAmd64RegName);

    // exception:

    // base exception
    python::class_<pykd::DbgException>  dbgExceptionClass( "BaseException",
        "Pykd base exception class",
        python::no_init );
    dbgExceptionClass
        .def( python::init<std::string>( python::args("desc"), "constructor" ) )
        .def( "desc", &pykd::DbgException::getDesc,
            "Get exception description" )
        .def( "__str__", &pykd::DbgException::print);
    pykd::DbgException::setTypeObject( dbgExceptionClass.ptr() );
    boost::python::register_exception_translator<pykd::DbgException>( 
        &pykd::DbgException::exceptionTranslate );

    // DIA exceptions
    python::class_<pyDia::Exception, python::bases<DbgException> > diaException( 
        "DiaException", "Debug interface access exception",
        python::no_init );
    diaException
        .def( "hres", &pyDia::Exception::getRes );
    pyDia::Exception::setTypeObject( diaException.ptr() );
    boost::python::register_exception_translator<pyDia::Exception>( 
        &pyDia::Exception::exceptionTranslate );
}

#undef DEF_PY_CONST_ULONG

////////////////////////////////////////////////////////////////////////////////

WindbgGlobalSession::WindbgGlobalSession() {
                 
    PyImport_AppendInittab("pykd", initpykd ); 

    PyEval_InitThreads();

    Py_Initialize();    

    main = boost::python::import("__main__");
    
    python::object   main_namespace = main.attr("__dict__");

    // делаем аналог from pykd import *        
    python::object   pykd = boost::python::import( "pykd" );
    
    python::dict     pykd_namespace( pykd.attr("__dict__") ); 
    
    python::list     iterkeys( pykd_namespace.iterkeys() );
    
    for (int i = 0; i < boost::python::len(iterkeys); i++)
    {
        std::string     key = boost::python::extract<std::string>(iterkeys[i]);
               
        main_namespace[ key ] = pykd_namespace[ key ];
    }   

    pyState = PyEval_SaveThread();
}


volatile LONG            WindbgGlobalSession::sessionCount = 0;

WindbgGlobalSession     *WindbgGlobalSession::windbgGlobalSession = NULL; 

/////////////////////////////////////////////////////////////////////////////////

HRESULT
CALLBACK
DebugExtensionInitialize(
    OUT PULONG  Version,
    OUT PULONG  Flags )
{
    *Version = DEBUG_EXTENSION_VERSION( 1, 0 );
    *Flags = 0;

    WindbgGlobalSession::StartWindbgSession();

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

VOID
CALLBACK
DebugExtensionUninitialize()
{
    WindbgGlobalSession::StopWindbgSession();
}

////////////////////////////////////////////////////////////////////////////////

HRESULT 
CALLBACK
py( PDEBUG_CLIENT4 client, PCSTR args )
{
    DebugClientPtr      dbgClient = DebugClient::createDbgClient( client );
    DebugClientPtr      oldClient = DebugClient::setDbgClientCurrent( dbgClient );

    WindbgGlobalSession::RestorePyState();

    PyThreadState   *globalInterpreter = PyThreadState_Swap( NULL );
    PyThreadState   *localInterpreter = Py_NewInterpreter();

    try {

        // получаем достпу к глобальному мапу ( нужен для вызова exec_file )
        python::object       main =  python::import("__main__");

        python::object       global(main.attr("__dict__"));

        // настраиваем ввод/вывод ( чтобы в скрипте можно было писать print )

        python::object       sys = python::import("sys");
       
        sys.attr("stdout") = python::object( dbgClient->dout() );
        sys.attr("stderr") = python::object( dbgClient->dout() );
        sys.attr("stdin") = python::object( dbgClient->din() );

        // импортируем модуль обработки исключений ( нужен для вывода traceback а )
        boost::python::object       tracebackModule = python::import("traceback");
        
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
            
        char    **pythonArgs = new char* [ argsList.size() ];
     
        for ( size_t  i = 0; i < argsList.size(); ++i )
            pythonArgs[i] = const_cast<char*>( argsList[i].c_str() );
            
        PySys_SetArgv( (int)argsList.size(), pythonArgs );

        delete[]  pythonArgs;       

       // найти путь к файлу
        std::string     scriptName;
        std::string     filePath;
        DbgPythonPath   dbgPythonPath;        
        
        if ( !dbgPythonPath.findPath( argsList[0], scriptName, filePath ) )
        {
            dbgClient->eprintln( L"script file not found" );            
        }
        else
        try {             
      
            python::object       result;
    
            result =  python::exec_file( scriptName.c_str(), global, global );
        }                
        catch( boost::python::error_already_set const & )
        {
            // ошибка в скрипте
            PyObject    *errtype = NULL, *errvalue = NULL, *traceback = NULL;
            
            PyErr_Fetch( &errtype, &errvalue, &traceback );
            
            if(errvalue != NULL) 
            {
                PyObject *errvalueStr= PyUnicode_FromObject(errvalue);         

                dbgClient->eprintln( PyUnicode_AS_UNICODE( errvalueStr ) );

                if ( traceback )
                {
                    python::object    traceObj( python::handle<>( python::borrowed( traceback ) ) );
                    
                    dbgClient->eprintln( L"\nTraceback:" );

                    python::object   pFunc( tracebackModule.attr("format_tb") );
                    python::list     traceList( pFunc( traceObj ) );

                    for ( long i = 0; i < python::len(traceList); ++i )
                        dbgClient->eprintln( python::extract<std::wstring>(traceList[i]) );
                }

                Py_DECREF(errvalueStr);
            }

            Py_XDECREF(errvalue);
            Py_XDECREF(errtype);
            Py_XDECREF(traceback);        
        }  

    }
    catch(...)
    {      
        dbgClient->eprintln( L"unexpected error" );
    }    

    Py_EndInterpreter( localInterpreter ); 
    PyThreadState_Swap( globalInterpreter );

    WindbgGlobalSession::SavePyState();

    DebugClient::setDbgClientCurrent( oldClient );

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

HRESULT 
CALLBACK
pycmd( PDEBUG_CLIENT4 client, PCSTR args )
{
    DebugClientPtr      dbgClient = DebugClient::createDbgClient( client );
    DebugClientPtr      oldClient = DebugClient::setDbgClientCurrent( dbgClient );

    WindbgGlobalSession::RestorePyState();

    
    ULONG    mask = 0;
    client->GetOutputMask( &mask );

    try {

        // перенаправление стандартных потоков ВВ
        python::object       sys = python::import("sys");
       
        sys.attr("stdout") = python::object( DbgOut( client ) );
        sys.attr("stderr") = python::object( DbgOut( client ) );
        sys.attr("stdin") = python::object( DbgIn( client ) );

        client->SetOutputMask( DEBUG_OUTPUT_NORMAL );

        PyRun_String(
            "__import__('code').InteractiveConsole(__import__('__main__').__dict__).interact()", 
            Py_file_input,
            WindbgGlobalSession::global().ptr(),
            WindbgGlobalSession::global().ptr()
            );
    }
    catch(...)
    {      
        dbgClient->eprintln( L"unexpected error" );
    }    

    client->SetOutputMask( mask );

    WindbgGlobalSession::SavePyState();

    DebugClient::setDbgClientCurrent( oldClient );

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
