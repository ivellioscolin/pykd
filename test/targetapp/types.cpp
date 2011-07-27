#include "stdafx.h"

///////////////////////////////////////////////////////////////////////////////

// базовый типы

char                charVar = -100;
unsigned char       ucharVar = 200;

///////////////////////////////////////////////////////////////////////////////

enum Enum1 {

    Enum1Val1 = 100,
    Enum1Val2 = 200,
    Enum1Val3 = 300
};


struct Type1 {

    char                field1;

    unsigned char       field2;

};

Type1     var1 = { -121, 220 };


struct Type2 {

    Enum1           field1;

};

Type2       var2 = { Enum1Val1 };

///////////////////////////////////////////////////////////////////////////////