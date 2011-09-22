#include "stdafx.h"

#include <intrin.h>

#include <iostream>
#include <string>

#include "utils.h"

const ULONG g_constNumValue = 0x5555;
const bool g_constBoolValue = true;

UCHAR g_ucharValue = 1;
USHORT g_ushortValue = 2;
ULONG g_ulongValue = 4;
ULONGLONG g_ulonglongValue = 8;

std::string g_string;

struct structWithBits {
    ULONG m_bit0_4  : 5;
    ULONG m_bit5    : 1;
    ULONG m_bit6_7  : 2;
};
structWithBits g_structWithBits = {0};

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

