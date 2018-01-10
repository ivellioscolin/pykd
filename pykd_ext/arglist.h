#pragma once

#include <vector>
#include <string>

typedef  std::vector< std::string >  ArgsList;

struct Options
{
    int  pyMajorVersion;
    int  pyMinorVersion;
    bool  global;
    bool  showHelp;
    bool  runModule;
    std::vector<std::string>  args;

    Options() :
        pyMajorVersion(-1),
        pyMinorVersion(-1),
        global(true),
        showHelp(false),
        runModule(false)
    {}

    Options(const std::string&  cmdline);
};
