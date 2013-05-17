#pragma once

#include "kdlib/variant.h"

namespace pykd {

class NumVariant : public kdlib::NumVariant
{

public:

    NumVariant( const python::object &obj )
    {
        if ( PyBool_Check( obj.ptr() ) )
        {
            if ( obj.ptr() == Py_True )
               setBool(true);
            else
               setBool(false);
            return;
        }

        
        if ( PyInt_CheckExact( obj.ptr() ) )
        {
             setLong( PyLong_AsLong( obj.ptr() ) );
             return;
        }

        if ( _PyLong_Sign( obj.ptr() ) >= 0 )
            setULongLong( PyLong_AsUnsignedLongLong( obj.ptr() ) );
        else
            setLongLong( PyLong_AsLongLong( obj.ptr() ) );
    }

   python::object convertToPython()
   {
        if ( isChar() )
            return python::object( asInt() );
        
        if ( isUChar() )
            return python::object( asInt() );

        if (isShort() )
            return python::object( asInt() );

        if ( isUShort() )
            return python::object( asInt() );
    
        if ( isLong() )
            return python::object( asLong() );

        if ( isULong() )
            return python::object( asULong() );

        if ( isLongLong() )
            return python::object( asLongLong() );

        if ( isULongLong() )
            return python::object( asULongLong() );

        if ( isInt() )
            return python::object( asInt() );

        if ( isUInt() )
            return python::object( asUInt() );

        if ( isFloat() )
            return python::object( asFloat() );

        if ( isDouble() )
            return python::object( asDouble() );
   }


public:

    python::object eq( python::object&  obj ) {
        return convertToPython() == obj;
    }

    python::object ne( python::object&  obj ) {
        return convertToPython() != obj;
    }

    python::object lt( python::object&  obj ) {
        return convertToPython()  < obj;
    }

    python::object gt( python::object&  obj ) {
        return convertToPython() > obj;
    }

    python::object le( python::object&  obj ) {
        return convertToPython() <= obj;
    }

    python::object ge( python::object&  obj ) {
        return convertToPython() >= obj;
    }

    python::object add( python::object&  obj ) {
        return convertToPython() + obj;
    }

    python::object sub( python::object&  obj ) {
        return convertToPython() - obj;
    }

    python::object rsub( python::object&  obj ) {
        return obj - convertToPython();
    }

    python::object mul( python::object&  obj ) {
        return convertToPython() * obj;
    }

    python::object div( python::object&  obj ) {
        return convertToPython() / obj;
    }

    python::object rdiv( python::object&  obj ) {
        return  obj / convertToPython();
    }

    python::object mod( python::object&  obj ) {
        return convertToPython() % obj;
    }

    python::object rmod( python::object&  obj ) {
        return  obj % convertToPython();
    }

    python::object rshift( python::object&  obj ) {
        return convertToPython() >> obj;
    }

    python::object rrshift( python::object&  obj ) {
        return obj >> convertToPython();
    }

    python::object lshift( python::object&  obj ) {
        return convertToPython() << obj;
    }

    python::object rlshift( python::object&  obj ) {
        return obj << convertToPython();
    }

    python::object and( python::object&  obj ) {
        return convertToPython() & obj;
    }

    python::object or( python::object&  obj ) {
        return convertToPython() | obj;
    }

    python::object xor( python::object&  obj ) {
        return convertToPython() ^ obj;
    }

    python::object neg() {
        return 0 - convertToPython();
    }

    python::object pos() {
        return 0 + convertToPython();
    }

    python::object invert() {
        return convertToPython() ^ convertToPython();
    }

    python::object nonzero() {
        return convertToPython() != 0;
    }

public:

    python::object long_() {
        return python::long_( asLongLong() );
    }

    python::object int_() {
        return python::long_( asLongLong() );
    }

    operator unsigned long long() {
        return asULongLong();
    }

    operator unsigned long() {
        return asULong();
    }

    operator long long() {
        return asLongLong();
    }

    operator long() {
        return asLong();
    }

};

} // end pykf namespace
