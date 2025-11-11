#ifndef _UTEIS_HPP_
#define _UTEIS_HPP_

#include <string>

std::string trim(const std::string& s);
std::string toUpper(std::string s);
std::string changeExtension(const std::string& fileName, const std::string& extensao);
std::string joinPath(const std::string& dir, const std::string& file);

#endif
