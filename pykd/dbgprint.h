// вывод информации в отладчик

#pragma once

#include <string>

class DbgPrint {

public:

	static void dprint( const std::string&  str );
	
	static void dprintln( const std::string&  str );
};

