#include "stdafx.h"

#include <boost\algorithm\string\case_conv.hpp>

#include "win/dbgeng.h"
#include "win/dbgio.h"
#include "dbgexcept.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

DebugEngine     g_dbgEng;

///////////////////////////////////////////////////////////////////////////////////

ULONG startProcess( const std::wstring  &processName )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    ULONG       opt;
    hres = g_dbgEng->control->GetEngineOptions( &opt );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetEngineOptions failed" );

    opt |= DEBUG_ENGOPT_INITIAL_BREAK;
    hres = g_dbgEng->control->SetEngineOptions( opt );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::SetEngineOptions failed" );

    std::vector< std::wstring::value_type >      cmdLine( processName.size() + 1 );
    wcscpy_s( &cmdLine[0], cmdLine.size(), processName.c_str() );

    hres = g_dbgEng->client->CreateProcessWide( 0, &cmdLine[0], DEBUG_PROCESS | DETACHED_PROCESS );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient4::CreateProcessWide failed" );

    hres = g_dbgEng->control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::WaitForEvent failed" );

    ULONG processId = -1;
    hres = g_dbgEng->system->GetCurrentProcessId( &processId );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects::GetCurrentProcessId failed" );

    return processId;
}

///////////////////////////////////////////////////////////////////////////////////

ULONG attachProcess( ULONG pid )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    ULONG       opt;
    hres = g_dbgEng->control->GetEngineOptions( &opt );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetEngineOptions failed" );

    opt |= DEBUG_ENGOPT_INITIAL_BREAK;
    hres = g_dbgEng->control->SetEngineOptions( opt );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::SetEngineOptions failed" );
    
    hres = g_dbgEng->client->AttachProcess( 0, pid, 0 );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient::AttachProcess failed" );

    hres = g_dbgEng->control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::WaitForEvent failed" );

    ULONG processId = -1;
    hres = g_dbgEng->system->GetCurrentProcessId( &processId );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects::GetCurrentProcessId failed" );

    return processId;
}

///////////////////////////////////////////////////////////////////////////////////

void detachProcess( ULONG processId )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;

    if ( processId != -1 )
    {
        hres = g_dbgEng->system->SetCurrentProcessId(processId);
        if ( FAILED(hres) )
            throw DbgException( "IDebugSystemObjects::SetCurrentProcessId failed" );
    }

    hres = g_dbgEng->client->DetachCurrentProcess();
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient::DetachCurrentProcess failed" );
}

///////////////////////////////////////////////////////////////////////////////////

void terminateProcess( ULONG processId )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    if ( processId != -1 )
    {
        hres = g_dbgEng->system->SetCurrentProcessId(processId);
        if ( FAILED(hres) )
            throw DbgException( "IDebugSystemObjects::SetCurrentProcessId failed" );
    }

    hres = g_dbgEng->client->TerminateCurrentProcess();
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient::TerminateCurrentProcess", hres );

}

///////////////////////////////////////////////////////////////////////////////////

void loadDump( const std::wstring &fileName )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
     
    hres = g_dbgEng->client->OpenDumpFileWide( fileName.c_str(), NULL );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient4::OpenDumpFileWide failed" );

    hres = g_dbgEng->control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::WaitForEvent failed" );
}

///////////////////////////////////////////////////////////////////////////////////

bool isDumpAnalyzing()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT         hres;
    ULONG           debugClass, debugQualifier;
    
    hres = g_dbgEng->control->GetDebuggeeType( &debugClass, &debugQualifier );
    
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetDebuggeeType  failed" );   
         
    return debugQualifier >= DEBUG_DUMP_SMALL;
}

///////////////////////////////////////////////////////////////////////////////////

bool isKernelDebugging()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG       debugClass, debugQualifier;
    
    hres = g_dbgEng->control->GetDebuggeeType( &debugClass, &debugQualifier );
    
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetDebuggeeType  failed" );   
         
    return debugClass == DEBUG_CLASS_KERNEL;
}

///////////////////////////////////////////////////////////////////////////////////

void debugGo()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    hres = g_dbgEng->control->SetExecutionStatus( DEBUG_STATUS_GO );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::SetExecutionStatus failed" );

    ULONG    currentStatus;

    do {
        hres = g_dbgEng->control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
        if ( FAILED( hres ) )
            throw DbgException( "IDebugControl::WaitForEvent failed" );

        hres = g_dbgEng->control->GetExecutionStatus( &currentStatus );

        if ( FAILED( hres ) )
            throw  DbgException( "IDebugControl::GetExecutionStatus  failed" ); 

    } while( currentStatus != DEBUG_STATUS_BREAK && currentStatus != DEBUG_STATUS_NO_DEBUGGEE );
}

///////////////////////////////////////////////////////////////////////////////

