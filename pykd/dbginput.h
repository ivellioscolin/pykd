#pragma once

#include "dbgprint.h"

/////////////////////////////////////////////////////////////////////////////////

class dbgOut {

public:

    void
    write( const std::string&  str ) {
        DbgPrint::dprint( str );
    }    
    
private:
   
    
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
    
private:
   

};

/////////////////////////////////////////////////////////////////////////////////

