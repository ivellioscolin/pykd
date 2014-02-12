#include "stdafx.h"

#include <boost\algorithm\string\case_conv.hpp>

#include "win/dbgeng.h"
#include "win/dbgio.h"
#include "win/windbg.h"
#include "dbgexcept.h"
#include "eventhandler.h"
#include "symengine.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

DebugEngine     g_dbgEng;

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

void writeDump( const std::wstring &fileName, bool smallDump )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    ULONG       debugClass, debugQualifier;
    
    hres = g_dbgEng->control->GetDebuggeeType( &debugClass, &debugQualifier );
    
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetDebuggeeType  failed" );   

    hres = g_dbgEng->client->WriteDumpFileWide(
        fileName.c_str(), 
        NULL,
        smallDump ? DEBUG_DUMP_SMALL : ( debugClass == DEBUG_CLASS_KERNEL ? DEBUG_DUMP_FULL : DEBUG_DUMP_DEFAULT ),
        DEBUG_FORMAT_DEFAULT,
        NULL );

    if ( FAILED(hres) )
        throw DbgException( "IDebugClient4::WriteDumpFileWide failed" );
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
        {
            if (E_UNEXPECTED == hres)
                throw WaitEventException();
            throw DbgException( "IDebugControl::WaitForEvent", hres );
        }

        hres = g_dbgEng->control->GetExecutionStatus( &currentStatus );

        if ( FAILED( hres ) )
            throw  DbgException( "IDebugControl::GetExecutionStatus", hres ); 

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

BaseTypeVariant evaluate( const std::wstring  &expression, bool cplusplus )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT             hres;
    ULONG64             value = 0;

    DEBUG_VALUE  debugValue = {};
    ULONG        remainderIndex = 0;
    ULONG        expresionSyntax;
    
    hres = g_dbgEng->control->GetExpressionSyntax( &expresionSyntax );
    if ( FAILED(hres) )
    {
        throw  DbgException( "IDebugControl3::GetExpressionSyntax failed" );
    }

    hres = g_dbgEng->control->SetExpressionSyntax( cplusplus ? DEBUG_EXPR_CPLUSPLUS : DEBUG_EXPR_MASM );
    if ( FAILED(hres) )
    {
        throw  DbgException( "IDebugControl3::GetExpressionSyntax failed" );
    }

    hres = g_dbgEng->control->EvaluateWide( 
        expression.c_str(), 
        DEBUG_VALUE_INVALID,
        &debugValue,
        &remainderIndex );

    if ( FAILED( hres ) )
    {
        g_dbgEng->control->SetExpressionSyntax( expresionSyntax );
        throw  DbgException( "IDebugControl::Evaluate  failed" );
    }

    BaseTypeVariant   var;

    switch( debugValue.Type )
    {
    case DEBUG_VALUE_INT8:
        var =  BaseTypeVariant( (LONG)debugValue.I8 );
        break;
        
    case DEBUG_VALUE_INT16:
        var =  BaseTypeVariant( (LONG)debugValue.I16 );
        break;
        
    case DEBUG_VALUE_INT32:
        var = BaseTypeVariant( debugValue.I32 );
        break;
        
    case DEBUG_VALUE_INT64:
        var =  BaseTypeVariant( debugValue.I64 );
        break;

    default:
        g_dbgEng->control->SetExpressionSyntax( expresionSyntax );
        throw DbgException("unsupported type");
    } 

    g_dbgEng->control->SetExpressionSyntax( expresionSyntax );

    return var;
}

///////////////////////////////////////////////////////////////////////////////

std::wstring debugCommand( const std::wstring &command )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT         hres;
    OutputReader    outReader( g_dbgEng->client );

    hres = g_dbgEng->control->ExecuteWide( DEBUG_OUTCTL_THIS_CLIENT, command.c_str(), 0 );

    if ( FAILED( hres ) )
        throw  DbgException( "IDebugControl::ExecuteWide  failed" ); 

    return outReader.Line();
}

///////////////////////////////////////////////////////////////////////////////

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

ULONG getSystemUptime()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;
    ULONG  time;

    hres = g_dbgEng->control->GetCurrentSystemUpTime( &time );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl2::GetCurrentSystemUpTime", hres );

    return time;
}

///////////////////////////////////////////////////////////////////////////////

ULONG getCurrentTime()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;
    ULONG  time;

    hres = g_dbgEng->control->GetCurrentTimeDate( &time );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl2::GetCurrentTimeDate", hres );

    return time;
}

///////////////////////////////////////////////////////////////////////////////