void debugStep()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    hres = g_dbgEng->control->SetExecutionStatus( DEBUG_STATUS_STEP_OVER );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::SetExecutionStatus failed" );

    ULONG    currentStatus;

    do {
        hres = g_dbgEng->control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
        if ( FAILED( hres ) )
            throw DbgException( "IDebugControl::WaitForEvent failed" );

        hres = g_dbgEng->control->GetExecutionStatus( &currentStatus );

        if ( FAILED( hres ) )
            throw  DbgException( "IDebugControl::GetExecutionStatus  failed" ); 

    } while( currentStatus != DEBUG_STATUS_BREAK && currentStatus != DEBUG_STATUS_NO_DEBUGGEE );
}

///////////////////////////////////////////////////////////////////////////////

void debugStepIn()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    hres = g_dbgEng->control->SetExecutionStatus( DEBUG_STATUS_STEP_INTO );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::SetExecutionStatus failed" );

    ULONG    currentStatus;

    do {
        hres = g_dbgEng->control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
        if ( FAILED( hres ) )
            throw DbgException( "IDebugControl::WaitForEvent failed" );

        hres = g_dbgEng->control->GetExecutionStatus( &currentStatus );

        if ( FAILED( hres ) )
            throw  DbgException( "IDebugControl::GetExecutionStatus  failed" ); 

    } while( currentStatus != DEBUG_STATUS_BREAK && currentStatus != DEBUG_STATUS_NO_DEBUGGEE );
}

///////////////////////////////////////////////////////////////////////////////

void debugBreak()
{
    PyThreadState   *pystate = PyEval_SaveThread();

    HRESULT     hres;

    hres = g_dbgEng->control->SetInterrupt( DEBUG_INTERRUPT_ACTIVE );

    PyEval_RestoreThread( pystate );

    if ( FAILED( hres ) )
        throw  DbgException( "IDebugControl::SetInterrupt" ); 
}

///////////////////////////////////////////////////////////////////////////////

ULONG64 evaluate( const std::wstring  &expression )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT             hres;
    ULONG64             value = 0;

    DEBUG_VALUE  debugValue = {};
    ULONG        remainderIndex = 0;

    hres = g_dbgEng->control->IsPointer64Bit();
    if ( FAILED( hres ) )
        throw  DbgException( "IDebugControl::IsPointer64Bit  failed" );
    
    if ( hres == S_OK )
    {
        hres = g_dbgEng->control->EvaluateWide( 
            expression.c_str(), 
            DEBUG_VALUE_INT64,
            &debugValue,
            &remainderIndex );
            
        if ( FAILED( hres ) )
            throw  DbgException( "IDebugControl::Evaluate  failed" );
            
        if ( remainderIndex == expression.length() )
            value = debugValue.I64;
    }
    else
    {
        hres = g_dbgEng->control->EvaluateWide( 
            expression.c_str(), 
            DEBUG_VALUE_INT32,
            &debugValue,
            &remainderIndex );
            
        if (  FAILED( hres ) )
            throw  DbgException( "IDebugControl::Evaluate  failed" );
            
        if ( remainderIndex == expression.length() )
            value = debugValue.I32;
    }      

    return value;
}

///////////////////////////////////////////////////////////////////////////////

std::string debugCommand( const std::wstring &command )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT         hres;
    OutputReader    outReader( g_dbgEng->client );

    hres = g_dbgEng->control->ExecuteWide( DEBUG_OUTCTL_THIS_CLIENT, command.c_str(), 0 );

    if ( FAILED( hres ) )
        throw  DbgException( "IDebugControl::Execute  failed" ); 

    return std::string( outReader.Line() ); 
}

///////////////////////////////////////////////////////////////////////////////

ULONG64 findModuleBase( const std::string &moduleName )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG64     base;

    hres = g_dbgEng->symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &base );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleByModuleName  failed" ); 

    return base;
}

///////////////////////////////////////////////////////////////////////////////////

ULONG64 findModuleBase( ULONG64 offset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG64     base;
    ULONG       moduleIndex;

    hres = g_dbgEng->symbols->GetModuleByOffset( offset, 0, &moduleIndex, &base );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleByOffset failed" );

    return base;
}

///////////////////////////////////////////////////////////////////////////////////

ULONG64 findModuleBySymbol( const std::string &symbolName )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG64     base;

    hres = g_dbgEng->symbols->GetSymbolModule( ( std::string("!") + symbolName ).c_str(), &base );
    if ( FAILED( hres ) )
    {
        std::stringstream   sstr;
        sstr << "failed to find module for symbol: " << symbolName;
        throw SymbolException( sstr.str() );
    }

    return base;
}

///////////////////////////////////////////////////////////////////////////////////

