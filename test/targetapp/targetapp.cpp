#include "stdafx.h"

#include <intrin.h>

#include <iostream>
#include <string>

#include "utils.h"

#pragma pack( push, 4 )

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
    structTest* m_field4;
};

structWithBits g_structWithBits = {0};

structTest      g_structTest = { 0, 500, true, 1, NULL };
structTest      g_structTest1 = { 0, 500, true, 1, &g_structTest };

structTest      g_testArray[2] = { { 0, 500, true, 1 }, { 2, 1500, false, 1 } };

structTest      *g_structTestPtr = &g_structTest;
structTest      **g_structTestPtrPtr = &g_structTestPtr;

char            helloStr[] = "Hello";
wchar_t         helloWStr[] = L"Hello";

unsigned char ucharArray[] = {0, 10, 0x78, 128, 0xFF };
unsigned short ushortArray[] = {0, 10, 0xFF, 0x8000, 0xFFFF };
unsigned long ulongArray[] = {0, 0xFF, 0x8000, 0x80000000, 0xFFFFFFFF };
long longArray[] = {0, -10, -2000, -100000, 0xFFFFFFFF };
unsigned __int64 ulonglongArray[] = {0, 0xFF, 0xFFFFFFFF, 0x8000000000000000, 0xFFFFFFFFFFFFFFFF };
long long longlongArray[] = {0, -10, -2000, -100000, -10000000000 };

int intMatrix[2][3] = { { 0, 1, 2}, { 3, 4, 5 } };
int intMatrix2[2][3] = { { 0, 1, 2}, { 3, 4, 5 } };
int intMatrix3[2][3] = { { 0, 1, 2}, { 3, 4, 5 } };
int intMatrix4[2][3] = { { 0, 1, 2}, { 3, 4, 5 } };
char* strArray[] = { "Hello", "Bye" };
int (*ptrIntMatrix)[2][3] = &intMatrix;

// kd> x targetapp!arrIntMatrixPtrs
// xxxxxxxx targetapp!arrIntMatrixPtrs = int (*[4])[2][3]
int (* arrIntMatrixPtrs[4])[2][3] = {
    &intMatrix, &intMatrix2, &intMatrix3, &intMatrix4
};

int ((*ptrIntMatrix1))[2][3] = &intMatrix;

char *(*ptrStrArray)[2] = &strArray;

class classChild : public classBase {
public:
    int m_childField;
    int m_childField2;
    structTest m_childField3;
    void childMethod() const {}
    virtual void virtFunc() {}
    virtual void virtFunc2() {}
};

struct struct2 {
    structTest m_struct;
    unionTest m_union;
    int m_field;
};

struct struct3 {
    int   m_arrayField[2];
    int   m_noArrayField;
};

struct3  g_struct3 = { { 0, 2 }, 3 };

__int64  g_bigValue = 0x8080808080808080;

void FuncWithName0()
{
    classChild _classChild;
    _classChild.baseMethod();

    reinterpret_cast<classChild *>(&_classChild)->virtFunc2();
    std::cout << _classChild.m_childField2;
    std::cout << g_constNumValue;
    std::cout << g_constBoolValue;
    std::cout << g_ucharValue;
    std::cout << g_ushortValue;
    std::cout << g_ulongValue;
    std::cout << g_ulonglongValue;

    std::cout << g_structTest.m_field0;
    std::cout << g_testArray[1].m_field3;
    std::cout << g_structTestPtr->m_field3;
    std::cout << (*g_structTestPtrPtr)->m_field3;

    std::cout << helloStr;
    std::wcout << helloWStr;
    std::cout << ucharArray[2];
    std::cout << ushortArray[2];
    std::cout << ulongArray[2];
    std::cout << ulonglongArray[2];

    std::cout << intMatrix[1][1];
    std::cout << strArray[0];
    std::cout << (*ptrIntMatrix)[0][1];
    std::cout << g_struct3.m_noArrayField;
}

void FuncWithName1(int a)
{
    unionTest _unionTest[2] = {0};
    _unionTest[1].m_value = 0;
    structTest _structTest;
    _structTest.m_field1 = a;
    struct2 _struct2;
    RtlZeroMemory(&_struct2, sizeof(_struct2));

    std::cout << _unionTest[0].m_value;
    std::cout << _structTest.m_field1;
    std::cout << _struct2.m_struct.m_field1;
    std::cout << g_string;
}

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////

int doLoadUnload()
{
    __debugbreak();
    HMODULE hmod = ::LoadLibrary( _T("iphlpapi.dll") );
    if (hmod)
        ::FreeLibrary(hmod);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
    try
    {
        // Let test scripts to execute
        __debugbreak();

        if (2 == argc)
        {
            // run with parameters
            if ( !_tcsicmp(argv[1], _T("-testLoadUnload")) )
                return doLoadUnload();
        }

        __debugbreak();
        __debugbreak();
        __debugbreak();
        FuncWithName0();
        FuncWithName1(2);
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

