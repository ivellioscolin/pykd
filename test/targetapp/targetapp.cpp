#include "stdafx.h"

#include <intrin.h>

#include <iostream>
#include <string>

#include "utils.h"

////////////////////////////////////////////////////////////////////////////////

#pragma pack( push, 4 )

const ULONG g_constNumValue = 0x5555;

// 
// kd> x targetapp!g_constBoolValue
// *** nothing *** 
// 
// kd> > u 01331995 
// targetapp!FuncWithName0+0x75 [c:\projects\pykd\branch\0.1.x\test\targetapp\targetapp.cpp @ 198]:
// 01331995 8bf4            mov     esi,esp
// 01331997 6a01            push    1   ; << g_constBoolValue
// 01331999 8b0d84043401    mov     ecx,dword ptr [targetapp!_imp_?coutstd (01340484)]
// 0133199f ff1578043401    call    dword ptr [targetapp!_imp_??6?$basic_ostreamDU?$char_traitsDstdstdQAEAAV01_NZ (01340478)]
const bool g_constBoolValue = true;

UCHAR g_ucharValue = 1;
USHORT g_ushortValue = 2;
ULONG g_ulongValue = 4;
ULONGLONG g_ulonglongValue = 8;
ULONGLONG *g_pUlonglongValue = &g_ulonglongValue;

CHAR g_charValue = -1;
SHORT g_shortValue = -2;
LONG g_longValue = -4;
LONGLONG g_longlongValue = -8;

float g_float = 5.123456f;
double g_double = 5.1234567891;

std::string g_string;

struct structWithBits {
    ULONG m_bit0_4  : 5;
    ULONG m_bit5    : 1;
    ULONG m_bit6_7  : 2;
};


union unionTest {
    ULONG m_value;
    double m_doubleValue;
    structWithBits m_bits;
};

class classBase {
public:
    int m_baseField;
    void baseMethod() const {}
    virtual void virtFunc() =  0;
    virtual void virtFunc2() =  0;

    classBase() :
        m_baseField( -100 )
        {}
};

class anotherBase {
    static std::string   m_stdstr;
    ULONG   m_count;
    int m_baseField;
public:
    anotherBase() : 
        m_count( 1234 ),
        m_baseField( 100 )
        {}
};
std::string   anotherBase::m_stdstr = "hello";


struct structTest {
    ULONG m_field0;
    ULONGLONG m_field1;
    bool m_field2;
    USHORT m_field3;
    structTest* m_field4;
};

struct structNullSize {
};

structNullSize* g_nullSizeArray = 0;

struct structAbstract;
typedef struct structAbstract  *pstructAbstract;

pstructAbstract g_structAbstract = 0;

structWithBits g_structWithBits = { 4, 1, 3};

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
float floatArray[] = { 1.0f, 2.001f, -3.0004f };
double doubleArray[] = { 1.0, 2.0000001, -3.0000004 };

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

enum enumType {

    ONE = 1,
    TWO = 2,
    THREE = 3
};

const enumType g_constEnumThree = THREE;

const ULONG g_constUlong = 0xffffff;
const ULONGLONG g_constUlonglong = 0xffffff000000;

class classChild : public classBase, public anotherBase {

public:

    static const int  m_staticConst = 100;

    static int  m_staticField;

public:
    int m_childField;
    int m_childField2;
    structTest m_childField3;
    enumType m_enumField;
    void childMethod() const {}
    virtual void virtFunc() {}
    virtual void virtFunc2() {}

    classChild() :
        m_enumField( THREE )
        {}
};

struct YetAnotherChild_class : classBase
{
    int m_i;
    YetAnotherChild_class() : m_i(0) {}
    virtual void virtFunc() {}
    virtual void virtFunc2() {}
};


int classChild::m_staticField = 200;

classChild      g_classChild;

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


static LIST_ENTRY       g_listHead;

struct listStruct {
    int             num;
    LIST_ENTRY      listEntry;
};

listStruct  g_listItem1 = { 1 };
listStruct  g_listItem2 = { 2 };
listStruct  g_listItem3 = { 3 };

LIST_ENTRY entry1;
LIST_ENTRY entry2;

struct listStruct1;

static listStruct1       *g_listHead1 = NULL;

