#include "workflow.h"
using namespace tubekit::engine;

workflow::~workflow()
{
}

bool workflow::load(const std::string &path)
{
    std::cout << "hardworking ...." << __FILE__ << " " << __LINE__ << std::endl;
    return false;
}

bool workflow::run(const std::string work, const std::string input, const std::string output)
{
    return false;
}

bool workflow::load_plugin(work &work, tubekit::xml::element &el)
{
    return false;
}