// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

//#define BOOST_PYTHON_STATIC_LIB

#pragma warning(push)
// 'return' : conversion from 'Py_ssize_t' to 'unsigned int', possible loss of data
#pragma warning(disable:4244)
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/module.hpp>
#pragma warning(pop)

namespace python = boost::python;



