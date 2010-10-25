#pragma once

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

class DbgPythonPath
{
public:

    DbgPythonPath();

    std::string 
    getStr() const;
    
    bool
    findPath( 
        const std::string  &fileName,
        std::string  &fullFileName,
        std::string  &filePath ) const;
    
    
private:

    std::vector< std::string >      m_pathList;    

};

extern DbgPythonPath&   dbgPythonPath;


///////////////////////////////////////////////////////////////////////////////