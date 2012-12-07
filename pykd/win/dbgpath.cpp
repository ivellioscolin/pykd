#include "stdafx.h"

#include "dbgpath.h"
#include "utils.h"

#include <boost/tokenizer.hpp>

///////////////////////////////////////////////////////////////////////////////

DbgPythonPath::DbgPythonPath()
{
    boost::python::object sys = boost::python::import("sys");

    boost::python::list pathList(sys.attr("path"));

    boost::python::ssize_t n = boost::python::len(pathList);

    for (boost::python::ssize_t i = 0; i < n ; i++) 
        m_extactedPathList.push_back(boost::python::extract<std::string>(pathList[i]));
}

///////////////////////////////////////////////////////////////////////////////
 
bool DbgPythonPath::getFullFileName(const std::string &fileName, std::string &fullFileName) const
{
    bool fileHasPyExt = false;

    if ( fileName.length() > 3 )
        fileHasPyExt = fileName.rfind(".py") == fileName.length() - 3;
    
    fullFileName = fileName;

    if (!fileHasPyExt)
        fullFileName += ".py";
    
    if (FileExists(fullFileName.c_str()))
        return true;

    std::vector<std::string>::const_iterator it = m_extactedPathList.begin();
    for ( ; it != m_extactedPathList.end(); ++it)
    {
        DWORD bufSize = SearchPathA(
            (*it).c_str(), 
            fullFileName.c_str(), 
            NULL, 
            0, 
            NULL, 
            NULL);          
                
        if (bufSize > 0)
        {
            bufSize += 1;
            std::vector<char> fullFileNameCStr(bufSize);
            char *partFileNameCStr = NULL;
            
            bufSize = SearchPathA(
                (*it).c_str(),
                fullFileName.c_str(),
                NULL,
                bufSize,
                &fullFileNameCStr[0],
                &partFileNameCStr);
            
            if (bufSize > 0)
            {
                fullFileName = std::string(&fullFileNameCStr[0]);
                return true;
            }
        }                  
    }

    fullFileName = "";
    return false;
}
    
//////////////////////////////////////////////////////////////////////////////    
