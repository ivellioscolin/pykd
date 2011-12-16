#pragma once

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

class DbgPythonPath
{
public:
    DbgPythonPath();

    bool getFullFileName(const std::string &fileName, std::string &fullFileName) const;

private:
    std::vector<std::string> m_extactedPathList;
};

///////////////////////////////////////////////////////////////////////////////