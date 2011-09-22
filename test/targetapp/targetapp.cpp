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

union unionTest {
    ULONG m_value;
    structWithBits m_bits;
};

class classBase {
public:
    int m_baseField;
    void baseMethod() const {}
    virtual void virtFunc() =  0;
    virtual void virtFunc2() =  0;
};

struct structTest {
    ULONG m_field0;
    ULONGLONG m_field1;
    bool m_field2;
    USHORT m_field3;
};

class classChild : public classBase {
public:
    int m_childField;
    int m_childField2;
    void childMethod() const {}
    virtual void virtFunc() {}
    virtual void virtFunc2() {}
};

struct struct2 {
    structTest m_struct;
    unionTest m_union;
    int m_field;
};

void FuncWithName0()
{
    classChild _classChild;
    _classChild.baseMethod();

    reinterpret_cast<classChild *>(&_classChild)->virtFunc2();
}

void FuncWithName1()
{
    unionTest _unionTest;
    _unionTest.m_value = 0;
    structTest _structTest;
    _structTest.m_field1 = 1;
    struct2 _struct2;
    RtlZeroMemory(&_struct2, sizeof(_struct2));
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