std::string getModuleName( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;

    char  moduleName[0x100];

    hres = g_dbgEng->symbols->GetModuleNameString( 
        DEBUG_MODNAME_MODULE,
        DEBUG_ANY_ID,
        baseOffset,
        moduleName,
        sizeof( moduleName ),
        NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleNameString failed" );

    return std::string( moduleName );
}

///////////////////////////////////////////////////////////////////////////////////

std::string getModuleImageName( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;
    char  imageName[0x100];

    hres = g_dbgEng->symbols->GetModuleNameString( 
        DEBUG_MODNAME_IMAGE,
        DEBUG_ANY_ID,
        baseOffset,
        imageName,
        sizeof( imageName ),
        NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleNameString failed" );

    return std::string( imageName );
}

///////////////////////////////////////////////////////////////////////////////////

ULONG getModuleSize( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;
    DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };

    hres = g_dbgEng->symbols->GetModuleParameters( 1, &baseOffset, 0, &moduleParam );
    if ( FAILED( hres ) )
         throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );    

    return moduleParam.Size;
}

///////////////////////////////////////////////////////////////////////////////////

std::string getModuleSymbolFileName( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;
    IMAGEHLP_MODULEW64   moduleInfo = {};

    hres = g_dbgEng->advanced->GetSymbolInformation(
        DEBUG_SYMINFO_IMAGEHLP_MODULEW64,
        baseOffset,
        0,
        &moduleInfo,
        sizeof(moduleInfo),
        NULL,
        NULL,
        0,
        NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugAdvanced2::GetSymbolInformation failed" );

    if (!*moduleInfo.LoadedPdbName)
    {
        std::wstring  param = L"/f ";
        param += moduleInfo.ImageName;

        hres = g_dbgEng->symbols->ReloadWide( param.c_str() );
        if ( FAILED( hres ) )
            throw DbgException("IDebugSymbols::Reload failed" );

        hres = g_dbgEng->advanced->GetSymbolInformation(
            DEBUG_SYMINFO_IMAGEHLP_MODULEW64,
            baseOffset,
            0,
            &moduleInfo,
            sizeof(moduleInfo),
            NULL,
            NULL,
            0,
            NULL );

        if ( FAILED( hres ) )
            throw DbgException( "IDebugAdvanced2::GetSymbolInformation failed" );
    }

    char  pdbName[ 256 ];
    WideCharToMultiByte( CP_ACP, 0, moduleInfo.LoadedPdbName, 256, pdbName, 256, NULL, NULL );

    return std::string( pdbName );
}

///////////////////////////////////////////////////////////////////////////////////

ULONG getModuleTimeStamp( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;
    DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };

    hres = g_dbgEng->symbols->GetModuleParameters( 1, &baseOffset, 0, &moduleParam );
    if ( FAILED( hres ) )
         throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );    

    return moduleParam.TimeDateStamp;
}

///////////////////////////////////////////////////////////////////////////////////

ULONG getModuleCheckSum( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;
    DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };

    hres = g_dbgEng->symbols->GetModuleParameters( 1, &baseOffset, 0, &moduleParam );
    if ( FAILED( hres ) )
         throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );    

    return moduleParam.Checksum;
}

///////////////////////////////////////////////////////////////////////////////////

ULONG ptrSize()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    hres = g_dbgEng->control->IsPointer64Bit();

    if ( FAILED( hres ) )
        throw  DbgException( "IDebugControl::IsPointer64Bit  failed" );
    
    return S_OK == hres ? 8 : 4;
}

///////////////////////////////////////////////////////////////////////////////////

bool is64bitSystem()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG  procType;

    hres = g_dbgEng->control->GetActualProcessorType( &procType );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetActualProcessorType failed" );
        
    return procType == IMAGE_FILE_MACHINE_AMD64;
}

///////////////////////////////////////////////////////////////////////////////

ULONG getPageSize()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;
    ULONG  pageSize;

    hres = g_dbgEng->control->GetPageSize( &pageSize );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetPageSize failed" );

    return pageSize;
}

///////////////////////////////////////////////////////////////////////////////

ULONG64 loadMSR( ULONG  msr )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG64     value;

    hres = g_dbgEng->dataspace->ReadMsr( msr, &value );
    if ( FAILED( hres ) )
         throw DbgException( "IDebugDataSpaces::ReadMsr", hres );

    return value;
}

///////////////////////////////////////////////////////////////////////////////

void setMSR( ULONG msr, ULONG64 value)
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    hres = g_dbgEng->dataspace->WriteMsr(msr, value);
    if ( FAILED( hres ) )
         throw DbgException( "IDebugDataSpaces::WriteMsr", hres );
}

///////////////////////////////////////////////////////////////////////////////

ULONG getRegIndexByName( const std::string &regName )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;
    ULONG  index;

    hres = g_dbgEng->registers->GetIndexByName( boost::to_lower_copy(regName).c_str(), &index );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegister::GetIndexByName", hres );

    return index;
}

