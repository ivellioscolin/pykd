#include "stdafx.h"

#include <intrin.h>

#include <iostream>

#include "utils.h"

void FuncWithName0()
{
}

void FuncWithName1()
{
}

int _tmain(int argc, _TCHAR* argv[])
{
    try
    {
        // Let test scripts to execute
        __debugbreak();
        FuncWithName0();
        FuncWithName1();
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

