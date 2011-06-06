#include "stdafx.h"

#include <intrin.h>

#include <iostream>

#include "utils.h"

OSVERSIONINFOA gVerInfo = {0};

void test_getOffset_typedVar()
{
    gVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    if (!::GetVersionExA(&gVerInfo))
        throw std::runtime_error("Can't get OS version: " + utils::GetLastErrorStr());
    __debugbreak();
}

int _tmain(int argc, _TCHAR* argv[])
{
    try
    {
	    test_getOffset_typedVar();
    }
    catch(std::exception & ex)
    {
        std::cout << ex.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cout << "Unknown error" << std::endl;
        return 1;
    }
    return 0;
}

