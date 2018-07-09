#pragma once

#include <comutil.h>

#include "kdlib/variant.h"
#include "kdlib/exceptions.h"
#include "dbgexcept.h"
#include "pythreadstate.h"

namespace pykd {

class NumVariantAdaptor : public kdlib::NumConvertable
{

public:

   static kdlib::NumVariant NumVariantAdaptor::convertToVariant( const python::object &obj)
   {
        kdlib::NumVariant   var;

        python::extract<kdlib::NumConvertable>  getNumVar(obj);
        if ( getNumVar.check() )
        {
            var = getNumVar();
            return var;
        }

        if (PyBool_Check(obj.ptr()))
        {
            if (obj.ptr() == Py_True)
                var.setBool(true);
            else
                var.setBool(false);
            return var;
        }

        if (PyFloat_Check(obj.ptr()))
        {
            var.setDouble(PyFloat_AsDouble(obj.ptr()));
            return var;
        }

#if PY_VERSION_HEX < 0x03000000

        if (PyInt_CheckExact(obj.ptr()))
        {
            var.setLong(PyLong_AsLong(obj.ptr()));
            return var;
        }
#endif

        if (_PyLong_Sign(obj.ptr()) >= 0)
        {
           if (_PyLong_NumBits(obj.ptr()) > 64)
               throw pykd::OverflowException("int too big to convert");

            var.setULongLong(PyLong_AsUnsignedLongLong(obj.ptr()));
        }
        else
        {
            if (_PyLong_NumBits(obj.ptr()) > 63)
                throw pykd::OverflowException("int too big to convert");

            var.setLongLong(PyLong_AsLongLong(obj.ptr()));
        }


        return var;
   }

   static python::object NumVariantAdaptor::convertToPython( const kdlib::NumVariant& var )
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
            return python::object( var.asInt() );

        if ( var.isDouble() )
            return python::object( var.asInt() );

        return python::object( var.asInt() );
   }


    //static kdlib::NumConvertable* NumVariantAdaptor::getVariant(const python::object &obj)
    //{
    //    //NumVariantAdaptor*  var = new NumVariantAdaptor();

    //    //var->m_variant = NumVariantAdaptor::convertToVariant(obj);

    //    //return var;
    //}

   static python::object NumVariantAdaptor::convertToPython( kdlib::NumConvertable& num )
   {
        kdlib::NumVariant var;
        
        {
            AutoRestorePyState  pystate;
            var = kdlib::NumVariant( num );
        }

        return convertToPython( var );
   }

public:

    static python::object eq( kdlib::NumConvertable& var, python::object&  obj )
    {
        try {
            return convertToPython(var) == obj;
        } 
        catch( kdlib::DbgException& )
        {}

        return python::object(false);
    }

    static python::object ne( kdlib::NumConvertable& var, python::object&  obj )
    {
        try {
            return convertToPython(var) != obj;
        } 
        catch( kdlib::DbgException& )
        {}

        return python::object(true);
    }

    static python::object lt( kdlib::NumConvertable& var, python::object&  obj ) {
        return convertToPython(var) < obj;
    }

    static python::object gt( kdlib::NumConvertable& var, python::object&  obj ) {
        return convertToPython(var) > obj;
    }

    static python::object le( kdlib::NumConvertable& var, python::object&  obj ) {
        return convertToPython(var) <= obj;
    }

    static python::object ge( kdlib::NumConvertable& var, python::object&  obj ) {
        return convertToPython(var) >= obj;
    }

    static python::object add( kdlib::NumConvertable& var, python::object&  obj ) {
        return convertToPython(var) + obj;
    }

    static python::object sub( kdlib::NumConvertable& var, python::object&  obj ) {
        return convertToPython(var) - obj;
    }

    static python::object rsub( kdlib::NumConvertable& var, python::object&  obj ) {
        return obj - convertToPython(var);
    }

    static python::object mul( kdlib::NumConvertable& var, python::object&  obj ) {
        return convertToPython(var) * obj;
    }

    static python::object div( kdlib::NumConvertable& var, python::object&  obj ) {
        return convertToPython(var) / obj;
    }

    static python::object truediv(kdlib::NumConvertable& var, python::object&  obj) {
        return convertToPython(var) / obj;
    }

    static python::object rtruediv(kdlib::NumConvertable& var, python::object&  obj) {
        return obj / convertToPython(var);
    }

    static python::object rdiv( kdlib::NumConvertable& var, python::object&  obj ) {
        return  obj / convertToPython(var);
    }

    static python::object mod( kdlib::NumConvertable& var, python::object&  obj ) {
        return convertToPython(var) % obj;
    }

    static python::object rmod( kdlib::NumConvertable& var, python::object&  obj ) {
        return  obj % convertToPython(var);
    }

    static python::object rshift( kdlib::NumConvertable& var, python::object&  obj ) {
        return convertToPython(var) >> obj;
    }

    static python::object rrshift( kdlib::NumConvertable& var, python::object&  obj ) {
        return obj >> convertToPython(var);
    }

    static python::object lshift( kdlib::NumConvertable& var, python::object&  obj ) {
        return convertToPython(var) << obj;
    }

    static python::object rlshift( kdlib::NumConvertable& var, python::object&  obj ) {
        return obj << convertToPython(var);
    }

    static python::object and( kdlib::NumConvertable& var, python::object&  obj ) {
        return convertToPython(var) & obj;
    }

    static python::object or( kdlib::NumConvertable& var, python::object&  obj ) {
        return convertToPython(var) | obj;
    }

    static python::object xor( kdlib::NumConvertable& var, python::object&  obj ) {
        return convertToPython(var) ^ obj;
    }

    static python::object neg(kdlib::NumConvertable& var) {
        return 0 - convertToPython(var);
    }

    static  python::object pos(kdlib::NumConvertable& var) {
        return 0 + convertToPython(var);
    }

   static  python::object invert(kdlib::NumConvertable& var) {
        return convertToPython(var) ^ convertToPython(var);
    }

    static python::object nonzero(kdlib::NumConvertable& var) {
        try {
            return convertToPython(var) != 0;
        } 
        catch( kdlib::DbgException& )
        {}

        return python::object(true);
    }

    static python::object long_(kdlib::NumConvertable& var ) {
        return convertToPython(var);
    }

    static python::object float_(kdlib::NumConvertable& var) {
        kdlib::NumVariant  v = var;
        return python::object(v.asDouble());
    }

    static python::object int_(kdlib::NumConvertable& var) {
        return convertToPython(var);
    }

    static std::wstring str(kdlib::NumConvertable& var) {
            kdlib::NumVariant  v = var;
            return v.asStr();
    }

    static std::string hex(kdlib::NumConvertable& var) {
            kdlib::NumVariant  v = var;
            return std::string("0x") + std::string(_bstr_t(v.asHex().c_str()));
    }

    static bool isInteger(kdlib::NumConvertable& var) {
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
        python::extract<kdlib::NumConvertable> getNumVar(obj_ptr);

        if (getNumVar.check())
            return obj_ptr;
        else
            return 0;
    }

    template<typename T>
    static void numConstruct( PyObject* obj_ptr, python::converter::rvalue_from_python_stage1_data* data)
    {
        void* storage = ( (python::converter::rvalue_from_python_storage<T>*)data)->storage.bytes;
 
        kdlib::NumConvertable& num = python::extract<kdlib::NumConvertable&>(obj_ptr);

        kdlib::NumVariant   var = num;
        
        new (storage ) T( static_cast<T>(var.asULongLong() ) );

        data->convertible = storage;
    }
};

} // end pykf namespace
