#pragma once

#include <boost/python/list.hpp>

#include "module.h"
#include "dbgengine.h"

///////////////////////////////////////////////////////////////////////////////

namespace pykd {
namespace pysupport {

python::list getProcessThreads();

python::tuple getBugCheckData();

python::tuple findSymbolAndDisp( ULONG64 offset );

python::tuple moduleFindSymbolAndDisp( pykd::Module &module, ULONG64 offset );

std::string printSystemVersion(SystemVersionPtr sysVer);

} } //pykd::support namespace end

///////////////////////////////////////////////////////////////////////////////
