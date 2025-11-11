#include "Uteis.hpp"
#include <string>

using namespace std;

string trim(const string& s)
{
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start])))
        ++start;

    size_t end = s.size();
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1])))
        --end;

    return (start <= end) ? s.substr(start, end - start) : std::string();
}

string toUpper(string s)
{
    for (char& c : s)
        c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    return s;
}

string changeExtension(const string& fileName, const string& extensao)
{
    std::size_t pos = fileName.find_last_of('.');

    std::string base;
    if (pos == std::string::npos)
        base = fileName;
    else
        base = fileName.substr(0, pos);

    if (extensao.empty())
        return base;

    return (extensao[0] == '.')? base + extensao : base + "." + extensao;
}

string joinPath(const string& dir, const string& file)
{
    if (dir.empty())
        return file;

    if (file.empty())
        return dir;

    char last = dir.back();
    if (last == '/' || last == '\\')
        return dir + file;

    return dir + "/" + file; // funciona em Windows e Unix
}