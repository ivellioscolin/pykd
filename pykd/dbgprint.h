// вывод информации в отладчик

#pragma once

#include <string>
#include <boost/python.hpp>
#include <boost/python/object.hpp>

class DbgPrint {

public:

	static void dprint( const boost::python::object& obj, bool dml = false );
	
	static void dprintln( const boost::python::object& obj, bool dml = false );
	

};

