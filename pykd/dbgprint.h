// вывод информации в отладчик

#pragma once

#include <string>

class DbgPrint {

public:

	static void dprint( const boost::python::object& obj, bool dml = false );
	
	static void dprintln( const boost::python::object& obj, bool dml = false );
	

};

