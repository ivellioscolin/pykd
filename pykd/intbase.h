#pragma once

namespace pykd {

class intBase : boost::integer_arithmetic<intBase>
{

public:
    
    operator ULONG64() const {
        return getValue();
    }
    
    intBase& operator= ( ULONG64 val ) {
        setValue( val );    
        return *this;
    }
    
    virtual ULONG64  getValue() const {
        return m_intValue;
    }
    
    virtual void setValue( ULONG64  value ) {
        m_intValue = value;
    }
    
    std::string 
    str() const {
        std::stringstream   ss;
        ss << getValue();
        return ss.str();
    }
    
    std::string 
    hex() const {
        std::stringstream   ss;
        ss << std::hex << getValue();
        return ss.str();
    }        
    
    template <class T>
    intBase& operator+=(T const& rhs)
    { setValue( getValue() + rhs ); return *this; }    
    
    template <class T>
    intBase& operator-=(T const& rhs)
    { setValue( getValue() - rhs ); return *this; }  
    
    template <class T>
    intBase& operator*=(T const& rhs)
    { setValue( getValue() * rhs ); return *this; }        
    
    template <class T>
    intBase& operator/=(T const& rhs)
    { setValue( getValue() / rhs ); return *this; } 
        
    template <class T>
    intBase& operator%=(T const& rhs)
    { setValue( getValue() % rhs ); return *this; }      
        
    template <class T>
    intBase& operator&=(T const& rhs)
    { setValue( getValue() & rhs ); return *this; }   
    
    template <class T>
    intBase& operator|=(T const& rhs)
    { setValue( getValue() | rhs ); return *this; }         
    
    template <class T>
    intBase& operator^=(T const& rhs)
    { setValue( getValue() ^ rhs ); return *this; }        
    
    template <class T>
    intBase& operator<<=(T const& rhs)
    { setValue( getValue() << rhs ); return *this; }    
    
    template <class T>
    intBase& operator>>=(T const& rhs)
    { setValue( getValue() >> rhs ); return *this; }  

private:

    ULONG64     m_intValue;

};

};