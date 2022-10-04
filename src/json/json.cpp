#include "json/json.h"

using namespace tubekit::json;

json::iterator json::begin()
{
    return m_array.begin();
}

json::iterator json::end()
{
    return m_array.end();
}

json::const_iterator json::begin() const
{
    return m_array.begin();
}

json::const_iterator json::end() const
{
    return m_array.end();
}

