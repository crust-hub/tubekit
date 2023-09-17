#include "connection/connection.h"

using tubekit::connection::connection;

connection::connection() : close_flag(false)
{
}

connection::~connection()
{
}

void connection::close_before()
{
}

void connection::mark_close()
{
    close_flag = true;
}

bool connection::is_close()
{
    return close_flag;
}