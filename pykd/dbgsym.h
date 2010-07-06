#pragma once

#include <string>

#include <boost/python.hpp>
#include <boost/python/object.hpp>

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
findSymbolForAddress( ULONG64 addr );

ULONG64
findAddressForSymbol( const std::string  &moduleName, const std::string  &symbolName );

/////////////////////////////////////////////////////////////////////////////////
