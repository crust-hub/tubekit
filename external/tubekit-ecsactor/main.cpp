/**
 * @file ECS Actor Model
 * @author gaowanlu (2209120827@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-10-15
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <iostream>
#include <vector>
#include "util_string.h"
using namespace std;
using namespace ::tubekit;

int main(int argc, char **argv)
{
    // 1 2.300000
    cout << ecsactor::strutil::format("%d %f", 1, 2.3f) << endl;

    std::string str1 = "hello world hello world world";
    int count = ecsactor::strutil::replace(str1, "world", "hello");
    cout << str1 << endl;  // hello hello hello hello hello
    cout << count << endl; // 3

    vector<string> vecs;
    std::string str_split = " hello world hello world ";
    ecsactor::strutil::split(str_split, ' ', vecs);
    for (auto str : vecs)
    {
        cout << "\'" << str << "\'" << endl;
    }
    //'hello' 'world' 'hello' 'world'

    cout << ecsactor::strutil::stricmp("HELLO", "hello") << endl;
    // 0

    cout << ecsactor::strutil::trim("  strutil     ") << endl;
    // strutil
    return 0;
}