listStruct1* g_arrOfListStruct1[] = {
    g_listHead1, g_listHead1
};

void*  g_voidPtr = g_listHead1;

void *g_arrOfVoidPtr[] = {
    g_voidPtr, g_voidPtr, g_voidPtr
};

// kd> x targetapp!g_arrOfPtrToFunc
// xxxxxxxx`xxxxxxxx targetapp!g_arrOfPtrToFunc = <function> *[4]
WNDPROC g_arrOfPtrToFunc[] = {
    NULL, NULL, NULL, NULL
};

struct listStruct1 {
    int                     num;
    struct listStruct1     *next;
};


struct SomeBaseClassWithFields {
    int m_someBaseFiled1;
    int m_someBaseFiled2;
};

struct BaseEntryStruct {
    BaseEntryStruct *m_next;
};

struct ChildEntryTest : SomeBaseClassWithFields, BaseEntryStruct {
    int m_childFiled1;
};

BaseEntryStruct *g_childListHead = NULL;
ChildEntryTest g_childListEntry1;
ChildEntryTest g_childListEntry2;
ChildEntryTest g_childListEntry3;

class classWithDestructor
{
public:
    classWithDestructor(DWORD errCode) : m_errCode(errCode) {}
    virtual ~classWithDestructor() {::SetLastError(m_errCode);}

private:
    DWORD m_errCode;
};

listStruct1  g_listItem11 = { 100 };
listStruct1  g_listItem12 = { 200 };
listStruct1  g_listItem13 = { 300 };

struct {
    struct {
        int m_fieldNestedStruct;
    };
    int m_fieldOfUnNamed;
}g_unNamedStruct;

struct StructWithNested {
    struct Nested {
        int m_nestedFiled;
    };
    int m_field;

    struct {
        int m_field2;
    };
};

StructWithNested g_structWithNested;
StructWithNested::Nested g_structNested;

////////////////////////////////////////////////////////////////////////////////

struct baseStruct1
{
    int m_field;
};

struct intermediateStruct : baseStruct1
{
};

struct baseStruct2
{
    char m_field;
};

struct fieldSameNameStruct  : intermediateStruct
                            , baseStruct2
{
    char *m_field;
};
fieldSameNameStruct g_fieldSameNameStruct;

// kd> ?? g_fieldSameNameStruct
// struct fieldSameNameStruct
//    +0x000 m_field          : 0x400
//    +0x004 m_field          : 12 ''
//    +0x008 m_field          : 0x00000001`3f7bc928  "toaster"

// kd> dt fieldSameNameStruct @@C++(&g_fieldSameNameStruct)
// targetapp!fieldSameNameStruct
//    +0x000 m_field          : 0x400
//    +0x004 m_field          : 12 ''
//    +0x008 m_field          : 0x00000001`3f7bc928  "toaster"

// kd> dt fieldSameNameStruct
// targetapp!fieldSameNameStruct
//    +0x000 m_field          : Int4B
//    +0x004 m_field          : Char
//    +0x008 m_field          : Ptr64 Char

// kd> ?? g_fieldSameNameStruct.m_field
// char * 0x00000001`3f04c928
//  "toaster"
// kd> ?? g_fieldSameNameStruct.intermediateStruct::baseStruct1::m_field
// Type does not have given member error at 'intermediateStruct::baseStruct1::m_field'
// kd> g_fieldSameNameStruct.baseStruct2::m_field
// Type does not have given member error at 'baseStruct2::m_field'

////////////////////////////////////////////////////////////////////////////////

class VirtualBaseClass1 : public virtual classBase
{
    int     m_member;

public:
    VirtualBaseClass1() : m_member(123){}

    virtual void virtFunc() {}
    virtual void virtFunc2() {}
};

class VirtualBaseClass2 : public virtual classBase
{
    int     m_member;

public:
    VirtualBaseClass2() : m_member(345){}

    virtual void virtFunc() {}
    virtual void virtFunc2() {}
};

class VirtualChildClass : public VirtualBaseClass1, public  VirtualBaseClass2
{
    void virtFunc() {}
    void virtFunc2() {}

    void virtual virtFunc3() {}
};

VirtualChildClass       g_virtChild;

////////////////////////////////////////////////////////////////////////////////

char*  bigCStr = NULL;
wchar_t* bigWStr = NULL;

