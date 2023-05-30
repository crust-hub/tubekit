#include <iostream>
#include <memory>

#include "query.h"
#include "connection.h"
#include "sql.h"

using namespace tubekit::sql;
using std::shared_ptr;

int main(int argc, char **argv)
{
    shared_ptr<connection> conn = std::make_shared<connection>();
    bool result = conn->connect("", "root", "", "");
    if (result)
    {
        std::cout << "connect success" << std::endl;
    }
    query m_query(conn);
    m_query.select("SELECT * FROM user");
    std::cout << m_query.update("UPDATE user SET user_age = 66 WHERE user_id = 1 ") << std::endl;
    m_query.select("SELECT * FROM user");

    sql<1> m_sql(conn, "SELECT * FROM user WHERE user_id = ?");
    int id = 1;
    m_sql.set_bind(0, &id, sizeof(id), value_type::LONG);
    m_sql.execute(); // BUG

    return 0;
}
