#include "stdafx.h"

#include <boost\algorithm\string\case_conv.hpp>

#include "win/dbgeng.h"
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
    PyThreadState   *pystate = PyEval_SaveThread();

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
    
    hres = g_dbgEng->control->IsPointer64Bit();
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::IsPointer64Bit failed" );
        
    return hres == S_OK;
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

ULONG getStackTraceFrameCount()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;
    ULONG  filledFrames;

    hres = g_dbgEng->control->GetStackTrace( 0, 0, 0, NULL, 0, &filledFrames );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetStackTrace  failed" );

    return filledFrames;
}

///////////////////////////////////////////////////////////////////////////////

void getStackTrace( STACK_FRAME_DESC* frames, ULONG frameCount, ULONG* frameReturned )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    ULONG   filledFrames;
    std::vector<DEBUG_STACK_FRAME>  stack(frameCount); 

    hres = g_dbgEng->control->GetStackTrace( 0, 0, 0, &stack[0], frameCount, &filledFrames);
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetStackTrace  failed" );

    for ( ULONG i = 0; i < filledFrames; ++i )
    {
        frames[i].number = stack[i].FrameNumber;
        frames[i].instructionOffset = stack[i].InstructionOffset;
        frames[i].returnOffset = stack[i].ReturnOffset;
        frames[i].frameOffset = stack[i].FrameOffset;
        frames[i].stackOffset = stack[i].StackOffset;
    }

    if ( frameReturned )
        *frameReturned = filledFrames;
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

} // end pykd namespace