///////////////////////////////////////////////////////////////////////////////

std::string getRegNameByIndex( ULONG index )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    ULONG       nameSize = 0;

    hres = 
       g_dbgEng->registers->GetDescription( 
            index,
            NULL,
            0,
            &nameSize,
            NULL );

    if ( nameSize == 0 )
    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegister::GetDescription", hres );

    std::vector<char>   nameBuffer(nameSize);
    DEBUG_REGISTER_DESCRIPTION    desc = {};

    hres = 
        g_dbgEng->registers->GetDescription( 
            index,
            &nameBuffer[0],
            nameSize,
            NULL,
            &desc );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegister::GetDescription", hres );

    return std::string( &nameBuffer[0] );
}

///////////////////////////////////////////////////////////////////////////////

BaseTypeVariant getRegVariantValue( ULONG index )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT         hres;
        
    DEBUG_VALUE    debugValue;
    hres = g_dbgEng->registers->GetValue( index, &debugValue );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegister::GetValue", hres );
        
    switch( debugValue.Type )
    {
    case DEBUG_VALUE_INT8:
        return BaseTypeVariant( (LONG)debugValue.I8 );
        break;
        
    case DEBUG_VALUE_INT16:
        return BaseTypeVariant( (LONG)debugValue.I16 );
        break;
        
    case DEBUG_VALUE_INT32:
        return BaseTypeVariant( debugValue.I32 );
        break;
        
    case DEBUG_VALUE_INT64:
        return BaseTypeVariant( debugValue.I64 );
        break;
    } 

    throw DbgException( "Failed to convert register value" );
}

///////////////////////////////////////////////////////////////////////////////

ULONG64 getRegInstructionPointer()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;
    ULONG64  ip = 0;

    hres = g_dbgEng->registers->GetInstructionOffset( &ip );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugRegisters::GetInstructionOffset failed" );

    return ip;
}

///////////////////////////////////////////////////////////////////////////////

static void buildStacksFrames(
    std::vector<STACK_FRAME_DESC> &frames,
    ULONG64 frameOffset = 0,
    ULONG64 stackOffset = 0,
    ULONG64 instructionOffset = 0)
{
    ULONG   filledFrames = 1024;
    std::vector<DEBUG_STACK_FRAME> dbgFrames(filledFrames);

    HRESULT hres = 
        g_dbgEng->control->GetStackTrace(
            frameOffset,
            stackOffset,
            instructionOffset,
            &dbgFrames[0],
            filledFrames,
            &filledFrames);
    if (S_OK != hres)
        throw DbgException( "IDebugControl::GetStackTrace", hres );

    frames.resize(filledFrames);
    for ( ULONG i = 0; i < filledFrames; ++i )
    {
        frames[i].number = dbgFrames[i].FrameNumber;
        frames[i].instructionOffset = dbgFrames[i].InstructionOffset;
        frames[i].returnOffset = dbgFrames[i].ReturnOffset;
        frames[i].frameOffset = dbgFrames[i].FrameOffset;
        frames[i].stackOffset = dbgFrames[i].StackOffset;
    }
}

///////////////////////////////////////////////////////////////////////////////

void getStackTrace(std::vector<STACK_FRAME_DESC> &frames)
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    frames.resize(0);

    buildStacksFrames(frames);
}

///////////////////////////////////////////////////////////////////////////////

