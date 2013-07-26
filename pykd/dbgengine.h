#pragma once

#include <boost/python/tuple.hpp>
namespace python = boost::python;

#include "kdlib/dbgengine.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

kdlib::ExecutionStatus targetGo();
void targetBreak();
kdlib::ExecutionStatus targetStep();
kdlib::ExecutionStatus targetStepIn();

kdlib::PROCESS_DEBUG_ID startProcess( const std::wstring  &processName );
kdlib::PROCESS_DEBUG_ID attachProcess( kdlib::PROCESS_ID pid );
void loadDump( const std::wstring &fileName );

std::wstring debugCommand( const std::wstring &command );
unsigned long long evaluate( const std::wstring  &expression );

python::tuple getSourceLine( kdlib::MEMOFFSET_64 offset = 0 );

python::tuple findSymbolAndDisp( ULONG64 offset );

kdlib::SystemInfo getSystemVersion();

std::wstring printSystemVersion( kdlib::SystemInfo& sysInfo );

///////////////////////////////////////////////////////////////////////////////

} //end namespace pykd