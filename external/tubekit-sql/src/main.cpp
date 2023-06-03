#include <iostream>
#include <memory>

#include "query.h"
#include "connection.h"
#include "sql.h"
#include "pool.h"

using namespace tubekit::sql;
using std::shared_ptr;

int main(int argc, char **argv)
{
    pool m_pool;
    m_pool.init(10, "", "root", "", "cloudalbum");
    for (int i = 0; i < 10; i++)
    {
        shared_ptr<connection> conn = m_pool.get();
        std::cout << "<==>m_list size = " << m_pool.get_size() << std::endl;
        {
            query m_query(conn);
            m_query.select("SELECT * FROM user");
            std::cout << m_query.update("UPDATE user SET user_age = 66 WHERE user_id = 1 ") << std::endl;
            m_query.select("SELECT * FROM user");
        }
        {
            sql<1> m_sql(conn, "SELECT * FROM user WHERE user_id = ?");
            int64_t param_id = 1;
            m_sql.set_bind(0, &param_id, sizeof(param_id), value_type::LONGLONG);

            tubekit::sql::result<8> m_result;
            bool is_null[8];
            int64_t id;
            m_result.set_bind(0, &id, sizeof(id), value_type::LONGLONG, is_null + 0);
            int32_t age;
            m_result.set_bind(1, &age, sizeof(age), value_type::LONG, is_null + 1);
            int64_t avatar;
            m_result.set_bind(2, &avatar, sizeof(avatar), value_type::LONGLONG, is_null + 2);
            char email[300];
            m_result.set_bind(3, email, 300, value_type::VAR_STRING, is_null + 3);
            char name[300];
            m_result.set_bind(4, name, 300, value_type::VAR_STRING, is_null + 4);
            char password[300];
            m_result.set_bind(5, password, 300, value_type::VAR_STRING, is_null + 5);
            char profile[300];
            m_result.set_bind(6, profile, 300, value_type::VAR_STRING, is_null + 6);
            int32_t space;
            m_result.set_bind(7, &space, sizeof(space), value_type::LONG, is_null + 7);

            if (m_sql.execute(m_result))
            {
                while (m_sql.fetch())
                {
                    if (is_null[0])
                    {
                        std::cout << "null ";
                    }
                    else
                    {
                        std::cout << id << " ";
                    }
                    if (is_null[1])
                    {
                        std::cout << "null ";
                    }
                    else
                    {
                        std::cout << age << " ";
                    }
                    if (is_null[2])
                    {
                        std::cout << "null ";
                    }
                    else
                    {
                        std::cout << avatar << " ";
                    }
                    if (is_null[3])
                    {
                        std::cout << "null ";
                    }
                    else
                    {
                        std::cout << email << " ";
                    }
                    if (is_null[4])
                    {
                        std::cout << "null ";
                    }
                    else
                    {
                        std::cout << name << " ";
                    }
                    if (is_null[5])
                    {
                        std::cout << "null ";
                    }
                    else
                    {
                        std::cout << password << " ";
                    }
                    if (is_null[6])
                    {
                        std::cout << "null ";
                    }
                    else
                    {
                        std::cout << profile << " ";
                    }
                    if (is_null[7])
                    {
                        std::cout << "null ";
                    }
                    else
                    {
                        std::cout << space << " ";
                    }
                    std::cout << std::endl;
                }
            }
        }
        m_pool.back(conn);
    }
    return 0;
}