static void ReadWow64Context(WOW64_CONTEXT &Context)
{
    // 
    //  *** undoc ***
    // !wow64exts.r
    // http://www.woodmann.com/forum/archive/index.php/t-11162.html
    // http://www.nynaeve.net/Code/GetThreadWow64Context.cpp
    // 

    HRESULT     hres;
    ULONG       debugClass, debugQualifier;
    
    hres = g_dbgEng->control->GetDebuggeeType( &debugClass, &debugQualifier );
    
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetDebuggeeType  failed" );   
         
    ULONG64 teb64Address;

    if ( debugClass == DEBUG_CLASS_KERNEL )
    {
        DEBUG_VALUE  debugValue = {};
        ULONG        remainderIndex = 0;

        hres = g_dbgEng->control->EvaluateWide( 
            L"@@C++(#FIELD_OFFSET(nt!_KTHREAD, Teb))",
            DEBUG_VALUE_INT64,
            &debugValue,
            &remainderIndex );
            
        if ( FAILED( hres ) )
            throw  DbgException( "IDebugControl::Evaluate  failed" );
            
        ULONG64 tebOffset = debugValue.I64;

        hres = g_dbgEng->system->GetImplicitThreadDataOffset(&teb64Address);
        if (S_OK != hres)
            throw DbgException( "IDebugSystemObjects::GetImplicitThreadDataOffset", hres);

        ULONG readedBytes;

        readMemoryUnsafeNoSafe(
            teb64Address + tebOffset,
            &teb64Address,
            sizeof(teb64Address),
            false,
            &readedBytes);
    }
    else
    {
        hres = g_dbgEng->system->GetImplicitThreadDataOffset(&teb64Address);
        if (S_OK != hres)
            throw DbgException( "IDebugSystemObjects::GetImplicitThreadDataOffset", hres);
    }

    // ? @@C++(#FIELD_OFFSET(nt!_TEB64, TlsSlots))
    // hardcoded in !wow64exts.r (6.2.8250.0)
    static const ULONG teb64ToTlsOffset = 0x01480;
    static const ULONG WOW64_TLS_CPURESERVED = 1;
    ULONG64 cpuAreaAddress;
    ULONG readedBytes;

    bool readRes;
    readRes = 
        readMemoryUnsafeNoSafe(
            teb64Address + teb64ToTlsOffset + (sizeof(ULONG64) * WOW64_TLS_CPURESERVED),
            &cpuAreaAddress,
            sizeof(cpuAreaAddress),
            false,
            &readedBytes);
    if (!readRes || readedBytes != sizeof(cpuAreaAddress))
        throw DbgException( "IDebugDataSpaces::ReadVirtual", hres);

    // CPU Area is:
    // +00 unknown ULONG
    // +04 WOW64_CONTEXT struct
    static const ULONG cpuAreaToWow64ContextOffset = sizeof(ULONG);
    readRes =
        readMemoryUnsafeNoSafe(
            cpuAreaAddress + cpuAreaToWow64ContextOffset,
            &Context,
            sizeof(Context),
            false,
            &readedBytes);
    if (!readRes || readedBytes != sizeof(Context))
        throw DbgException( "IDebugDataSpaces::ReadVirtual", hres);
}

///////////////////////////////////////////////////////////////////////////////

class AutoRevertEffectiveProcessorType
{
public:
    AutoRevertEffectiveProcessorType() 
        : m_processType(IMAGE_FILE_MACHINE_UNKNOWN)
    {
    }

    ~AutoRevertEffectiveProcessorType()
    {
        if (IMAGE_FILE_MACHINE_UNKNOWN != m_processType)
            BOOST_VERIFY(S_OK == g_dbgEng->control->SetEffectiveProcessorType(m_processType));
    }

    void to(ULONG processType)  { m_processType = processType; }
private:
    ULONG m_processType;
};

///////////////////////////////////////////////////////////////////////////////

void getStackTraceWow64(std::vector<STACK_FRAME_DESC> &frames)
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    frames.resize(0);

    ULONG processorType;
    HRESULT hres = g_dbgEng->control->GetActualProcessorType(&processorType);
    if (S_OK != hres)
        throw DbgException( "IDebugControl::GetActualProcessorType", hres );
    if (IMAGE_FILE_MACHINE_AMD64 != processorType)
        throw DbgException( "Only for WOW64 processor mode" );

    hres = g_dbgEng->control->GetEffectiveProcessorType(&processorType);
    if (S_OK != hres)
        throw DbgException( "IDebugControl::GetEffectiveProcessorType", hres );

    AutoRevertEffectiveProcessorType autoRevertEffectiveProcessorType;
    if (IMAGE_FILE_MACHINE_I386 != processorType)
    {
        if (IMAGE_FILE_MACHINE_AMD64 != processorType)
            throw DbgException( "Only for WOW64 processor mode" );

        hres = g_dbgEng->control->SetEffectiveProcessorType(IMAGE_FILE_MACHINE_I386);
        if (S_OK != hres)
            throw DbgException( "IDebugControl::SetEffectiveProcessorType", hres );

        autoRevertEffectiveProcessorType.to(IMAGE_FILE_MACHINE_AMD64);
    }

    WOW64_CONTEXT Context;
    ReadWow64Context(Context);

    buildStacksFrames(frames, Context.Ebp, Context.Esp, Context.Eip);
}

///////////////////////////////////////////////////////////////////////////////

std::string processorToStr(ULONG processorMode)
{
    switch( processorMode )
    {
    case IMAGE_FILE_MACHINE_I386:
        return "X86";
        
    case IMAGE_FILE_MACHINE_ARM:
        return "ARM";
        
    case IMAGE_FILE_MACHINE_IA64:
        return "IA64";
        
    case IMAGE_FILE_MACHINE_AMD64:
        return "X64";
    }

    throw DbgException( "Unknown CPU type" );
}

///////////////////////////////////////////////////////////////////////////////

std::string getProcessorMode()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT         hres;
    ULONG           processorMode;

    hres = g_dbgEng->control->GetEffectiveProcessorType( &processorMode );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetEffectiveProcessorType  failed" );

    return processorToStr(processorMode);
}

