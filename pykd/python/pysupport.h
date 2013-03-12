#pragma once

#include <boost/python/list.hpp>

#include "module.h"

///////////////////////////////////////////////////////////////////////////////

namespace pykd {
namespace pysupport {

python::list getProcessThreads();

python::tuple getBugCheckData();

python::tuple findSymbolAndDisp( ULONG64 offset );

python::tuple moduleFindSymbolAndDisp( pykd::Module &module, ULONG64 offset );

} } //pykd::support namespace end

///////////////////////////////////////////////////////////////////////////////
