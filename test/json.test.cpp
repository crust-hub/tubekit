#include <iostream>
#include <string>
#include "../external/tubekit-json/json.h"
#include "../external/tubekit-json/parser.h"

using namespace std;
using tubekit::json::json;
using tubekit::json::parser;

int main(int argc, char **argv)
{
    json json_obj;
    const std::string str = "{\"int\": 232, \"double\":-434.2, \"arr\":[23,43,1,43.3,{\"n\":{\"arr\":[12,32],\"obj\":{\"in_obj\":{}}}}]}";
    json_obj.parse(str);
    std::cout << json_obj.to_string() << std::endl;
    return 0;
}
// g++ ../external/tubekit-json/json.cpp ../external/tubekit-json/parser.cpp json.test.cpp -o json.test.exe --std=c++17