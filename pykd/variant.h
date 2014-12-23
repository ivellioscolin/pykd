#pragma once

#include <comutil.h>

#include "kdlib/variant.h"
#include "kdlib/exceptions.h"

namespace pykd {

class NumVariantAdaptor : public kdlib::NumBehavior
{

public:


    static kdlib::NumBehavior* NumVariantAdaptor::getVariant( const python::object &obj )
    {
        NumVariantAdaptor*  var = new NumVariantAdaptor();

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

   static python::object NumVariantAdaptor::convertToPython( kdlib::NumVariant& var )
   {
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

   static python::object NumVariantAdaptor::convertToPython( kdlib::NumBehavior& num )
   {
        kdlib::NumVariant var = kdlib::NumVariant( num );

        return convertToPython( var );
   }

public:

    static python::object eq( kdlib::NumBehavior& var, python::object&  obj )
    {
        try {
            return convertToPython(var) == obj;
        } 
        catch( kdlib::DbgException& )
        {}

        return python::object(false);
    }

    static python::object ne( kdlib::NumBehavior& var, python::object&  obj ) 
    {
        try {
            return convertToPython(var) != obj;
        } 
        catch( kdlib::DbgException& )
        {}

        return python::object(true);
    }

    static python::object lt( kdlib::NumBehavior& var, python::object&  obj ) {
        return convertToPython(var) < obj;
    }

    static python::object gt( kdlib::NumBehavior& var, python::object&  obj ) {
        return convertToPython(var) > obj;
    }

    static python::object le( kdlib::NumBehavior& var, python::object&  obj ) {
        return convertToPython(var) <= obj;
    }

    static python::object ge( kdlib::NumBehavior& var, python::object&  obj ) {
        return convertToPython(var) >= obj;
    }

    static python::object add( kdlib::NumBehavior& var, python::object&  obj ) {
        return convertToPython(var) + obj;
    }

    static python::object sub( kdlib::NumBehavior& var, python::object&  obj ) {
        return convertToPython(var) - obj;
    }

    static python::object rsub( kdlib::NumBehavior& var, python::object&  obj ) {
        return obj - convertToPython(var);
    }

    static python::object mul( kdlib::NumBehavior& var, python::object&  obj ) {
        return convertToPython(var) * obj;
    }

    static python::object div( kdlib::NumBehavior& var, python::object&  obj ) {
        return convertToPython(var) / obj;
    }

    static python::object rdiv( kdlib::NumBehavior& var, python::object&  obj ) {
        return  obj / convertToPython(var);
    }

    static python::object mod( kdlib::NumBehavior& var, python::object&  obj ) {
        return convertToPython(var) % obj;
    }

    static python::object rmod( kdlib::NumBehavior& var, python::object&  obj ) {
        return  obj % convertToPython(var);
    }

    static python::object rshift( kdlib::NumBehavior& var, python::object&  obj ) {
        return convertToPython(var) >> obj;
    }

    static python::object rrshift( kdlib::NumBehavior& var, python::object&  obj ) {
        return obj >> convertToPython(var);
    }

    static python::object lshift( kdlib::NumBehavior& var, python::object&  obj ) {
        return convertToPython(var) << obj;
    }

    static python::object rlshift( kdlib::NumBehavior& var, python::object&  obj ) {
        return obj << convertToPython(var);
    }

    static python::object and( kdlib::NumBehavior& var, python::object&  obj ) {
        return convertToPython(var) & obj;
    }

    static python::object or( kdlib::NumBehavior& var, python::object&  obj ) {
        return convertToPython(var) | obj;
    }

    static python::object xor( kdlib::NumBehavior& var, python::object&  obj ) {
        return convertToPython(var) ^ obj;
    }

    static python::object neg(kdlib::NumBehavior& var) {
        return 0 - convertToPython(var);
    }

    static  python::object pos(kdlib::NumBehavior& var) {
        return 0 + convertToPython(var);
    }

   static  python::object invert(kdlib::NumBehavior& var) {
        return convertToPython(var) ^ convertToPython(var);
    }

    static python::object nonzero(kdlib::NumBehavior& var) {
        try {
            return convertToPython(var) != 0;
        } 
        catch( kdlib::DbgException& )
        {}

        return python::object(true);
    }

    static python::object long_(kdlib::NumBehavior& var ) {
        return convertToPython(var);
    }

    static python::object int_(kdlib::NumBehavior& var) {
        return convertToPython(var);
    }

    static std::wstring str(kdlib::NumBehavior& var) { 
            kdlib::NumVariant  v = var;
            return v.asStr();
    }

    static std::string hex(kdlib::NumBehavior& var) { 
            kdlib::NumVariant  v = var;
            return std::string("0x") + std::string(_bstr_t(v.asHex().c_str()));
    }

    static bool isInteger(kdlib::NumBehavior& var) { 
        kdlib::NumVariant  v = var;
        return v.isInteger();
    }

    static void registerNumConvertion()  {
        python::converter::registry::push_back( &numConvertible, &numConstruct<long>, python::type_id<long>() );
        python::converter::registry::push_back( &numConvertible, &numConstruct<unsigned long>, python::type_id<unsigned long>() );
        python::converter::registry::push_back( &numConvertible, &numConstruct<long long>, python::type_id<long long>() );
        python::converter::registry::push_back( &numConvertible, &numConstruct<unsigned long long>, python::type_id<unsigned long long>() );
    }

private:

    virtual kdlib::NumVariant getValue()  const{
        return m_variant;
    }

    kdlib::NumVariant  m_variant;

    static void* numConvertible( PyObject* obj_ptr)
    {
        python::extract<kdlib::NumBehavior> getNumVar(obj_ptr);

        if (getNumVar.check())
            return obj_ptr;
        else
            return 0;
    }

    template<typename T>
    static void numConstruct( PyObject* obj_ptr, python::converter::rvalue_from_python_stage1_data* data)
    {
        void* storage = ( (python::converter::rvalue_from_python_storage<T>*)data)->storage.bytes;
 
        kdlib::NumBehavior* num = python::extract<kdlib::NumBehavior*>(obj_ptr);

        kdlib::NumVariant   var = *num;
        
        new (storage ) T( static_cast<T>(var.asULongLong() ) );

        data->convertible = storage;
    }
};

} // end pykf namespace