////////////////////////////////////////////////////////////////////////////////

WNDENUMPROC g_ptrToFunction;
void *g_unTypedPtrToFunction = g_ptrToFunction;
#pragma pack( pop )

typedef struct structTest       structTestTypeDef;

structTestTypeDef               g_structTypeDef = { 0 };

////////////////////////////////////////////////////////////////////////////////

#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead))

#define InsertTailList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
    }

////////////////////////////////////////////////////////////////////////////////

void FuncWithName0()
{
    classChild _classChild;
    _classChild.baseMethod();

    static volatile YetAnotherChild_class yetAnotherChild;
    std::cout << yetAnotherChild.m_i;

    reinterpret_cast<classChild *>(&_classChild)->virtFunc2();
    std::cout << _classChild.m_childField2;
    std::cout << g_constNumValue;
    std::cout << g_constBoolValue;
    std::cout << g_ucharValue;
    std::cout << g_ushortValue;
    std::cout << g_ulongValue;
    std::cout << g_ulonglongValue;
    std::cout << *g_pUlonglongValue;
    std::cout << g_charValue;
    std::cout << g_shortValue;
    std::cout << g_longValue;
    std::cout << g_longlongValue;

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
    std::cout << floatArray[2];
    std::cout << doubleArray[2];

    std::cout << intMatrix[1][1];
    std::cout << strArray[0];
    std::cout << (*ptrIntMatrix)[0][1];
    std::cout << g_struct3.m_noArrayField;
    std::cout << g_structWithBits.m_bit5;
    std::cout << ptrStrArray;
    std::cout << g_structTest1.m_field2;
    std::cout << ptrIntMatrix1;
    std::cout << g_bigValue;
    std::cout << g_classChild.m_enumField;
    std::cout << g_classChild.m_staticConst;
    std::cout << g_constEnumThree;
    std::cout << g_constUlong;
    std::cout << g_constUlonglong;
    std::cout << g_unNamedStruct.m_fieldNestedStruct;
    std::cout << g_structNested.m_nestedFiled;
    std::cout << g_unTypedPtrToFunction;

    std::cout << g_fieldSameNameStruct.m_field;
    std::cout << g_fieldSameNameStruct.intermediateStruct::baseStruct1::m_field;
    std::cout << g_fieldSameNameStruct.intermediateStruct::m_field;
    std::cout << g_fieldSameNameStruct.baseStruct2::m_field;

    std::cout << g_structTypeDef.m_field0;

    std::cout << g_nullSizeArray;
    std::cout << g_structAbstract;

    std::cout << g_float;
    std::cout << g_double;

    //std::cout << g_virtChild.VirtualBaseClass1::m_baseField;
}

////////////////////////////////////////////////////////////////////////////////

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
    std::cout << g_unNamedStruct.m_fieldOfUnNamed;
    std::cout << g_structWithNested.m_field;
    std::cout << g_ptrToFunction;

    std::cout << g_ptrToFunction;
    std::cout << g_arrOfPtrToFunc[1];
}

#pragma optimize("g", off)

static
void _FuncWithName2(int a)
{
    std::cout << a;
}

////////////////////////////////////////////////////////////////////////////////

VOID functionCalledFromEnumWindowsProc1(DWORD dwProcessId)
{
    DWORD dwCurrentProcessId = GetCurrentProcessId();
    if (dwCurrentProcessId != dwProcessId)
        std::cout << dwCurrentProcessId << dwProcessId;
    __debugbreak();
}

////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK EnumWindowsProc1(
    HWND hWindow,
    const LPARAM lParam
)
{
    DWORD dwProccessId = 0;
    if (hWindow)
        std::cout << lParam;

    if (hWindow)
    {
        static ULONGLONG staticVar = 0;
        DWORD dwThreadId = ::GetWindowThreadProcessId(hWindow, &dwProccessId);
        staticVar = dwProccessId + 1;
        __debugbreak();
        functionCalledFromEnumWindowsProc1(dwProccessId);
        std::cout << dwProccessId << dwThreadId << staticVar;
    }
    return hWindow ? FALSE : TRUE;
}

////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK EnumWindowsProc2(HWND, LPARAM)
{
    __debugbreak();
    return FALSE;
}

