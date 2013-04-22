#pragma once

#include <dia2.h>

#include "symengine.h"

#include "dia\diaexcept.h"


namespace pykd {

//////////////////////////////////////////////////////////////////////////////

typedef CComPtr< IDiaSymbol > DiaSymbolPtr;
typedef CComPtr< IDiaEnumSymbols > DiaEnumSymbolsPtr;
typedef CComPtr< IDiaDataSource > DiaDataSourcePtr;
typedef CComPtr< IDiaSession > DiaSessionPtr;
typedef CComPtr< IDiaEnumSymbolsByAddr > DiaEnumSymbolsByAddrPtr;
typedef CComPtr< IDiaEnumLineNumbers > DiaEnumLineNumbersPtr;
typedef CComPtr< IDiaLineNumber> DiaLineNumberPtr;
typedef CComPtr< IDiaSourceFile > DiaSourceFilePtr;

//////////////////////////////////////////////////////////////////////////////

} // end pykd namespace
