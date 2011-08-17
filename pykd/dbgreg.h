#pragma once

#include <string>
#include <list>

#include "intbase.h"

/////////////////////////////////////////////////////////////////////////////////

class cpuReg : public intBase {

public :

    cpuReg( std::string  regName );

    cpuReg( ULONG index );
    
    std::string
    name() const {
        return m_name;
    }

    ULONG
    index() const {
        return m_index;            
    }
    
    void beLive() {
        m_lived = true;
    }
    
    ULONG64  value() const 
    {
        if ( m_lived )
            reloadValue();
                
        return intBase::value();
    }    
    
private:

    void  reloadValue() const;

    std::string     m_name;

    ULONG           m_index;
    
    bool            m_lived;
};

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadRegister( const std::string &registerName );

ULONG64
loadMSR( ULONG  msr );

void setMSR( ULONG msr, ULONG64 val);

/////////////////////////////////////////////////////////////////////////////////