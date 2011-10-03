#include "stdafx.h"

#include <dbgeng.h>

#include <dia2.h>

#include "module.h"
#include "diawrapper.h"
#include "dbgclient.h"

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
        .def( "loadModule", &pykd::DebugClient::loadModule, 
            "Return instance of Module class" )
        .def( "findModule", &pykd::DebugClient::findModule, 
            "Return instance of the Module class which posseses specified address" );

//    python::def( "createDbgClient", pykd::DebugClient::createDbgClient, 
//        "create a new instance of the dbgClient class" );
    python::def( "loadDump", &loadDump,
        "Load crash dump (only for console)");
    python::def( "startProcess", &startProcess,
        "Start process for debugging (only for console)"); 
    python::def( "attachProcess", &attachProcess,
        "Attach debugger to a exsisting process" );
    python::def( "attachKernel", &attachKernel,
        "Attach debugger to a kernel target" );
    python::def( "loadModule", &loadModule,
        "Return instance of Module class"  );
    python::def( "findModule", &findModule,
        "Return instance of the Module class which posseses specified address" );


    python::class_<pykd::TypeInfo>("typeInfo", "Class representing typeInfo", python::no_init );

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



        //.def("symbols", &pykd::Module::getSymbols,
        //    "Return list of all symbols of the module" );

        
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

class WindbgGlobalSession 
{
public:

    static
    boost::python::object
    global() {
        return windbgGlobalSession->main.attr("__dict__");
    }
    
    static 
    VOID
    StartWindbgSession() {
        if ( 1 == InterlockedIncrement( &sessionCount ) )
        {
            windbgGlobalSession = new WindbgGlobalSession();
        }
    }
    
    static
    VOID
    StopWindbgSession() {
        if ( 0 == InterlockedDecrement( &sessionCount ) )
        {
            delete windbgGlobalSession;
            windbgGlobalSession = NULL;
        }            
    }
    
    static
    bool isInit() {
        return windbgGlobalSession != NULL;
    }
    

private:

    WindbgGlobalSession() {
                 
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
     }
    
    ~WindbgGlobalSession() {
    }
   
    python::object                  main;

    static volatile LONG            sessionCount;      
    
    static WindbgGlobalSession      *windbgGlobalSession;     

};   

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

    PyThreadState   *globalInterpreter = PyThreadState_Swap( NULL );
    PyThreadState   *localInterpreter = Py_NewInterpreter();

    try {

    
    }
    catch(...)
    {      
    //    dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "unexpected error" );         
    }    

    Py_EndInterpreter( localInterpreter ); 
    PyThreadState_Swap( globalInterpreter );

    DebugClient::setDbgClientCurrent( oldClient );

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

HRESULT 
CALLBACK
pycmd( PDEBUG_CLIENT4 client, PCSTR args )
{
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
