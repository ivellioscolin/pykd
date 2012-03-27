// 
// Python module initialization
// 

#include "stdafx.h"

#include <dia2.h>

#include "module.h"
#include "diawrapper.h"
#include "dbgclient.h"
#include "dbgio.h"
#include "dbgpath.h"
#include "dbgcmd.h"
#include "dbgevent.h"
#include "typeinfo.h"
#include "typedvar.h"
#include "dbgmem.h"
#include "intbase.h"
#include "process.h"
#include "bpoint.h"
#include "stkframe.h"
#include "pykdver.h"

using namespace pykd;

////////////////////////////////////////////////////////////////////////////////

static const std::string pykdVersion = PYKD_VERSION_BUILD_STR;

////////////////////////////////////////////////////////////////////////////////

static python::dict genDict(const pyDia::Symbol::ValueNameEntry srcValues[], size_t cntValues)
{
    python::dict resDict;
    for (size_t i = 0; i < cntValues; ++i)
        resDict[srcValues[i].first] = srcValues[i].second;
    return resDict;
}

////////////////////////////////////////////////////////////////////////////////

std::string
getDebuggerImage()
{
    std::vector<char>   buffer(MAX_PATH);
    GetModuleFileNameExA( GetCurrentProcess(), NULL, &buffer[0], (DWORD)buffer.size() );
    return std::string( &buffer[0] );
}

////////////////////////////////////////////////////////////////////////////////


