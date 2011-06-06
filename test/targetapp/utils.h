#pragma once

#include <string>

namespace utils
{

std::string GetWinErrorText(DWORD dwError);

std::string GetLastErrorStr();

} // namespace utils