///////////////////////////////////////////////////////////////////////////////

void setProcessorMode( const std::string &mode )
{
    HRESULT         hres;  
    ULONG           processorMode;

    if ( mode == "X86" )
        processorMode = IMAGE_FILE_MACHINE_I386;
    else if ( mode == "ARM" )
        processorMode = IMAGE_FILE_MACHINE_ARM;
    else if ( mode == "IA64" )
        processorMode = IMAGE_FILE_MACHINE_IA64;
    else if ( mode == "X64" )
        processorMode = IMAGE_FILE_MACHINE_AMD64;
    else
        throw DbgException( "Unknown processor type" );

    hres =  g_dbgEng->control->SetEffectiveProcessorType( processorMode );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::SetEffectiveProcessorType  failed" );
}

///////////////////////////////////////////////////////////////////////////////

std::string getProcessorType()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG       processorMode;

    hres = g_dbgEng->control->GetActualProcessorType( &processorMode );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetActualProcessorType  failed" );

    return processorToStr(processorMode);
}

///////////////////////////////////////////////////////////////////////////////

ULONG breakPointSet( ULONG64 offset, bool hardware, ULONG size, ULONG accessType )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;

    IDebugBreakpoint  *bp;
    hres = g_dbgEng->control->AddBreakpoint(
        hardware ? DEBUG_BREAKPOINT_DATA : DEBUG_BREAKPOINT_CODE,
        DEBUG_ANY_ID,
        &bp);
    if (S_OK != hres)
        throw DbgException("IDebugControl::AddBreakpoint", hres);

    hres = bp->SetOffset(offset);
    if (S_OK != hres)
    {
        g_dbgEng->control->RemoveBreakpoint(bp);
        throw DbgException("IDebugBreakpoint::SetOffset", hres);
    }

    ULONG bpFlags;
    hres = bp->GetFlags(&bpFlags);
    if (S_OK != hres)
    {
        g_dbgEng->control->RemoveBreakpoint(bp);
        throw DbgException("IDebugBreakpoint::GetFlags", hres);
    }

    bpFlags |= DEBUG_BREAKPOINT_ENABLED;
    hres = bp->SetFlags(bpFlags);
    if (S_OK != hres)
    {
        g_dbgEng->control->RemoveBreakpoint(bp);
        throw DbgException("IDebugBreakpoint::SetFlags", hres);
    }

    if ( hardware )
    {
        HRESULT hres = bp->SetDataParameters(size, accessType);
        if (S_OK != hres)
        {
            g_dbgEng->control->RemoveBreakpoint(bp);
            throw DbgException("IDebugBreakpoint::SetDataParameters", hres);
        }
    }

    ULONG  breakId;
    hres = bp->GetId(&breakId);
    if (S_OK != hres)
    {
        g_dbgEng->control->RemoveBreakpoint(bp);
        throw DbgException("IDebugBreakpoint::GetId", hres);
    }

    return breakId;
}

///////////////////////////////////////////////////////////////////////////////

void breakPointRemove( ULONG id )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    IDebugBreakpoint *bp;
    HRESULT hres = g_dbgEng->control->GetBreakpointById(id, &bp);
    if (S_OK != hres)
        throw DbgException("IDebugControl::GetBreakpointById", hres);

    hres = g_dbgEng->control->RemoveBreakpoint(bp);
    if (S_OK != hres)
        throw DbgException("IDebugControl::RemoveBreakpoint", hres);
}

///////////////////////////////////////////////////////////////////////////////

void breakPointRemoveAll()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    ULONG numberOfBps;
    do {
        HRESULT hres = g_dbgEng->control->GetNumberBreakpoints(&numberOfBps);
        if (S_OK != hres)
            throw DbgException("IDebugControl::GetNumberBreakpoints", hres);

        if (!numberOfBps)
            break;

        IDebugBreakpoint *bp;
        hres = g_dbgEng->control->GetBreakpointByIndex(0, &bp);
        if (S_OK != hres)
            throw DbgException("IDebugControl::GetBreakpointByIndex", hres);

        hres = g_dbgEng->control->RemoveBreakpoint(bp);
        if (S_OK != hres)
            throw DbgException("IDebugControl::RemoveBreakpoint", hres);

    } while (numberOfBps);
}

///////////////////////////////////////////////////////////////////////////////

void eventRegisterCallbacks( const DEBUG_EVENT_CALLBACK *callbacks )
{
    g_dbgEng.registerCallbacks( callbacks );
}


void DebugEngine::registerCallbacks( const DEBUG_EVENT_CALLBACK *callbacks )
{
    boost::recursive_mutex::scoped_lock l(m_handlerLock);

    m_handlers.push_back( DebugEventContext( callbacks, PyThreadStateSaver( PyThreadState_Get() ) ) );
}

