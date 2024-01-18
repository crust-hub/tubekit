#pragma once
#include <string>
#include <vector>
#include <stdarg.h>

namespace tubekit::ecsactor
{
    namespace strutil
    {
        /**
         * @brief 字符串比较
         *
         * @param c1
         * @param c2
         * @return int
         */
        int stricmp(const char *c1, const char *c2);

        /**
         * @brief 格式化
         *
         * @param _format
         * @param ...
         * @return std::string
         */
        std::string format(const char *_format, ...);

        /**
         * @brief 裁剪掉字符串开头和末尾的空格
         *
         * @param s
         * @return std::string
         */
        std::string trim(const std::string &s);

        /**
         * @brief 将str内的pattern替换为nwepat
         *
         * @param str
         * @param pattern
         * @param newpat
         * @return int
         */
        int replace(std::string &str, const std::string &pattern, const std::string &newpat);

        /**
         * @brief 将s以c分割，内容装进v
         *
         * @tparam T
         * @param s
         * @param c
         * @param v
         */
        template <typename T>
        void split(const std::basic_string<T> &s, T c, std::vector<std::basic_string<T>> &v);
    }
}

template <typename T>
void tubekit::ecsactor::strutil::split(const std::basic_string<T> &s, T c, std::vector<std::basic_string<T>> &v)
{
    if (s.size() == 0)
    {
        return;
    }

    // using 'typename' keyword to plain std::basic_string<T>::size_type is a type but variable
    typename std::basic_string<T>::size_type i = 0;
    typename std::basic_string<T>::size_type j = s.find(c);

    while (j != std::basic_string<T>::npos)
    {
        std::basic_string<T> buf = s.substr(i, j - i);
        if (buf.size() > 0)
        {
            v.push_back(buf);
        }
        i = ++j;
        j = s.find(c, j);
    }

    if (j == std::basic_string<T>::npos)
    {
        std::basic_string<T> buf = s.substr(i, s.length() - i);
        if (buf.size() > 0)
        {
            v.push_back(buf);
        }
    }
}
