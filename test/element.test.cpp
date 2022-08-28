#include <iostream>
#include "../src/xml/element.h"

using namespace std;
using namespace tubekit::xml;

int main(int argc, char **argv)
{
    element el;
    el.name("div");

    // el.append(element("span"));
    el.text("hello world");

    el.operator<<(cout);
    cout << endl;
    return 0;
}
// g++ ../src/xml/element.cpp element.test.cpp -o element.test.exe
