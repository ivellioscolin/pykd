#include "stdafx.h"
#include "pysupport.h"

#include "dbgengine.h"
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace pykd {
namespace pysupport {

///////////////////////////////////////////////////////////////////////////////

python::list getProcessThreads()
{
    std::vector<ULONG64>  threads;
    getAllProcessThreads( threads );

    python::list  threadsLst;

    std::vector<ULONG64>::iterator  it;
    for ( it = threads.begin(); it != threads.end(); ++it )
        threadsLst.append( *it );

    return threadsLst;
}

///////////////////////////////////////////////////////////////////////////////

} } //pykd::support namespace end