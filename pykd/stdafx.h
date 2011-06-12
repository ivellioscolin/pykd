// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:

#include <tchar.h>
#include <windows.h>



#ifndef  __field_ecount_opt
#define  __field_ecount_opt(x)  
#endif // __field_ecount_opt


#define BOOST_PYTHON_STATIC_LIB

#pragma warning(push)
// 'return' : conversion from 'Py_ssize_t' to 'unsigned int', possible loss of data
#pragma warning(disable:4244)
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#pragma warning(pop)

#include <vector>

template <typename TElem>
TElem *getVectorBuffer(std::vector<TElem> &vec)
{
    return vec.size() ? &vec[0] : NULL;
}
template <typename TElem>
const TElem *getVectorBuffer(const std::vector<TElem> &vec)
{
    return vec.size() ? &vec[0] : NULL;
}

// TODO: reference additional headers your program requires here
