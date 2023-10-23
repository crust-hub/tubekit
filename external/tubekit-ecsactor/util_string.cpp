#include "util_string.h"
#include <cstring>
#include <cstdint>

using namespace tubekit::ecsactor;

int strutil::stricmp(const char *c1, const char *c2)
{
    return strcasecmp(c1, c2);
}

std::string strutil::format(const char *_format, ...)
{
    va_list args;
    va_start(args, _format);
    va_list args_copy;
    va_copy(args_copy, args);
    const size_t buffer_size = std::vsnprintf(nullptr, 0, _format, args_copy);
    va_end(args_copy);

    if (buffer_size < 0)
    {
        va_end(args);
        return std::string();
    }

    std::string result(buffer_size + 1, '\0');

    size_t formatted_chars = std::vsnprintf(&result[0], result.size(), _format, args);
    va_end(args);

    if (formatted_chars < 0)
    {
        return std::string();
    }

    result.resize(formatted_chars);
    return result;
}

std::string strutil::trim(const std::string &s)
{
    std::string::const_iterator iter = s.begin();
    while (iter != s.end() && isspace(*iter))
    {
        ++iter; // jump space left
    }
    std::string::const_reverse_iterator riter = s.rbegin();
    while (riter.base() != iter && isspace(*riter))
    {
        ++riter; // jump space right
    }
    // riter.base(); return std::string::const_iterator
    return std::string(iter, riter.base());
}

int strutil::replace(std::string &str, const std::string &pattern, const std::string &newpat)
{
    int count = 0;
    const size_t nsize = newpat.size();
    const size_t psize = pattern.size();

    for (size_t pos = str.find(pattern, 0); pos != std::string::npos; pos = str.find(pattern, pos + nsize))
    {
        str.replace(pos, psize, newpat);
        count++;
    }
    return count;
}