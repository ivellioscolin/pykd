#pragma once


class intBase : boost::integer_arithmetic<intBase>
{

public:

    explicit intBase( ULONG64 val = 0 ) : m_value( val) {}
    
    virtual ~intBase() {}
    
    operator ULONG64() const {
        return value();
    }
    
    intBase& operator= ( ULONG64 val ) {
        setValue( val );    
        return *this;
    }
    
    virtual ULONG64  value() const {
        return m_value;
    }
    
    virtual void setValue( ULONG64  value) {
        m_value = value;
    }
    
    std::string 
    str() const {
        std::stringstream   ss;
        ss << value();
        return ss.str();
    }
    
    std::string 
    hex() const {
        std::stringstream   ss;
        ss << std::hex << value();
        return ss.str();
    }        
    
    template <class T>
    intBase& operator+=(T const& rhs)
    { m_value += rhs; return *this; }    
    
    template <class T>
    intBase& operator-=(T const& rhs)
    { m_value -= rhs; return *this; }       
    
    template <class T>
    intBase& operator*=(T const& rhs)
    { m_value *= rhs; return *this; }         
    
    template <class T>
    intBase& operator/=(T const& rhs)
    { m_value /= rhs; return *this; }      
        
    template <class T>
    intBase& operator%=(T const& rhs)
    { m_value %= rhs; return *this; }      
        
    template <class T>
    intBase& operator&=(T const& rhs)
    { m_value &= rhs; return *this; }      
    
    template <class T>
    intBase& operator|=(T const& rhs)
    { m_value |= rhs; return *this; }        
    
    template <class T>
    intBase& operator^=(T const& rhs)
    { m_value ^= rhs; return *this; }        
    
    template <class T>
    intBase& operator<<=(T const& rhs)
    { m_value <<= rhs; return *this; }   
    
    template <class T>
    intBase& operator>>=(T const& rhs)
    { m_value >>= rhs; return *this; }             
        
protected:

    mutable ULONG64     m_value;    

};