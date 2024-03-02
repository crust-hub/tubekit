#pragma once

#include <cstring>
#include <memory>
#include <mysql/mysql.h>
#include "connection.h"

namespace tubekit
{
    namespace sql
    {
        enum value_type
        {
            BIT = 0,
            BLOB,
            DATE,
            DATETIME2,
            DATETIME,
            DECIMAL,
            DOUBLE,
            ENUM,
            FLOAT,
            GEOMETRY,
            INT24,
            INVALID,
            JSON,
            LONG,
            LONG_BLOB,
            LONGLONG,
            MEDIUM_BLOB,
            NEWDATE,
            NEWDECIMAL,
            NULL_TYPE,
            SET,
            SHORT,
            STRING,
            TIME2,
            TIME,
            TIMESTAMP2,
            TIMESTAMP,
            TINY,
            TINY_BLOB,
            TYPED_ARRAY,
            VAR_STRING,
            VARCHAR,
            YEAR
        };

        /**
         * @brief query result template
         *
         * @tparam bind_size
         */
        template <size_t bind_size>
        class result
        {
        public:
            result();
            ~result();
            void set_bind(size_t index, void *buffer, unsigned long size, value_type type, bool *is_null);
            MYSQL_BIND *get_bind();
            size_t get_size();

        private:
            MYSQL_BIND bind[bind_size];
            size_t size{bind_size};
        };

        template <size_t bind_size>
        result<bind_size>::result()
        {
            ::memset(bind, 0, sizeof(bind));
        }

        template <size_t bind_size>
        result<bind_size>::~result()
        {
        }

        template <size_t bind_size>
        size_t result<bind_size>::get_size()
        {
            return size;
        }

        template <size_t bind_size>
        MYSQL_BIND *result<bind_size>::get_bind()
        {
            return this->bind;
        }

        template <size_t bind_size>
        void result<bind_size>::set_bind(size_t index, void *buffer, unsigned long size, value_type type, bool *is_null)
        {
            switch (type)
            {
            case BIT:
                bind[index].buffer_type = MYSQL_TYPE_BIT;
                break;
            case BLOB:
                bind[index].buffer_type = MYSQL_TYPE_BLOB;
                break;
            case DATE:
                bind[index].buffer_type = MYSQL_TYPE_DATE;
                break;
            case DATETIME2:
                bind[index].buffer_type = MYSQL_TYPE_DATETIME2;
                break;
            case DATETIME:
                bind[index].buffer_type = MYSQL_TYPE_DATETIME;
                break;
            case DECIMAL:
                bind[index].buffer_type = MYSQL_TYPE_DECIMAL;
                break;
            case DOUBLE:
                bind[index].buffer_type = MYSQL_TYPE_DOUBLE;
                break;
            case ENUM:
                bind[index].buffer_type = MYSQL_TYPE_ENUM;
                break;
            case FLOAT:
                bind[index].buffer_type = MYSQL_TYPE_FLOAT;
                break;
            case GEOMETRY:
                bind[index].buffer_type = MYSQL_TYPE_GEOMETRY;
                break;
            case INT24:
                bind[index].buffer_type = MYSQL_TYPE_INT24;
                break;
            case INVALID:
                bind[index].buffer_type = MYSQL_TYPE_INVALID;
                break;
            case JSON:
                bind[index].buffer_type = MYSQL_TYPE_JSON;
                break;
            case LONG:
                bind[index].buffer_type = MYSQL_TYPE_LONG;
                break;
            case LONG_BLOB:
                bind[index].buffer_type = MYSQL_TYPE_LONG_BLOB;
                break;
            case LONGLONG:
                bind[index].buffer_type = MYSQL_TYPE_LONGLONG;
                break;
            case MEDIUM_BLOB:
                bind[index].buffer_type = MYSQL_TYPE_MEDIUM_BLOB;
                break;
            case NEWDATE:
                bind[index].buffer_type = MYSQL_TYPE_NEWDATE;
                break;
            case NEWDECIMAL:
                bind[index].buffer_type = MYSQL_TYPE_NEWDECIMAL;
                break;
            case NULL_TYPE:
                bind[index].buffer_type = MYSQL_TYPE_NULL;
                break;
            case SET:
                bind[index].buffer_type = MYSQL_TYPE_SET;
                break;
            case SHORT:
                bind[index].buffer_type = MYSQL_TYPE_SHORT;
                break;
            case STRING:
                bind[index].buffer_type = MYSQL_TYPE_STRING;
                break;
            case TIME2:
                bind[index].buffer_type = MYSQL_TYPE_TIME2;
                break;
            case TIME:
                bind[index].buffer_type = MYSQL_TYPE_TIME;
                break;
            case TIMESTAMP2:
                bind[index].buffer_type = MYSQL_TYPE_TIMESTAMP2;
                break;
            case TIMESTAMP:
                bind[index].buffer_type = MYSQL_TYPE_TIMESTAMP;
                break;
            case TINY:
                bind[index].buffer_type = MYSQL_TYPE_TINY;
                break;
            case TINY_BLOB:
                bind[index].buffer_type = MYSQL_TYPE_TINY_BLOB;
                break;
            case TYPED_ARRAY:
                bind[index].buffer_type = MYSQL_TYPE_TYPED_ARRAY;
                break;
            case VAR_STRING:
                bind[index].buffer_type = MYSQL_TYPE_VAR_STRING;
                break;
            case VARCHAR:
                bind[index].buffer_type = MYSQL_TYPE_VARCHAR;
                break;
            case YEAR:
                bind[index].buffer_type = MYSQL_TYPE_YEAR;
                break;
            default:
                bind[index].buffer_type = MYSQL_TYPE_BLOB;
                break;
            }
            bind[index].buffer = buffer;
            bind[index].buffer_length = size;
            bind[index].is_null = is_null;
        }

