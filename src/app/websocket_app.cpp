#include "app/websocket_app.h"
#include <iostream>

using namespace tubekit::app;

void websocket_app::process_connection(tubekit::connection::websocket_connection &m_websocket_connection)
{
    std::cout << "process_connection" << std::endl;
}

void websocket_app::on_close_connection(tubekit::connection::websocket_connection &m_websocket_connection)
{
    std::cout << "on_close_connection" << std::endl;
}

void websocket_app::on_new_connection(tubekit::connection::websocket_connection &m_websocket_connection)
{
    std::cout << "on_new_connection" << std::endl;
}