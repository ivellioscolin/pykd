#include "stdafx.h"
#include "pysupport.h"

#include "dbgengine.h"
#include "typeinfo.h"
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

python::tuple getBugCheckData()
{
    BUG_CHECK_DATA bugCheckData;
    readBugCheckData(bugCheckData);
    return python::make_tuple(bugCheckData.code, bugCheckData.arg1, bugCheckData.arg2, bugCheckData.arg3, bugCheckData.arg4);
}

python::tuple findSymbolAndDisp( ULONG64 offset )
{
    std::string  symbolName;
    LONG  displacement;

    pykd::TypeInfo::findSymbolAndDisp( offset, symbolName, displacement );

    return python::make_tuple(symbolName,displacement);
}

python::tuple moduleFindSymbolAndDisp( pykd::Module &module, ULONG64 offset )
{
    std::string  symbolName;
    LONG  displacement;

    module.getSymbolAndDispByVa( offset, symbolName, displacement );

    return python::make_tuple(symbolName,displacement);
}


///////////////////////////////////////////////////////////////////////////////

} } //pykd::support namespace end