        /**
         * @brief sql template
         *
         * @tparam bind_size
         */
        template <size_t bind_size>
        class sql
        {
        public:
            sql(std::weak_ptr<connection> conn, const std::string &sql);
            ~sql();
            template <size_t result_size>
            bool execute(result<result_size> &m_result);
            bool fetch();
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
        template <size_t result_size>
        bool sql<bind_size>::execute(result<result_size> &m_result)
        {
            if (m_conn.expired())
            {
                std::cerr << "sql m_conn. is expired" << std::endl;
                return false;
            }
            else
            {
                std::cout << " m_conn.lock()" << std::endl;
                auto m_conn_ptr = m_conn.lock();
                std::cout << "mysql_stmt_bind_param(stmt, bind)" << std::endl;
                if (0 != mysql_stmt_bind_param(stmt, bind))
                {
                    std::cout << mysql_stmt_error(stmt) << std::endl;
                    return false;
                }
                std::cout << "mysql_stmt_execute(stmt)" << std::endl;
                if (0 != mysql_stmt_execute(stmt))
                {
                    std::cout << mysql_stmt_error(stmt) << std::endl;
                    return false;
                }
                std::cout << "mysql_stmt_bind_result" << std::endl;
                if (0 != mysql_stmt_bind_result(stmt, m_result.get_bind()))
                {
                    std::cout << mysql_stmt_error(stmt) << std::endl;
                    return false;
                }
            }
            return true;
        }

        template <size_t bind_size>
        bool sql<bind_size>::fetch()
        {
            int res = mysql_stmt_fetch(stmt);
            if (res == 0)
            {
                return true;
            }
            else if (res == 1)
            {
                std::cout << "fetch end" << std::endl;
                return false;
            }
            else if (res == MYSQL_NO_DATA)
            {
                std::cout << "fetch result is no data" << std::endl;
                return false;
            }
            else if (res == MYSQL_DATA_TRUNCATED)
            {
                std::cout << "fetch result is MYSQL_DATA_TRUNCATED" << std::endl;
                return false;
            }
            return false;
        }

