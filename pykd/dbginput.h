#pragma once

#include "dbgprint.h"

/////////////////////////////////////////////////////////////////////////////////

class dbgOut {

public:

    void
    write( const boost::python::object  &str ) {
        DbgPrint::dprint( str );
    }         
    
};

/////////////////////////////////////////////////////////////////////////////////

class dbgIn {

public:

    std::string
    readline() {
    
        char        str[100];
        ULONG       inputSize;
        
        OutputReader        outputReader( dbgExt->client );
    
        dbgExt->control->Input( str, sizeof(str), &inputSize );
    
        return std::string( str );
    }    

};

/////////////////////////////////////////////////////////////////////////////////