BOOST_PYTHON_FUNCTION_OVERLOADS( pyDia_GlobalScope_loadExe, pyDia::GlobalScope::loadExe, 1, 2 );

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( StackFrame_getLocals, StackFrame::getLocals, 0, 1 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( pyDia_Symbol_findChildrenEx, pyDia::Symbol::findChildrenEx, 1, 3 );

#define DEF_PY_CONST_ULONG(x)    \
    python::scope().attr(#x) = ULONG(##x)

BOOST_PYTHON_MODULE( pykd )
{
    python::scope().attr("version") = pykdVersion;

    python::def( "debuggerPath", &getDebuggerImage,
        "Return full path to the process image that uses pykd" );

    python::class_<intBase>( "intBase", "intBase", python::no_init )
        .def( python::init<python::object&>() )
        .def( "__eq__", &intBase::eq )
        .def( "__ne__", &intBase::ne)
        .def( "__lt__", &intBase::lt)
        .def( "__gt__", &intBase::gt )
        .def( "__le__", &intBase::le )
        .def( "__ge__", &intBase::ge )
        .def( "__add__", &intBase::add )
        .def( "__radd__", &intBase::add )
        .def( "__sub__", &intBase::sub )
        .def( "__rsub__", &intBase::rsub )
        .def( "__mul__", &intBase::mul )
        .def( "__rmul__", &intBase::mul )
        .def( "__div__", &intBase::div )
        .def( "__rdiv__", &intBase::rdiv )
        .def( "__mod__", &intBase::mod )
        .def( "__rmod__", &intBase::rmod )
        .def( "__rshift__", &intBase::rshift )
        .def( "__rrshift__", &intBase::rrshift )
        .def( "__lshift__", &intBase::lshift )
        .def( "__rlshift__", &intBase::rlshift )
        .def( "__and__", &intBase::and )
        .def( "__rand__", &intBase::and )
        .def( "__or__", &intBase::or )
        .def( "__ror__", &intBase::or )
        .def( "__xor__", &intBase::xor )
        .def( "__rxor__", &intBase::xor )
        .def( "__neg__", &intBase::neg )
        .def( "__pos__", &intBase::pos ) 
        .def( "__invert__", &intBase::invert ) 
        .def( "__nonzero__", &intBase::nonzero )
        .def( "__str__", &intBase::str )
        .def( "__hex__", &intBase::hex )
        .def( "__long__", &intBase::long_ )
        .def( "__int__", &intBase::int_ )
        .def( "__index__", &intBase::long_ )
        .def( "__hash__", &intBase::long_ );

    python::implicitly_convertible<intBase,ULONG64>();
    python::implicitly_convertible<intBase,LONG64>();

    initDbgClientPyExports();

    python::class_<TypeInfo, TypeInfoPtr, python::bases<intBase>, boost::noncopyable >("typeInfo", "Class representing typeInfo", python::no_init )
        .def( "name", &TypeInfo::getName )
        .def( "size", &TypeInfo::getSize )
        .def( "offset", &TypeInfo::getOffset )
        .def( "bitOffset", &TypeInfo::getBitOffset )
        .def( "bitWidth", &TypeInfo::getBitWidth )
        .def( "field", &TypeInfo::getField )
        .def( "asMap", &TypeInfo::asMap )
        .def( "deref", &TypeInfo::deref )
        .def( "__str__", &TypeInfo::print )
        .def( "__getattr__", &TypeInfo::getField );
        
    python::class_<TypedVar, TypedVarPtr, python::bases<intBase>, boost::noncopyable >("typedVar", 
        "Class of non-primitive type object, child class of typeClass. Data from target is copied into object instance", python::no_init  )
        .def("__init__", python::make_constructor(TypedVar::getTypedVarByName) )
        .def("__init__", python::make_constructor(TypedVar::getTypedVarByTypeName) )
        .def("__init__", python::make_constructor(TypedVar::getTypedVarByTypeInfo) )
        .def("getAddress", &TypedVar::getAddress, 
            "Return virtual address" )
        .def("sizeof", &TypedVar::getSize,
            "Return size of a variable in the target memory" )
        .def("offset", &TypedVar::getOffset,
            "Return offset to parent" )
        .def("field", &TypedVar::getField,
            "Return field of structure as an object attribute" )
        .def( "dataKind", &TypedVar::getDataKind,
            "Retrieves the variable classification of a data: DataIsXxx")
        .def("deref", &TypedVar::deref,
            "Return value by pointer" )
        .def("__getattr__", &TypedVar::getField,
            "Return field of structure as an object attribute" )
        .def( "__str__", &TypedVar::print )
        .def("__len__", &TypedVar::getElementCount )
        .def("__getitem__", &TypedVar::getElementByIndex )
        .def("__getitem__", &TypedVar::getElementByIndexPtr );

    python::class_<Module, python::bases<intBase> >("module", "Class representing executable module", python::no_init )
        .def("begin", &Module::getBase,
             "Return start address of the module" )
        .def("end", &Module::getEnd,
             "Return end address of the module" )
        .def("size", &Module::getSize,
              "Return size of the module" )
        .def("name", &Module::getName,
             "Return name of the module" )
        .def("image", &Module::getImageName,
             "Return name of the image of the module" )
        .def("pdb", &Module::getPdbName,
             "Return the full path to the module's pdb file ( symbol information )" )
        .def("reload", &Module::reloadSymbols,
            "(Re)load symbols for the module" )
        .def("offset", &Module::getSymbol,
            "Return offset of the symbol" )
        .def("rva", &Module::getSymbolRva,
            "Return rva of the symbol" )
        .def("type", &Module::getTypeByName,
            "Return typeInfo class by type name" )
        .def("typedVar", &Module::getTypedVarByAddr,
            "Return a typedVar class instance" )
        .def("typedVar",&Module::getTypedVarByName,
            "Return a typedVar class instance" )
        .def("typedVar",&Module::getTypedVarByType,
            "Return a typedVar class instance" )
        .def("typedVar",&Module::getTypedVarByTypeName,
            "Return a typedVar class instance" )
        .def("typedVarList", &Module::getTypedVarListByTypeName,
            "Return a list of the typedVar class instances. Each item represents an item of the linked list in the target memory" )
        .def("typedVarList", &Module::getTypedVarListByType,
            "Return a list of the typedVar class instances. Each item represents an item of the linked list in the target memory" )
        .def("typedVarArray", &Module::getTypedVarArrayByTypeName,
            "Return a list of the typedVar class instances. Each item represents an item of the counted array in the target memory" )
        .def("typedVarArray", &Module::getTypedVarArrayByType,
            "Return a list of the typedVar class instances. Each item represents an item of the counted array in the target memory" )
        .def("containingRecord", &Module::containingRecordByName,
            "Return instance of the typedVar class. It's value are loaded from the target memory."
            "The start address is calculated by the same method as the standard macro CONTAINING_RECORD does" )
        .def("containingRecord", &Module::containingRecordByType,
            "Return instance of the typedVar class. It's value are loaded from the target memory."
            "The start address is calculated by the same method as the standard macro CONTAINING_RECORD does" )
        .def("checksum",&Module::getCheckSumm,
            "Return a image file checksum: IMAGE_OPTIONAL_HEADER.CheckSum" )
        .def("timestamp",&Module::getTimeDataStamp,
            "Return a low 32 bits of the time stamp of the image: IMAGE_FILE_HEADER.TimeDateStamp" )
        .def("__getattr__", &Module::getSymbol,
            "Return address of the symbol" );

    python::class_<DbgOut>( "dout", "dout", python::no_init )
        .def( "write", &DbgOut::write );

    python::class_<DbgIn>( "din", "din", python::no_init )
        .def( "readline", &DbgIn::readline );

    python::class_<DbgExtension, DbgExtensionPtr>("ext", python::no_init )
        .def( "call", &DbgExtension::call,
            "Call debug extension command end return it's result as a string" );

    python::class_<EventHandlerWrap, boost::noncopyable>(
        "eventHandler", "Base class for overriding and handling debug notifications" )
        .def( python::init<>() )
        .def( python::init<DebugClientPtr&>() )
        .def( "onBreakpoint", &pykd::EventHandlerWrap::onBreakpoint,
            "Triggered breakpoint event. Parameter is int: ID of breakpoint\n"
            "For ignore event method must return DEBUG_STATUS_NO_CHANGE value" )
        .def( "onException", &pykd::EventHandlerWrap::onException,
            "Exception event. Parameter is dict:\n"
            "{\"Code\":int, \"Flags\":int, \"Record\":int, \"Address\":int,"
            " \"Parameters\":[int], \"FirstChance\":bool}\n"
            "Detailed information: http://msdn.microsoft.com/en-us/library/aa363082(VS.85).aspx \n"
            "For ignore event method must return DEBUG_STATUS_NO_CHANGE value" )
        .def( "onLoadModule", &pykd::EventHandlerWrap::onLoadModule,
            "Load module event. Parameter is instance of module.\n"
            "For ignore event method must return DEBUG_STATUS_NO_CHANGE value" )
        .def( "onUnloadModule", &pykd::EventHandlerWrap::onUnloadModule,
            "Unload module event. Parameter is base address of unloaded module.\n"
            "For ignore event method must return DEBUG_STATUS_NO_CHANGE value" );

    python::class_<Disasm>("disasm", "Class disassemble a processor instructions" )
        .def( python::init<>( "constructor" ) )
        .def( python::init<ULONG64>( boost::python::args("offset"), "constructor" ) )
        .def( "disasm", &Disasm::disassemble, "Disassemble next instruction" )
        .def( "asm", &Disasm::assembly, "Insert assemblied instuction to current offset" )
        .def( "begin", &Disasm::begin, "Return begin offset" )
        .def( "current", &Disasm::current, "Return current offset" )
        .def( "length", &Disasm::length, "Return current instruction length" )
        .def( "instruction", &Disasm::instruction, "Returm current disassembled instruction" )
        .def( "ea", &Disasm::ea, "Return effective address for last disassembled instruction or 0" )
        .def( "reset", &Disasm::reset, "Reset current offset to begin" );

    python::class_<StackFrame>( "stackFrame", 
         "Class representing a frame of the call stack", python::no_init )
        .def_readonly( "instructionOffset", &StackFrame::m_instructionOffset,
            "Return a frame's instruction offset" )
        .def_readonly( "returnOffset", &StackFrame::m_returnOffset,
            "Return a frame's return offset" )
        .def_readonly( "frameOffset", &StackFrame::m_frameOffset,
            "Return a frame's offset" )
        .def_readonly( "stackOffset", &StackFrame::m_stackOffset,
            "Return a frame's stack offset" )
        .def_readonly( "frameNumber", &StackFrame::m_frameNumber,
            "Return a frame's number" )
        .def( "getLocals", &StackFrame::getLocals, StackFrame_getLocals( python::args( "ctx" ),
            "Get list of local variables for this stack frame" ) )
        .def( "__str__", &StackFrame::print,
            "Return stacks frame as string");

    python::class_<ThreadContext, ContextPtr>(
        "Context", "Context of thread (register values)", python::no_init )
        .def( "ip", &ThreadContext::getIp, 
            "Get instruction pointer register" )
        .def( "retreg", &ThreadContext::getRetReg, 
            "Get primary return value register" )
        .def( "csp", &ThreadContext::getSp, 
            "Get current stack pointer" )
        .def( "get", &ThreadContext::getValue, 
            "Get register value by ID (CV_REG_XXX)" )
        .def( "processorType", &ThreadContext::getProcessorType,
            "Get processor ThreadContext as string")
        .def( "fork", &ThreadContext::forkByStackFrame,
            "Create new thread context by stackFrame")
        .def("__len__", &ThreadContext::getCount,
            "Return count of registers")
        .def("__getitem__", &ThreadContext::getByIndex,
            "Return tuple<ID, VALUE> by index");

    python::class_<CpuReg, python::bases<intBase> >( 
        "cpuReg", "CPU regsiter class", boost::python::no_init )
            .def( "name", &CpuReg::name, "The name of the regsiter" )
            .def( "index", &CpuReg::index, "The index of thr register" );

    python::def( "diaLoadPdb", &pyDia::GlobalScope::loadPdb, 
        "Open pdb file for querying debug symbols. Return DiaSymbol of global scope");
    python::def( "diaLoadExe", &pyDia::GlobalScope::loadExe, pyDia_GlobalScope_loadExe( python::args( "fileName", "searchPath" ) ,
        "Load the debug symbols associated with the executable file. Return DiaSymbol of global scope") );

    python::class_<pyDia::Symbol, pyDia::SymbolPtr>(
        "DiaSymbol", "class wrapper for MS DIA Symbol", python::no_init )
        .def( "findEx", &pyDia::Symbol::findChildrenEx, pyDia_Symbol_findChildrenEx( python::args( "symTag", "name", "cmpFlags" ) ,
            "Retrieves the children of the symbol" ) )
        .def( "find", &pyDia::Symbol::findChildren, 
            "Retrieves the children of the symbol" )
        .def( "size", &pyDia::Symbol::getSize, 
            "Retrieves the number of bits or bytes of memory used by the object represented by this symbol" )
        .def( "name", &pyDia::Symbol::getName, 
            "Retrieves the name of the symbol" )
        .def( "undecoratedName", &pyDia::Symbol::getUndecoratedName, 
            "Retrieves the undecorated name for a C++ decorated, or linkage, name" )
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
        .def( "value", (python::object(pyDia::Symbol::*)())&pyDia::Symbol::getValue,
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
        .def( "dataKind", &pyDia::Symbol::getDataKind,
            "Retrieves the variable classification of a data symbol")
        .def("registerId", &pyDia::Symbol::getRegisterId,
            "Retrieves the register designator of the location:\n"
            "CV_REG_XXX (for IMAGE_FILE_MACHINE_I386) or CV_AMD64_XXX (for IMAGE_FILE_MACHINE_AMD64)")
        .def("machineType", &pyDia::Symbol::getMachineType, 
            "Retrieves the type of the target CPU: IMAGE_FILE_MACHINE_XXX")
        .def( "__str__", &pyDia::Symbol::print)
        .def("__getitem__", &pyDia::Symbol::getChildByName)
        .def("__len__", &pyDia::Symbol::getChildCount )
        .def("__getitem__", &pyDia::Symbol::getChildByIndex )
        .def("__eq__", &pyDia::Symbol::eq)
        .def("__hash__", &pyDia::Symbol::getIndexId);
    
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

    DEF_PY_CONST_ULONG(DataIsUnknown);
    DEF_PY_CONST_ULONG(DataIsLocal);
    DEF_PY_CONST_ULONG(DataIsStaticLocal);
    DEF_PY_CONST_ULONG(DataIsParam);
    DEF_PY_CONST_ULONG(DataIsObjectPtr);
    DEF_PY_CONST_ULONG(DataIsFileStatic);
    DEF_PY_CONST_ULONG(DataIsGlobal);
    DEF_PY_CONST_ULONG(DataIsMember);
    DEF_PY_CONST_ULONG(DataIsStaticMember);
    DEF_PY_CONST_ULONG(DataIsConstant);
    python::scope().attr("diaDataKind") = 
        genDict(pyDia::Symbol::dataKindName, _countof(pyDia::Symbol::dataKindName));

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

    // wrapper for standart python exceptions
    python::register_exception_translator<PyException>( &PyException::exceptionTranslate );


    pykd::exception<DbgException>( "BaseException", "Pykd base exception class" );
    pykd::exception<MemoryException,DbgException>( "MemoryException", "Target memory access exception class" );
    pykd::exception<WaitEventException,DbgException>( "WaitEventException", "Debug interface access exception" );
    pykd::exception<SymbolException,DbgException>( "SymbolException", "Symbol exception" );
    pykd::exception<pyDia::Exception,SymbolException>( "DiaException", "Debug interface access exception" );
    pykd::exception<TypeException,SymbolException>( "TypeException", "type exception" );
    pykd::exception<AddSyntheticSymbolException,DbgException>( "AddSynSymbolException", "synthetic symbol exception" );

    DEF_PY_CONST_ULONG( DEBUG_CLASS_UNINITIALIZED );
    DEF_PY_CONST_ULONG( DEBUG_CLASS_KERNEL );
    DEF_PY_CONST_ULONG( DEBUG_CLASS_USER_WINDOWS );

    DEF_PY_CONST_ULONG( DEBUG_KERNEL_CONNECTION );
    DEF_PY_CONST_ULONG( DEBUG_KERNEL_LOCAL );
    DEF_PY_CONST_ULONG( DEBUG_KERNEL_EXDI_DRIVER );
    DEF_PY_CONST_ULONG( DEBUG_KERNEL_SMALL_DUMP );
    DEF_PY_CONST_ULONG( DEBUG_KERNEL_DUMP );
    DEF_PY_CONST_ULONG( DEBUG_KERNEL_FULL_DUMP );

    DEF_PY_CONST_ULONG( DEBUG_USER_WINDOWS_PROCESS );
    DEF_PY_CONST_ULONG( DEBUG_USER_WINDOWS_PROCESS_SERVER );
    DEF_PY_CONST_ULONG( DEBUG_USER_WINDOWS_SMALL_DUMP );
    DEF_PY_CONST_ULONG( DEBUG_USER_WINDOWS_DUMP );

    // exception codes
    DEF_PY_CONST_ULONG(EXCEPTION_ACCESS_VIOLATION);
    DEF_PY_CONST_ULONG(EXCEPTION_DATATYPE_MISALIGNMENT);
    DEF_PY_CONST_ULONG(EXCEPTION_BREAKPOINT);
    DEF_PY_CONST_ULONG(EXCEPTION_SINGLE_STEP);
    DEF_PY_CONST_ULONG(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
    DEF_PY_CONST_ULONG(EXCEPTION_FLT_DENORMAL_OPERAND);
    DEF_PY_CONST_ULONG(EXCEPTION_FLT_DIVIDE_BY_ZERO);
    DEF_PY_CONST_ULONG(EXCEPTION_FLT_INEXACT_RESULT);
    DEF_PY_CONST_ULONG(EXCEPTION_FLT_INVALID_OPERATION);
    DEF_PY_CONST_ULONG(EXCEPTION_FLT_OVERFLOW);
    DEF_PY_CONST_ULONG(EXCEPTION_FLT_STACK_CHECK);
    DEF_PY_CONST_ULONG(EXCEPTION_FLT_UNDERFLOW);
    DEF_PY_CONST_ULONG(EXCEPTION_INT_DIVIDE_BY_ZERO);
    DEF_PY_CONST_ULONG(EXCEPTION_INT_OVERFLOW);
    DEF_PY_CONST_ULONG(EXCEPTION_PRIV_INSTRUCTION);
    DEF_PY_CONST_ULONG(EXCEPTION_IN_PAGE_ERROR);
    DEF_PY_CONST_ULONG(EXCEPTION_ILLEGAL_INSTRUCTION);
    DEF_PY_CONST_ULONG(EXCEPTION_NONCONTINUABLE_EXCEPTION);
    DEF_PY_CONST_ULONG(EXCEPTION_STACK_OVERFLOW);
    DEF_PY_CONST_ULONG(EXCEPTION_INVALID_DISPOSITION);
    DEF_PY_CONST_ULONG(EXCEPTION_GUARD_PAGE);
    DEF_PY_CONST_ULONG(EXCEPTION_INVALID_HANDLE);

    // debug status
    DEF_PY_CONST_ULONG(DEBUG_STATUS_NO_CHANGE);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_GO);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_GO_HANDLED);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_GO_NOT_HANDLED);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_STEP_OVER);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_STEP_INTO);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_BREAK);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_NO_DEBUGGEE);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_STEP_BRANCH);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_RESTART_REQUESTED);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_REVERSE_GO);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_REVERSE_STEP_BRANCH);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_REVERSE_STEP_OVER);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_REVERSE_STEP_INTO);

    // breakpoints constatns
    DEF_PY_CONST_ULONG(DEBUG_BREAKPOINT_CODE);
    DEF_PY_CONST_ULONG(DEBUG_BREAKPOINT_DATA);

    DEF_PY_CONST_ULONG(DEBUG_BREAK_READ);
    DEF_PY_CONST_ULONG(DEBUG_BREAK_WRITE);
    DEF_PY_CONST_ULONG(DEBUG_BREAK_EXECUTE);
    DEF_PY_CONST_ULONG(DEBUG_BREAK_IO);
}

#undef DEF_PY_CONST_ULONG

////////////////////////////////////////////////////////////////////////////////