///////////////////////////////////////////////////////////////////////////////

void eventRemoveCallbacks( const DEBUG_EVENT_CALLBACK *callback )
{
    g_dbgEng.removeCallbacks( callback );
}

void DebugEngine::removeCallbacks( const DEBUG_EVENT_CALLBACK *callback )
{
    boost::recursive_mutex::scoped_lock l(m_handlerLock);

    HandlerList::iterator  it = m_handlers.begin();

    for ( ; it != m_handlers.end(); ++it )
    {
        if ( it->callback == callback )
        {
            m_handlers.erase( it );
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

inline
ULONG ConvertCallbackResult( DEBUG_CALLBACK_RESULT result )
{
    switch( result )
    {
    case DebugCallbackBreak:
        return DEBUG_STATUS_BREAK;

    case DebugCallbackProceed:
        return DEBUG_STATUS_GO_HANDLED;

    default:
        assert( 0 );

    case DebugCallbackNoChange:
        return DEBUG_STATUS_NO_CHANGE;
    }
}

///////////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugEngine::Breakpoint(
            __in IDebugBreakpoint *bp
        ) 
{
    HRESULT hres;
    ULONG id;
    DEBUG_CALLBACK_RESULT result = DebugCallbackNoChange;

    hres = bp->GetId( &id );
    if ( FAILED( hres ) )
        return DEBUG_STATUS_NO_CHANGE;

    boost::recursive_mutex::scoped_lock l(m_handlerLock);

    HandlerList::iterator  it = m_handlers.begin();

    for ( ; it != m_handlers.end(); ++it )
    {
        PyThread_StateSave pyThreadSave( it->pystate );

        DEBUG_CALLBACK_RESULT ret = it->callback->OnBreakpoint( id );

        result = ret != DebugCallbackNoChange ? ret : result;
    }

    return ConvertCallbackResult( result );
}

///////////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugEngine::LoadModule(
    __in ULONG64 ImageFileHandle,
    __in ULONG64 BaseOffset,
    __in ULONG ModuleSize,
    __in_opt PCSTR ModuleName,
    __in_opt PCSTR ImageName,
    __in ULONG CheckSum,
    __in ULONG TimeDateStamp
    )
{
    DEBUG_CALLBACK_RESULT result = DebugCallbackNoChange;

    boost::recursive_mutex::scoped_lock l(m_handlerLock);

    HandlerList::iterator  it = m_handlers.begin();

    for ( ; it != m_handlers.end(); ++it )
    {
        PyThread_StateSave pyThreadSave( it->pystate );

        std::string   modName;

        if ( ModuleName )
        {
            modName = ModuleName;
        }
        else if ( ImageName )
        {
            // при работоте kernel отладчика ModuleName может быть равен NULL;
            modName = ImageName;
            modName.erase( modName.rfind('.') );
        }
        else
            modName = "";

        DEBUG_CALLBACK_RESULT  ret = it->callback->OnModuleLoad( BaseOffset, modName );

        result = ret != DebugCallbackNoChange ? ret : result;
    }

    return ConvertCallbackResult( result );
}


///////////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugEngine::UnloadModule(
     __in_opt PCSTR ImageBaseName,
     __in ULONG64 BaseOffset )
{
    DEBUG_CALLBACK_RESULT result = DebugCallbackNoChange;

    HRESULT  hres;

    char  moduleNameBuf[0x100];

    // были случаи, когда по BaseOffset не получить было имя модуля
    // поэтому не используется ф. getModuleName
    hres = g_dbgEng->symbols->GetModuleNameString( 
        DEBUG_MODNAME_MODULE,
        DEBUG_ANY_ID,
        BaseOffset,
        moduleNameBuf,
        sizeof( moduleNameBuf ),
        NULL );

    std::string moduleName = "";
    if ( hres == S_OK )
        moduleName = std::string( moduleNameBuf );

    boost::recursive_mutex::scoped_lock l(m_handlerLock);

    HandlerList::iterator  it = m_handlers.begin();

    for ( ; it != m_handlers.end(); ++it )
    {
        PyThread_StateSave pyThreadSave( it->pystate );

        DEBUG_CALLBACK_RESULT  ret = it->callback->OnModuleUnload( BaseOffset, moduleName );

        result = ret != DebugCallbackNoChange ? ret : result;
    }


    return ConvertCallbackResult( result );
}

///////////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugEngine::Exception(
    __in PEXCEPTION_RECORD64 Exception,
    __in ULONG FirstChance )
{
    DEBUG_CALLBACK_RESULT result = DebugCallbackNoChange;

    boost::recursive_mutex::scoped_lock l(m_handlerLock);

    HandlerList::iterator  it = m_handlers.begin();

    for ( ; it != m_handlers.end(); ++it )
    {
        PyThread_StateSave pyThreadSave( it->pystate );

        DEBUG_CALLBACK_RESULT  ret = it->callback->OnException( 
            ExceptionInfoPtr( new ExceptionInfo(FirstChance, *Exception) ) 
        );

        result = ret != DebugCallbackNoChange ? ret : result;
    }

    return ConvertCallbackResult( result );
}