#pragma optimize("g", on)
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

int doAccessViolation()
{
    char *p = (char *)6;
    *p = 12;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

int g_valueForAccessTesting1 = 4;
int g_valueForAccessTesting2 = 5;

////////////////////////////////////////////////////////////////////////////////
#pragma optimize("g", off)
void changeValueForAccessTesting()
{
    g_valueForAccessTesting1 = 5;
}

////////////////////////////////////////////////////////////////////////////////

void readValueForAccessTesting()
{
    std::cout << g_valueForAccessTesting1 << g_valueForAccessTesting2;
}
#pragma optimize("g", on)
////////////////////////////////////////////////////////////////////////////////

int doExeptions()
{
    __debugbreak();

    changeValueForAccessTesting();

    readValueForAccessTesting();

    __debugbreak();

    PUCHAR _ptr = reinterpret_cast<UCHAR *>(2);
    __try {
        *_ptr = 5;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
    }

    ++_ptr;
    *_ptr = 6;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

int doChangeStatus()
{
    __debugbreak();
    __debugbreak();

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

#include <map>

std::map<int, bool> g_map;

int _tmain(int argc, _TCHAR* argv[])
{
    try
    {
        g_map.insert(std::pair<int, bool>(100, true));

        InitializeListHead( &g_listHead );
        InsertTailList( &g_listHead, &g_listItem1.listEntry );
        InsertTailList( &g_listHead, &g_listItem2.listEntry );
        InsertTailList( &g_listHead, &g_listItem3.listEntry );

        entry1.Flink = &entry2;
        entry2.Flink = &entry1;

        g_listHead1 = &g_listItem11;
        g_listItem11.next = &g_listItem12;
        g_listItem12.next = &g_listItem13;

        g_childListEntry1.m_someBaseFiled2 = 1000;
        g_childListEntry1.m_childFiled1    = 1001;

        g_childListEntry2.m_someBaseFiled2 = 2000;
        g_childListEntry2.m_childFiled1    = 2001;

        g_childListEntry3.m_someBaseFiled2 = 3000;
        g_childListEntry3.m_childFiled1    = 3001;

        g_childListHead = &g_childListEntry1;
        g_childListEntry1.m_next = &g_childListEntry2;
        g_childListEntry2.m_next = &g_childListEntry3;
        g_childListEntry3.m_next = NULL;

        g_unNamedStruct.m_fieldNestedStruct = 4;
        g_unNamedStruct.m_fieldOfUnNamed = 5;

        g_structWithNested.m_field = 34;
        g_structNested.m_nestedFiled = 46;

        g_fieldSameNameStruct.m_field = "toaster";
        g_fieldSameNameStruct.intermediateStruct::baseStruct1::m_field = 1024;
        g_fieldSameNameStruct.baseStruct2::m_field = 0xc;

        g_ptrToFunction = &EnumWindowsProc2;
        g_unTypedPtrToFunction = &EnumWindowsProc2;

        bigCStr = new char[0x2000 + 1];
        memset( bigCStr, 'a', 0x2000 );
        bigCStr[0x2000] = 0;

        bigWStr = new wchar_t[0x2000 + 1];
        wmemset( bigWStr, L'a', 0x2000 );
        bigWStr[0x2000] = 0;

        // Let test scripts to execute
        __debugbreak();

        if (2 == argc)
        {
            // run with parameters
            if ( !_tcsicmp(argv[1], _T("-testLoadUnload")) )
                return doLoadUnload();

            if ( !_tcsicmp(argv[1], _T("-testAccessViolation")) )
                return doAccessViolation();

            if ( !_tcsicmp(argv[1], _T("-testEnumWindows")) )
            {
                ::EnumWindows(&EnumWindowsProc1, 6);
                ::EnumWindows(&EnumWindowsProc2, 7);
                return ERROR_SUCCESS;
            }

            if ( !_tcsicmp(argv[1], _T("-testExceptions")) )
                return doExeptions();

            if ( !_tcsicmp(argv[1], _T("-testChangeStatus")) )
                return doChangeStatus();
        }

        __debugbreak();
        __debugbreak();
        __debugbreak();
        FuncWithName0();
        FuncWithName1(2);
        _FuncWithName2(3);
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

////////////////////////////////////////////////////////////////////////////////