        template <size_t bind_size>
        void sql<bind_size>::set_bind(size_t index, void *buffer, unsigned long size, value_type type, bool is_null)
        {
            switch (type)
            {
            case BIT:
                bind[index].buffer_type = MYSQL_TYPE_BIT;
                break;
            case BLOB:
                bind[index].buffer_type = MYSQL_TYPE_BLOB;
                break;
            case DATE:
                bind[index].buffer_type = MYSQL_TYPE_DATE;
                break;
            case DATETIME2:
                bind[index].buffer_type = MYSQL_TYPE_DATETIME2;
                break;
            case DATETIME:
                bind[index].buffer_type = MYSQL_TYPE_DATETIME;
                break;
            case DECIMAL:
                bind[index].buffer_type = MYSQL_TYPE_DECIMAL;
                break;
            case DOUBLE:
                bind[index].buffer_type = MYSQL_TYPE_DOUBLE;
                break;
            case ENUM:
                bind[index].buffer_type = MYSQL_TYPE_ENUM;
                break;
            case FLOAT:
                bind[index].buffer_type = MYSQL_TYPE_FLOAT;
                break;
            case GEOMETRY:
                bind[index].buffer_type = MYSQL_TYPE_GEOMETRY;
                break;
            case INT24:
                bind[index].buffer_type = MYSQL_TYPE_INT24;
                break;
            case INVALID:
                bind[index].buffer_type = MYSQL_TYPE_INVALID;
                break;
            case JSON:
                bind[index].buffer_type = MYSQL_TYPE_JSON;
                break;
            case LONG:
                bind[index].buffer_type = MYSQL_TYPE_LONG;
                break;
            case LONG_BLOB:
                bind[index].buffer_type = MYSQL_TYPE_LONG_BLOB;
                break;
            case LONGLONG:
                bind[index].buffer_type = MYSQL_TYPE_LONGLONG;
                break;
            case MEDIUM_BLOB:
                bind[index].buffer_type = MYSQL_TYPE_MEDIUM_BLOB;
                break;
            case NEWDATE:
                bind[index].buffer_type = MYSQL_TYPE_NEWDATE;
                break;
            case NEWDECIMAL:
                bind[index].buffer_type = MYSQL_TYPE_NEWDECIMAL;
                break;
            case NULL_TYPE:
                bind[index].buffer_type = MYSQL_TYPE_NULL;
                break;
            case SET:
                bind[index].buffer_type = MYSQL_TYPE_SET;
                break;
            case SHORT:
                bind[index].buffer_type = MYSQL_TYPE_SHORT;
                break;
            case STRING:
                bind[index].buffer_type = MYSQL_TYPE_STRING;
                break;
            case TIME2:
                bind[index].buffer_type = MYSQL_TYPE_TIME2;
                break;
            case TIME:
                bind[index].buffer_type = MYSQL_TYPE_TIME;
                break;
            case TIMESTAMP2:
                bind[index].buffer_type = MYSQL_TYPE_TIMESTAMP2;
                break;
            case TIMESTAMP:
                bind[index].buffer_type = MYSQL_TYPE_TIMESTAMP;
                break;
            case TINY:
                bind[index].buffer_type = MYSQL_TYPE_TINY;
                break;
            case TINY_BLOB:
                bind[index].buffer_type = MYSQL_TYPE_TINY_BLOB;
                break;
            case TYPED_ARRAY:
                bind[index].buffer_type = MYSQL_TYPE_TYPED_ARRAY;
                break;
            case VAR_STRING:
                bind[index].buffer_type = MYSQL_TYPE_VAR_STRING;
                break;
            case VARCHAR:
                bind[index].buffer_type = MYSQL_TYPE_VARCHAR;
                break;
            case YEAR:
                bind[index].buffer_type = MYSQL_TYPE_YEAR;
                break;
            default:
                bind[index].buffer_type = MYSQL_TYPE_BLOB;
                break;
            }
            bind[index].buffer = buffer;
            static bool static_is_null = true;
            if (is_null)
                bind[index].is_null = &static_is_null;
            else
                bind[index].is_null = nullptr;
            bind[index].buffer_length = size;
        }
    }
}