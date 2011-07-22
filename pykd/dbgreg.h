#pragma once

#include <string>
#include <list>

#include "intbase.h"

/////////////////////////////////////////////////////////////////////////////////

class cpuReg : public intBase {

public :

    cpuReg( std::string  regName );
    
    std::string
    name() const {
        return m_name;
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
    
    bool            m_lived;
};

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadRegister( const std::string &registerName );

ULONG64
loadMSR( ULONG  msr );

void setMSR( ULONG msr, ULONG64 val);

/////////////////////////////////////////////////////////////////////////////////