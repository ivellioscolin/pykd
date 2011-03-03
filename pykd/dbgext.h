#pragma once

#include <dbgeng.h>
#include <dbghelp.h>

struct DbgExt {

    IDebugClient            *client;
    IDebugClient4           *client4;
    
    IDebugControl           *control;
    IDebugControl4          *control4;
    
    IDebugRegisters         *registers;
    
    IDebugSymbols           *symbols;
    IDebugSymbols2          *symbols2;
    IDebugSymbols3          *symbols3;
    
    IDebugDataSpaces        *dataSpaces;
    IDebugDataSpaces4       *dataSpaces4;
    
    IDebugAdvanced2         *advanced2;
    
    IDebugSystemObjects     *system;
    IDebugSystemObjects2    *system2;
    
    DbgExt() :
        client( NULL ),
        client4( NULL ),
        control( NULL ),
        control4( NULL ),
        registers( NULL ),
        symbols( NULL ),
        symbols2( NULL ),
        symbols3( NULL ),
        dataSpaces( NULL ),
        dataSpaces4( NULL ),
        advanced2( NULL ),
        system( NULL ),
        system2( NULL )
        {}
            
    ~DbgExt();
};

extern DbgExt    *dbgExt;

void
SetupDebugEngine( IDebugClient4 *client, DbgExt *dbgExt  );

