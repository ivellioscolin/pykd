#include "stdafx.h"

#include <string>

///////////////////////////////////////////////////////////////////////////////

// базовый типы

char                charVar = -100;
unsigned char       ucharVar = 200;

///////////////////////////////////////////////////////////////////////////////

struct Type1 {

    char                field1;

    unsigned char       field2;

    double              field3;
};

Type1     var1 = { -121, 220, 1.0095f };

///////////////////////////////////////////////////////////////////////////////


enum Enum1 {

    Enum1Val1 = 100,
    Enum1Val2 = 200,
    Enum1Val3 = 300
};

struct Type2 {

    Enum1           field1;

};


Type2       var2 = { Enum1Val1 };

///////////////////////////////////////////////////////////////////////////////

namespace Namespace1 {


    class Class1 {

    public:

        Class1( unsigned long v1 ) :
            m_field1( v1 )
            {}

    private:

        unsigned long       m_field1;

    };

    Class1      var3( 50 );

    namespace Namespace2  {

        class Class2 {

        public:

            Class2( const std::string &str ) :
              m_field1 ( str )
              {}

        private:

            std::string     m_field1;
        };

        Class2    var4( "hello" );

    };
};

///////////////////////////////////////////////////////////////////////////////

namespace Namespace3
{

    template<typename T>
    class Class3
    {
    public:
        Class3(T val): m_field1 (val)
        {
        }
    private:
        T m_field1;
    };
    
    Class3<int> var4(4);

};
///////////////////////////////////////////////////////////////////////////////