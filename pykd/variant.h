#pragma once

#include "kdlib/variant.h"

namespace pykd {

class NumVariant : public kdlib::NumVariantGetter
{

public:

    static kdlib::NumVariantGetter*  getVariant(  const python::object &obj )
    {
        NumVariant*  var = new NumVariant();

        if ( PyBool_Check( obj.ptr() ) )
        {
            if ( obj.ptr() == Py_True )
               var->m_variant.setBool(true);
            else
               var->m_variant.setBool(false);
            return var;
        }
        
        if ( PyInt_CheckExact( obj.ptr() ) )
        {
             var->m_variant.setLong( PyLong_AsLong( obj.ptr() ) );
             return var;
        }

        if ( _PyLong_Sign( obj.ptr() ) >= 0 )
            var->m_variant.setULongLong( PyLong_AsUnsignedLongLong( obj.ptr() ) );
        else
           var->m_variant.setLongLong( PyLong_AsLongLong( obj.ptr() ) );

        return var;
    }

   static python::object convertToPython( kdlib::NumVariantGetter& v )
   {
        kdlib::NumVariant var = v;

        if ( var.isChar() )
            return python::object( var.asInt() );
        
        if ( var.isUChar() )
            return python::object( var.asInt() );

        if ( var.isShort() )
            return python::object( var.asInt() );

        if ( var.isUShort() )
            return python::object( var.asInt() );
    
        if ( var.isLong() )
            return python::object( var.asLong() );

        if ( var.isULong() )
            return python::object( var.asULong() );

        if ( var.isLongLong() )
            return python::object( var.asLongLong() );

        if ( var.isULongLong() )
            return python::object( var.asULongLong() );

        if ( var.isInt() )
            return python::object( var.asInt() );

        if ( var.isUInt() )
            return python::object( var.asUInt() );

        if ( var.isFloat() )
            return python::object( var.asFloat() );

        if ( var.isDouble() )
            return python::object( var.asDouble() );

        return python::object( var.asInt() );
   }

public:

    static python::object eq( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) == obj;
    }

    static python::object ne( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) != obj;
    }

    static python::object lt( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) < obj;
    }

    static python::object gt( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) > obj;
    }

    static python::object le( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) <= obj;
    }

    static python::object ge( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) >= obj;
    }

    static python::object add( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) + obj;
    }

    static python::object sub( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) - obj;
    }

    static python::object rsub( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return obj - convertToPython(var);
    }

    static python::object mul( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) * obj;
    }

    static python::object div( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) / obj;
    }

    static python::object rdiv( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return  obj / convertToPython(var);
    }

    static python::object mod( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) % obj;
    }

    static python::object rmod( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return  obj % convertToPython(var);
    }

    static python::object rshift( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) >> obj;
    }

    static python::object rrshift( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return obj >> convertToPython(var);
    }

    static python::object lshift( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) << obj;
    }

    static python::object rlshift( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return obj << convertToPython(var);
    }

    static python::object and( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) & obj;
    }

    static python::object or( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) | obj;
    }

    static python::object xor( kdlib::NumVariantGetter& var, python::object&  obj ) {
        return convertToPython(var) ^ obj;
    }

    static python::object neg(kdlib::NumVariantGetter& var) {
        return 0 - convertToPython(var);
    }

    static  python::object pos(kdlib::NumVariantGetter& var) {
        return 0 + convertToPython(var);
    }

   static  python::object invert(kdlib::NumVariantGetter& var) {
        return convertToPython(var) ^ convertToPython(var);
    }

    static python::object nonzero(kdlib::NumVariantGetter& var) {
        return convertToPython(var) != 0;
    }

    static python::object long_(kdlib::NumVariantGetter& var ) {
        return convertToPython(var);
    }

    static python::object int_(kdlib::NumVariantGetter& var) {
        return convertToPython(var);
    }


private:

    virtual kdlib::NumVariant getValue()  const{
        return m_variant;
    }

     kdlib::NumVariant  m_variant;
};

} // end pykf namespace
