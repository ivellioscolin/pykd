#include "stdafx.h"

#include <string>
#include <memory>

namespace utils
{

std::string GetWinErrorText(DWORD dwError)
{
    HLOCAL hLocal = NULL;   // Buffer that gets the error message string

    // Get the error code's textual description
    DWORD strSize = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL, dwError, 0 /* Default language */, (LPSTR) &hLocal, 0, NULL);
    
    if (!strSize)
    {
        // Is it a network-related error?
        HMODULE hDll = LoadLibraryEx(TEXT("netmsg.dll"), NULL, DONT_RESOLVE_DLL_REFERENCES);
        if (hDll == INVALID_HANDLE_VALUE) 
            return std::string("Can't load netmsg.dll");
        std::tr1::shared_ptr<void> guard1(hDll, ::FreeLibrary);
        strSize = FormatMessageA(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_FROM_SYSTEM,
            hDll, dwError,  0 /* Default language */, (LPSTR) &hLocal,  0,  NULL);
    }
    
    if (strSize != NULL)
    {
        std::tr1::shared_ptr<void> guard2(hLocal, ::LocalFree);
        std::string str;
        str += reinterpret_cast<const char*>(::LocalLock(hLocal));
        return str;
    }

    return std::string("Unknown error.");
}

std::string GetLastErrorStr()
{
    return GetWinErrorText(::GetLastError());
}

} // namespace utils