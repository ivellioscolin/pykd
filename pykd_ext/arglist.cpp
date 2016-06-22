#include "stdafx.h"

#include <boost/tokenizer.hpp>
#include <regex>

#include "arglist.h"

typedef  boost::escaped_list_separator<char>    char_separator_t;
typedef  boost::tokenizer< char_separator_t >   char_tokenizer_t;


ArgsList  getArgsList(const char* args)
{
    std::string  argsStr(args);

    char_tokenizer_t  token(argsStr, char_separator_t("", " \t", "\""));
    ArgsList  argsList;

    for (char_tokenizer_t::iterator it = token.begin(); it != token.end(); ++it)
    {
        if (*it != "")
            argsList.push_back(*it);
    }

    return argsList;
}

static const std::regex  versionRe("^-([2,3])(?:\\.(\\d+))?$");

Options::Options(const ArgsList& argList) :
    pyMajorVersion(-1),
    pyMinorVersion(-1),
    global(true),
    showHelp(false)
{

    args = argList;

    for (auto it = args.begin(); it != args.end();)
    {
        if (*it == "--global" || *it == "-g")
        {
            global = true;
            it = args.erase(it);
            continue;
        }

        if (*it == "--local" || *it == "-l")
        {
            global = false;
            it = args.erase(it);
            continue;
        }

        if (*it == "--help" || *it == "-h")
        {
            showHelp = true;
            it = args.erase(it);
            continue;
        }

        std::smatch  mres;
        if (std::regex_match(*it, mres, versionRe))
        {
            pyMajorVersion = atol(std::string(mres[1].first, mres[1].second).c_str());

            if (mres[2].matched)
            {
                pyMinorVersion = atol(std::string(mres[2].first, mres[2].second).c_str());
            }

            it = args.erase(it);
            continue;
        }
            
        break;
    }

}


