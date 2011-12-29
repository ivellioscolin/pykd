#pragma once

namespace pykd {

typedef boost::variant<LONG, ULONG, LONG64, ULONG64, bool>      BaseTypeVariant;

class VariantToStr : public boost::static_visitor<std::string>
{
public:
    template<typename T>
    std::string operator()(T i ) const {
        std::stringstream   sstr;
        sstr << i;
        return sstr.str();
    }
};

class VariantToHex : public boost::static_visitor<std::string>
{
public:
    template<typename T>
    std::string operator()(T i ) const {
        std::stringstream   sstr;
        sstr << std::hex <<  i;
        return sstr.str();
    }
};

class VariantToPyobj : public boost::static_visitor<python::object>
{
public:
    template<typename T>
    python::object operator()(T i ) const {
        return  python::object( i );
    }
};

class VariantToPylong : public boost::static_visitor<python::object>
{
public:
    template<typename T>
    python::object operator()(T i ) const {
        return  python::long_( i );
    }
};

class VariantToULong : public boost::static_visitor<ULONG>
{
public:
    template<typename T>
    ULONG operator()(T i ) const {
        return  static_cast<ULONG>( i );
    }
};


class intBase {

public:

    intBase(python::object &obj) {
        m_variant = convertToVar(obj);
    }

    intBase() : m_variant(LONG(0))
    {}

    std::string 
    str() {
        return boost::apply_visitor( VariantToStr(), getValue() );
    }
    
    std::string 
    hex() {
        return boost::apply_visitor( VariantToHex(), getValue() );
    }

    python::object
    long_() {
        return boost::apply_visitor( VariantToPylong(), getValue() );
    }

    python::object
    int_() {
        return boost::apply_visitor( VariantToPylong(), getValue() );
    }

    python::object eq( python::object&  obj ) {
        return boost::apply_visitor( VariantToPyobj(), getValue() ) == obj;
    }

    python::object ne( python::object&  obj ) {
        return  boost::apply_visitor( VariantToPyobj(), getValue() ) != obj;
    }

    python::object lt( python::object&  obj ) {
        return  boost::apply_visitor( VariantToPyobj(), getValue() ) < obj;
    }

    python::object gt( python::object&  obj ) {
        return  boost::apply_visitor( VariantToPyobj(), getValue() ) > obj;
    }

    python::object le( python::object&  obj ) {
        return  boost::apply_visitor( VariantToPyobj(), getValue() ) <= obj;
    }

    python::object ge( python::object&  obj ) {
        return  boost::apply_visitor( VariantToPyobj(), getValue() ) >= obj;
    }

    python::object add( python::object&  obj ) {
        return  boost::apply_visitor( VariantToPyobj(), getValue() ) + obj;
    }

    python::object sub( python::object&  obj ) {
        return  boost::apply_visitor( VariantToPyobj(), getValue() ) - obj;
    }

    python::object rsub( python::object&  obj ) {
        return  obj - boost::apply_visitor( VariantToPyobj(), getValue() );
    }

    python::object mul( python::object&  obj ) {
        return  boost::apply_visitor( VariantToPyobj(), getValue() ) * obj;
    }

    python::object div( python::object&  obj ) {
        return  boost::apply_visitor( VariantToPyobj(), getValue() ) / obj;
    }

    python::object rdiv( python::object&  obj ) {
        return  obj / boost::apply_visitor( VariantToPyobj(), getValue() );
    }

    python::object mod( python::object&  obj ) {
        return  boost::apply_visitor( VariantToPyobj(), getValue() ) % obj;
    }

    python::object rmod( python::object&  obj ) {
        return  obj % boost::apply_visitor( VariantToPyobj(), getValue() );
    }

    python::object rshift( python::object&  obj ) {
        return boost::apply_visitor( VariantToPyobj(), getValue() ) >> obj;
    }

    python::object rrshift( python::object&  obj ) {
        return obj >> boost::apply_visitor( VariantToPyobj(), getValue() );
    }

    python::object lshift( python::object&  obj ) {
        return boost::apply_visitor( VariantToPyobj(), getValue() ) << obj;
    }

    python::object rlshift( python::object&  obj ) {
        return obj << boost::apply_visitor( VariantToPyobj(), getValue() );
    }

    python::object and( python::object&  obj ) {
        return boost::apply_visitor( VariantToPyobj(), getValue() ) & obj;
    }

    python::object or( python::object&  obj ) {
        return boost::apply_visitor( VariantToPyobj(), getValue() ) | obj;
    }

    python::object xor( python::object&  obj ) {
        return boost::apply_visitor( VariantToPyobj(), getValue() ) ^ obj;
    }

    python::object neg() {
        return 0 - boost::apply_visitor( VariantToPyobj(), getValue() );
    }

    python::object pos() {
        return 0 + boost::apply_visitor( VariantToPyobj(), getValue() );
    }

    python::object invert() {
        return boost::apply_visitor( VariantToPyobj(), getValue() ) ^ boost::apply_visitor( VariantToPyobj(), getValue() );
    }

    python::object nonzero() {
        return boost::apply_visitor( VariantToPyobj(), getValue() ) != 0;
    }

private:

    virtual BaseTypeVariant getValue() {
        return m_variant;
    }

    BaseTypeVariant  convertToVar( python::object &obj )
    {

        if ( PyBool_Check( obj.ptr() ) )
        {
            if ( obj.ptr() == Py_True )
                return BaseTypeVariant(true);

            return BaseTypeVariant(false);
        }
        else if ( PyInt_CheckExact( obj.ptr() ) )
        {
             return BaseTypeVariant( LONG( PyLong_AsLong( obj.ptr() ) ) );
        }
        else
        {
            if ( _PyLong_Sign( obj.ptr() ) >= 0 )
                return BaseTypeVariant( ULONG64( PyLong_AsUnsignedLongLong( obj.ptr() ) ) );
            else
                return BaseTypeVariant( LONG64( PyLong_AsLongLong( obj.ptr() ) ) );
        }

        return BaseTypeVariant( false );
    }

    BaseTypeVariant     m_variant;
};

};