SystemVersionPtr getSystemVersion()
{
    SystemVersionPtr sysVer( new SystemVersion );
    ULONG kdMajor;

    boost::scoped_array< CHAR > arrSpString( new CHAR[MAX_PATH + 1] );
    memset(&arrSpString[0], 0, MAX_PATH + 1);

    boost::scoped_array< CHAR > arrBuildString( new CHAR[MAX_PATH + 1] );
    memset(&arrBuildString[0], 0, MAX_PATH + 1);

    ULONG tmp;

    HRESULT hres = 
        g_dbgEng->control->GetSystemVersion(
            &sysVer->platformId,
            &kdMajor,
            &sysVer->buildNumber,
            &arrSpString[0],
            MAX_PATH,
            &tmp,
            &tmp,
            &arrBuildString[0],
            MAX_PATH,
            &tmp);
    if (S_OK != hres)
        throw DbgException("IDebugControl::GetSystemVersion", hres);

    sysVer->buildString = &arrBuildString[0];
    sysVer->servicePackString = &arrSpString[0];
    sysVer->isCheckedBuild = 0xC == kdMajor;

    hres = 
        g_dbgEng->control->GetSystemVersionValues(
            &sysVer->platformId,
            &sysVer->win32Major,
            &sysVer->win32Minor,
            NULL,
            NULL);
    if (S_OK != hres)
        throw DbgException("IDebugControl::GetSystemVersionValues", hres);

    return sysVer;
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

void getCurrentFrame(STACK_FRAME_DESC &frame )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;

    frame.number = 0;
    
    hres = g_dbgEng->registers->GetInstructionOffset2( DEBUG_REGSRC_FRAME, &frame.instructionOffset );
    if ( FAILED(hres) )
        throw DbgException( "IDebugRegisters2::GetInstructionOffset2", hres );

    hres = g_dbgEng->control->GetReturnOffset( &frame.returnOffset );
    if ( FAILED(hres) )
        throw DbgException( "IDebugControl::GetReturnOffset", hres );

    hres = g_dbgEng->registers->GetFrameOffset2( DEBUG_REGSRC_FRAME, &frame.frameOffset );
    if ( FAILED(hres) )
        throw DbgException( "IDebugRegisters2::GetFrameOffset2", hres );

    hres = g_dbgEng->registers->GetStackOffset2( DEBUG_REGSRC_FRAME, &frame.stackOffset );
    if ( FAILED(hres) )
        throw DbgException( "IDebugRegisters2::GetStackOffset2", hres );
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

    WOW64_CONTEXT Context;
    ReadWow64Context(Context);

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

    ULONG   filledFrames = 1024;
    std::vector<DEBUG_STACK_FRAME> dbgFrames(filledFrames);

    hres = g_dbgEng->control->GetContextStackTrace(
        &Context,
        sizeof(Context),
        &dbgFrames[0],
        filledFrames,
        NULL,
        filledFrames*sizeof(Context),
        sizeof(Context),
        &filledFrames );

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

    if ( hardware )
    {
        HRESULT hres = bp->SetDataParameters(size, accessType);
        if (S_OK != hres)
        {
            g_dbgEng->control->RemoveBreakpoint(bp);
            throw DbgException("IDebugBreakpoint::SetDataParameters", hres);
        }
    }

    bpFlags |= DEBUG_BREAKPOINT_ENABLED | DEBUG_BREAKPOINT_GO_ONLY;
    hres = bp->SetFlags(bpFlags);
    if (S_OK != hres)
    {
        g_dbgEng->control->RemoveBreakpoint(bp);
        throw DbgException("IDebugBreakpoint::SetFlags", hres);
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

HRESULT STDMETHODCALLTYPE DebugEngine::ChangeEngineState(
    __in ULONG Flags,
    __in ULONG64 Argument )
{
    boost::recursive_mutex::scoped_lock l(m_handlerLock);

    HandlerList::iterator  it = m_handlers.begin();

    if ( ( ( Flags & DEBUG_CES_EXECUTION_STATUS ) != 0 ) &&
         ( ( Argument & DEBUG_STATUS_INSIDE_WAIT ) == 0 ) &&
         (ULONG)Argument != previousExecutionStatus )
    {
        if ( previousExecutionStatus == DEBUG_STATUS_NO_DEBUGGEE &&
             (ULONG)Argument != DEBUG_STATUS_GO )
                return S_OK;

        for ( ; it != m_handlers.end(); ++it )
        {

            PyThread_StateSave pyThreadSave( it->pystate );

            it->callback->onExecutionStatusChange( (ULONG)Argument );
        }

        previousExecutionStatus = (ULONG)Argument;
    }

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugEngine::ChangeSymbolState(
    __in ULONG Flags,
    __in ULONG64 Argument )
{
    HandlerList::iterator it;

    if (Flags & DEBUG_CSS_LOADS)
    {
        for ( it = m_handlers.begin(); it != m_handlers.end(); ++it )
        {
            PyThread_StateSave pyThreadSave( it->pystate );
            it->callback->onSymbolsLoaded( Argument );
        }
    }

    if (Flags & DEBUG_CSS_UNLOADS)
    {
        for ( it = m_handlers.begin(); it != m_handlers.end(); ++it )
        {
            PyThread_StateSave pyThreadSave( it->pystate );
            it->callback->onSymbolsUnloaded( Argument );
        }
    }

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugEngine::StartInput(
    __in ULONG BufferSize )
{
    std::string s = "";

    {
        PyThread_StateSave pyThreadSave( g_dbgEng->pystate );

        python::object ret = python::eval( "raw_input(\"input>\")" );

        s = python::extract<std::string>( ret );
    }

    g_dbgEng->control->ReturnInput ( s.c_str() );

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugEngine::EndInput()
{
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////


DebugEngine::DbgEngBind* DebugEngine::operator->() 
{
    if ( m_bind.get() != NULL )
        return m_bind.get();

    CComPtr<IDebugClient4>   client = NULL;

    HRESULT  hres = DebugCreate( __uuidof(IDebugClient4), (void **)&client );
    if ( FAILED( hres ) )
        throw DbgException("DebugCreate failed");

    m_bind.reset(new DbgEngBind(client, this) );

    if ( !WindbgGlobalSession::isInit() )
    {
        python::object   main = boost::python::import("__main__");

        python::object   main_namespace = main.attr("__dict__");

        python::object   pykd = boost::python::import( "pykd" );

        main_namespace["globalEventHandler"] = EventHandlerPtr( new EventHandlerImpl() );

        client->SetInputCallbacks( this );
    }

    return m_bind.get();
}

///////////////////////////////////////////////////////////////////////////////

static std::wstring getExtensionSearchPathImpl()
{
    ULONG chars = MAX_PATH;
    for (; ; )
    {
        std::vector< wchar_t > rawPath(chars + 1, L'\0');
        HRESULT hres = 
            g_dbgEng->advanced->Request(
                DEBUG_REQUEST_GET_EXTENSION_SEARCH_PATH_WIDE,
                NULL,
                0,
                &rawPath[0],
                chars * sizeof(wchar_t),
                NULL);
        if (S_OK == hres)
        {
            std::wstring result = &rawPath[0];
            return result;
        }

        if (S_FALSE == hres)
        {
            // The method was successful. However, the output would not fit in the 
            // output buffer OutBuffer, so truncated output was returned
            chars *= 2; 
            continue;
        }

        throw DbgException( "IDebugAdvanced::Request", hres );
    }
}

///////////////////////////////////////////////////////////////////////////////

std::wstring getExtensionSearchPath()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );
    return getExtensionSearchPathImpl();
}

///////////////////////////////////////////////////////////////////////////////

ULONG64 loadExtension(const std::wstring &extPath )
{
    HRESULT   hres;

    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );
    
     ULONG64  handle = 0;

    std::vector< wchar_t > rawPath(MAX_PATH + 1, L'\0');
    DWORD ret = 
        ::SearchPath(
            getExtensionSearchPathImpl().c_str(),
            extPath.c_str(),
            L".dll",
            MAX_PATH,
            &rawPath[0],
            NULL);
    if (!ret)
        throw DbgException( "extension not found" );

    struct _scoped_lib
    {
        _scoped_lib(const wchar_t *wsz) : m_hmod(::LoadLibrary(wsz)) {}
        ~_scoped_lib() { if (m_hmod) ::FreeLibrary(m_hmod);}
        HMODULE m_hmod;
    } scoped_lib(&rawPath[0]);
    if (!scoped_lib.m_hmod)
    {
        std::stringstream  sstr;
        sstr << "failed to load extension with error " << std::dec << GetLastError();
        throw DbgException( sstr.str() );
    }

    hres = g_dbgEng->control->AddExtensionWide( extPath.c_str(), 0, &handle );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::AddExtension", hres );

    // inderect call of dbgeng!ExtensionInfo::Load
    FARPROC dummy = NULL;
    g_dbgEng->control->GetExtensionFunctionWide(handle, L"dummy", &dummy);

    return handle;
}


///////////////////////////////////////////////////////////////////////////////

ULONG64 addExtension(const std::wstring &extPath )
{
    HRESULT   hres;

    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    ULONG64  handle = 0;

    hres = g_dbgEng->control->AddExtensionWide( extPath.c_str(), 0, &handle );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::AddExtension", hres );

    return handle;
}

///////////////////////////////////////////////////////////////////////////////

ULONG64 getExtension(const std::wstring &extPath )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;
    ULONG64  handle = 0;

    hres = g_dbgEng->control->GetExtensionByPathWide( extPath.c_str(), &handle );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetExtensionByPath", hres );

    return handle;
}

///////////////////////////////////////////////////////////////////////////////

void removeExtension(const std::wstring &extPath )
{
    HRESULT  hres;

    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );
    
    ULONG64  handle = 0;

    hres = g_dbgEng->control->GetExtensionByPathWide( extPath.c_str(), &handle );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetExtensionByPath", hres );

    g_dbgEng->control->RemoveExtension( handle );
}

///////////////////////////////////////////////////////////////////////////////

void removeExtension( ULONG64 extHandle )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    g_dbgEng->control->RemoveExtension( extHandle );
}

///////////////////////////////////////////////////////////////////////////////

std::wstring callExtension( ULONG64 extHandle, const std::wstring command, const std::wstring  &params  )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;

    OutputReader  outReader( g_dbgEng->client );

    hres = g_dbgEng->control->CallExtensionWide( extHandle, command.c_str(), params.c_str() );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::CallExtension", hres ); 

    return std::wstring( outReader.Line() );
}

///////////////////////////////////////////////////////////////////////////////

} // end pykd namespace