///////////////////////////////////////////////////////////////////////////////

ULONG64
getCurrentProcess()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT         hres;  
    ULONG64         processAddr = 0;
    
    hres = g_dbgEng->system->GetImplicitProcessDataOffset( &processAddr );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::GetImplicitProcessDataOffset  failed" ); 
        
     return processAddr; 
}

///////////////////////////////////////////////////////////////////////////////

ULONG64 
getImplicitThread()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres; 
    ULONG64     threadOffset = -1;

    hres = g_dbgEng->system->GetImplicitThreadDataOffset( &threadOffset );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::GetImplicitThreadDataOffset  failed" ); 
        
    return threadOffset;
}

///////////////////////////////////////////////////////////////////////////////

void setCurrentProcess( ULONG64 processAddr )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    processAddr = addr64NoSafe(processAddr);
    hres = g_dbgEng->system->SetImplicitProcessDataOffset( processAddr );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::SetImplicitProcessDataOffset  failed" );
}

///////////////////////////////////////////////////////////////////////////////

void setImplicitThread( ULONG64 threadAddr )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;

    threadAddr = addr64NoSafe(threadAddr);
    hres = g_dbgEng->system->SetImplicitThreadDataOffset( threadAddr );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::SetImplicitThreadDataOffset  failed" );
}

///////////////////////////////////////////////////////////////////////////////

void getAllProcessThreads( std::vector<ULONG64> &threadsArray )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG       debugClass, debugQualifier;
    
    hres = g_dbgEng->control->GetDebuggeeType( &debugClass, &debugQualifier );
    
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetDebuggeeType  failed" );   
         
    if  ( debugClass != DEBUG_CLASS_USER_WINDOWS )
        throw DbgException( "getAllProcessThreads routine only for usermode" );

    ULONG  threadsNumber = 0;

    hres = g_dbgEng->system->GetNumberThreads( &threadsNumber );
    if ( FAILED(hres) )
        throw DbgException( "IDebugSystemObjects::GetNumberThreads failed" );

    std::vector<ULONG>  ids(threadsNumber);

    hres = g_dbgEng->system->GetThreadIdsByIndex( 0, threadsNumber, &ids[0], NULL );
    if ( FAILED(hres) )
        throw DbgException( "IDebugSystemObjects::GetThreadIdsByIndex failed" );

    ULONG  currentThreadId;
    hres = g_dbgEng->system->GetCurrentThreadId( &currentThreadId );
    if ( FAILED(hres) )
        throw DbgException( "IDebugSystemObjects::GetCurrentThreadId failed" );

    threadsArray.resize( threadsNumber );
    for ( size_t i = 0; i < threadsNumber; ++i )
    {
        hres = g_dbgEng->system->SetCurrentThreadId( ids[i] );
        if ( FAILED(hres) )
        {   
            g_dbgEng->system->SetCurrentThreadId( currentThreadId );
            throw DbgException( "IDebugSystemObjects::SetCurrentThreadId failed" );
        }

        hres = g_dbgEng->system->GetCurrentThreadTeb( &threadsArray[i] );
        if ( FAILED(hres) )
        {   
            g_dbgEng->system->SetCurrentThreadId( currentThreadId );
            throw DbgException( "IDebugSystemObjects::GetCurrentThreadTeb failed" );
        }
    }

     g_dbgEng->system->SetCurrentThreadId( currentThreadId );
}

///////////////////////////////////////////////////////////////////////////////

ULONG64 loadExtension(const std::wstring &extPath )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;
    ULONG64  handle = 0;

    hres = g_dbgEng->control->AddExtensionWide( extPath.c_str(), 0, &handle );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::AddExtension failed" );

    return handle;
}

///////////////////////////////////////////////////////////////////////////////

void removeExtension( ULONG64 extHandle )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    g_dbgEng->control->RemoveExtension( extHandle );
}

///////////////////////////////////////////////////////////////////////////////

std::string callExtension( ULONG64 extHandle, const std::wstring command, const std::wstring  &params  )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;

    OutputReader  outReader( g_dbgEng->client );

    hres = g_dbgEng->control->CallExtensionWide( extHandle, command.c_str(), params.c_str() );

    if ( FAILED( hres ) )
        throw  DbgException( "IDebugControl::CallExtension  failed" ); 
        
    return std::string( outReader.Line() );
}

///////////////////////////////////////////////////////////////////////////////


} // end pykd namespace


