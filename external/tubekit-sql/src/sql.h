#pragma once

#include <memory>
#include <mysql/mysql.h>
#include "connection.h"

namespace tubekit
{
    namespace sql
    {
        enum value_type
        {
            TINY = 0,
            SHORT,
            LONG,
            LONGLONG,
            FLOAT,
            DOUBLE,
            DATE,
            TIME,
            DATETIME,
            TIMESTAMP,
            VAR_STRING,
            BLOB
        };
        template <size_t bind_size>
        class sql
        {
        public:
            sql(std::weak_ptr<connection> conn, const std::string &sql);
            ~sql();
            uint64_t execute();
            void set_bind(size_t index, void *buffer, size_t size, value_type type, bool is_null = false);

        private:
            MYSQL_STMT *stmt;
            MYSQL_BIND bind[bind_size];
            std::weak_ptr<connection> m_conn;
        };

        template <size_t bind_size>
        sql<bind_size>::sql(std::weak_ptr<connection> conn, const std::string &sql) : m_conn(conn)
        {
            auto m_conn_ptr = m_conn.lock();
            if ((stmt = mysql_stmt_init(m_conn_ptr->get())) == nullptr)
            {
                std::cout << "stmt == nullptr" << std::endl;
            }
            if (0 != mysql_stmt_prepare(stmt, sql.c_str(), sql.size()))
            {
                std::cout << mysql_stmt_error(stmt) << std::endl;
            }
        }

        template <size_t bind_size>
        sql<bind_size>::~sql()
        {

            mysql_stmt_close(stmt);
        }

        template <size_t bind_size>
        uint64_t sql<bind_size>::execute()
        {
            if (m_conn.expired())
            {
                std::cerr << "sql m_conn. is expired" << std::endl;
                return 0;
            }
            else
            {
                std::cout << " m_conn.lock()" << std::endl;
                auto m_conn_ptr = m_conn.lock();
                std::cout << "mysql_stmt_bind_param(stmt, bind)" << std::endl;
                if (0 != mysql_stmt_bind_param(stmt, bind))
                {
                    std::cout << mysql_stmt_error(stmt) << std::endl;
                }
                std::cout << "mysql_stmt_execute(stmt)" << std::endl;
                if (0 != mysql_stmt_execute(stmt)) // BUG
                {
                    std::cout << mysql_stmt_error(stmt) << std::endl;
                }
                std::cout << "execute return" << std::endl;
                return mysql_affected_rows(m_conn_ptr->get());
            }
        }

        template <size_t bind_size>
        void sql<bind_size>::set_bind(size_t index, void *buffer, unsigned long size, value_type type, bool is_null)
        {
            std::cout << "set_bind size = " << size << std::endl;
            switch (type)
            {
            case TINY:
                bind[index].buffer_type = MYSQL_TYPE_TINY;
                break;
            case SHORT:
                bind[index].buffer_type = MYSQL_TYPE_SHORT;
                break;
            case LONG:
                bind[index].buffer_type = MYSQL_TYPE_LONG;
                break;
            case LONGLONG:
                bind[index].buffer_type = MYSQL_TYPE_LONGLONG;
                break;
            case FLOAT:
                bind[index].buffer_type = MYSQL_TYPE_FLOAT;
                break;
            case DOUBLE:
                bind[index].buffer_type = MYSQL_TYPE_DOUBLE;
                break;
            case DATE:
                bind[index].buffer_type = MYSQL_TYPE_DATE;
                break;
            case DATETIME:
                bind[index].buffer_type = MYSQL_TYPE_DATETIME;
                break;
            case TIME:
                bind[index].buffer_type = MYSQL_TYPE_TIME;
                break;
            case TIMESTAMP:
                bind[index].buffer_type = MYSQL_TYPE_TIMESTAMP;
                break;
            case VAR_STRING:
                bind[index].buffer_type = MYSQL_TYPE_VAR_STRING;
                break;
            case BLOB:
                bind[index].buffer_type = MYSQL_TYPE_BLOB;
                break;
            default:
                break;
            }
            bind[index].buffer = buffer;
            if (is_null)
                bind[index].is_null = (bool *)0;
            else
                bind[index].is_null = (bool *)1;
            bind[index].buffer_length = size;
            bind[index].length = &bind[index].buffer_length;
        }
    }
}