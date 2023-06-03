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
    bool result = conn->connect("", "root", "", "cloudalbum");
    if (result)
    {
        std::cout << "connect success" << std::endl;
    }
    query m_query(conn);
    m_query.select("SELECT * FROM user");
    std::cout << m_query.update("UPDATE user SET user_age = 66 WHERE user_id = 1 ") << std::endl;
    m_query.select("SELECT * FROM user");
    {
        sql<1> m_sql(conn, "SELECT * FROM user WHERE user_id = ?");
        int64_t param_id = 4;
        m_sql.set_bind(0, &param_id, sizeof(param_id), value_type::LONGLONG);

        tubekit::sql::result<8> m_result;
        int64_t id;
        m_result.set_bind(0, &id, sizeof(id), value_type::LONGLONG);
        int32_t age;
        m_result.set_bind(1, &age, sizeof(age), value_type::LONG);
        int64_t avatar;
        m_result.set_bind(2, &avatar, sizeof(avatar), value_type::LONGLONG);
        char email[300];
        m_result.set_bind(3, email, 300, value_type::VAR_STRING);
        char name[300];
        m_result.set_bind(4, name, 300, value_type::VAR_STRING);
        char password[300];
        m_result.set_bind(5, password, 300, value_type::VAR_STRING);
        char profile[300];
        m_result.set_bind(6, profile, 300, value_type::VAR_STRING);
        int32_t space;
        m_result.set_bind(7, &space, sizeof(space), value_type::LONG);

        if (m_sql.execute(m_result))
        {
            while (m_sql.fetch())
            {
                std::cout << id << " " << age << " " << avatar << " " << email << " " << name << " " << password << " " << profile << " " << space << std::endl;
            }
        }
    }
    return 0;
}
