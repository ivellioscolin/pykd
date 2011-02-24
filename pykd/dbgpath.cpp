#include "stdafx.h"

#include <vector>

#include "dbgpath.h"

#include <boost/tokenizer.hpp>
#include <boost/python.hpp>

///////////////////////////////////////////////////////////////////////////////



DbgPythonPath::DbgPythonPath()
{
    DWORD   enviromentSize = 0;
    
    enviromentSize = GetEnvironmentVariableA( "PYTHONPATH", NULL, enviromentSize );

    std::vector<char> enviromentBuffer(enviromentSize);

	if (!enviromentBuffer.empty())
	{
		GetEnvironmentVariableA( "PYTHONPATH", &enviromentBuffer[0], enviromentSize );

		typedef  boost::escaped_list_separator<char>    char_separator_t;
		typedef  boost::tokenizer< char_separator_t >   char_tokenizer_t;  
	    
		std::string     pytonPath( &enviromentBuffer[0] );
	  
		char_tokenizer_t            token( pytonPath, char_separator_t( "", "; \t", "\"" ) );

		for ( char_tokenizer_t::iterator   it = token.begin(); it != token.end(); ++it )
		{
			if ( *it != "" )
				m_pathList.push_back( *it );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

std::string 
DbgPythonPath::getStr() const
{
    std::string     str;
    std::vector<std::string>::const_iterator      it = m_pathList.begin();
    
    for ( ; it != m_pathList.end(); ++it )
    {
        str += *it;
        str += ";";
    }
    
    return str;
}
 
///////////////////////////////////////////////////////////////////////////////
 
bool
DbgPythonPath::findPath( 
    const std::string  &fileName,
    std::string  &fullFileName,
    std::string  &filePath ) const
{
    std::vector< std::string >      extPathList;

    boost::python::object       sys = boost::python::import( "sys");

    boost::python::list       pathList( sys.attr("path") );
    
    boost::python::ssize_t n = boost::python::len(pathList);
    for(boost::python::ssize_t i=0;i<n;i++) 
         extPathList.push_back(  boost::python::extract<std::string>( pathList[i] ) );

    bool    pyExt = fileName.rfind( ".py" ) ==  fileName.length() - 3; 

    // 1. »щем в рабочей директории
    DWORD   bufSize =
        SearchPathA(
            NULL,
            fileName.c_str(),
            pyExt ? NULL : ".py",
            0,
            NULL,
            NULL );  
            
    if ( bufSize > 0 )    
    {
		std::vector<char> fullFileNameCStr(bufSize);
        char    *partFileNameCStr = NULL;

        SearchPathA(
            NULL,
            fileName.c_str(),
            pyExt ? NULL : ".py",
            bufSize,
            &fullFileNameCStr[0],
            &partFileNameCStr );              

        fullFileName = std::string( &fullFileNameCStr[0] );
        filePath = std::string( &fullFileNameCStr[0], partFileNameCStr );

        return true;        
    }                
            
    // 2. »щем во всех директори€х, указанных в m_pathList
    
    std::vector<std::string>::const_iterator      it = extPathList.begin();
    
    for ( ; it != extPathList.end(); ++it )
    {
        DWORD   bufSize =
            SearchPathA(
                (*it).c_str(),
                fileName.c_str(),
                 pyExt ? NULL : ".py",
                0,
                NULL,
                NULL );          
                
        if ( bufSize > 0 )
        {
			std::vector<char> fullFileNameCStr(bufSize);
            char    *partFileNameCStr = NULL;
            
            SearchPathA(
                (*it).c_str(),
                fileName.c_str(),
                pyExt ? NULL : ".py",
                bufSize,
                &fullFileNameCStr[0],
                &partFileNameCStr );              
            
            fullFileName = std::string( &fullFileNameCStr[0] );
            filePath = std::string( &fullFileNameCStr[0], partFileNameCStr );
            
            return true;
        }                  
    }
    
    return false;
}
    
//////////////////////////////////////////////////////////////////////////////    
