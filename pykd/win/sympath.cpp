// 
// Win-[DbgEng]: Debug symbols path
// 

////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbgeng.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

std::string getSymbolPath()
{
    ULONG retSymPathChars = 0;
    g_dbgEng->symbols->GetSymbolPath(NULL, 0, &retSymPathChars);
    if (!retSymPathChars)
        return std::string("");

    const ULONG symPathChars = retSymPathChars + 1;
    boost::scoped_array< CHAR > symPath( new CHAR [symPathChars] );
    RtlZeroMemory(symPath.get(), sizeof(CHAR) * symPathChars);
    HRESULT hres = 
        g_dbgEng->symbols->GetSymbolPath(symPath.get(), symPathChars, &retSymPathChars);
    if (S_OK != hres)
        throw DbgException("IDebugSymbols::GetSymbolPath", hres);

    return std::string( symPath.get() );
}

////////////////////////////////////////////////////////////////////////////////

void setSymbolPath(const std::string &symPath)
{
    g_dbgEng->symbols->SetSymbolPath(symPath.c_str());
}

////////////////////////////////////////////////////////////////////////////////

void appendSymbolPath(const std::string &symPath)
{
    g_dbgEng->symbols->AppendSymbolPath(symPath.c_str());
}

////////////////////////////////////////////////////////////////////////////////

}   // namespace pykd

////////////////////////////////////////////////////////////////////////////////
