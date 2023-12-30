#include "connection/connection.h"

using tubekit::connection::connection;

connection::connection(tubekit::socket::socket *socket_ptr) : close_flag(false),
                                                              socket_ptr(socket_ptr)
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
    if (close_flag == false)
    {
        close_flag = true;
        on_mark_close();
    }
}

bool connection::is_close()
{
    return close_flag;
}

tubekit::socket::socket *connection::get_socket_ptr()
{
    return socket_ptr;
}

void connection::set_socket_ptr(tubekit::socket::socket *socket_ptr)
{
    this->socket_ptr = socket_ptr;
}

void connection::reuse()
{
    this->close_flag = false;
    this->socket_ptr = nullptr;
}