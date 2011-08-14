#include "stdafx.h"

#include <intrin.h>

#include <iostream>

#include "utils.h"

int _tmain(int argc, _TCHAR* argv[])
{
    try
    {
        // Let test scripts to execute
        __debugbreak();
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

