#pragma once
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <cstring>

namespace tubekit
{
    namespace log
    {
        using namespace std;
        class logger
        {
        public:
            logger();

            ~logger();

            /**
             * @brief 日志级别
             *
             */
            enum flag
            {
                DEBUG,
                INFO,
                WARN,
                ERROR,
                FATAL,
                FLAG_COUNT
            };

            /**
             * @brief 获取日志实例
             *
             * @return logger*
             */
            static logger *instance();
            /**
             * @brief 打开日志文件
             *
             * @param log_file_path
             */

            void open(const string &log_file_path);
            /**
             * @brief 关闭日志文件
             *
             */

            void close();
            /**
             * @brief debug打印日志
             *
             * @param file 输出内容
             * @param line 行数
             * @param format 格式
             * @param ... 可变参数
             */

            void debug(const char *file, int line, const char *format, ...);
            /**
             * @brief info打印日志
             *
             * @param file 输出内容
             * @param line 行数
             * @param format 格式
             * @param ... 可变参数
             */

            void info(const char *file, int line, const char *format, ...);
            /**
             * @brief warn打印日志
             *
             * @param file 输出内容
             * @param line 行数
             * @param format 格式
             * @param ... 可变参数
             */

            void warn(const char *file, int line, const char *format, ...);
            /**
             * @brief error打印日志
             *
             * @param file 输出内容
             * @param line 行数
             * @param format 格式
             * @param ... 可变参数
             */

            void error(const char *file, int line, const char *format, ...);
            /**
             * @brief fatal打印日志
             *
             * @param file 输出内容
             * @param line 行数
             * @param format 格式
             * @param ... 可变参数
             */

            void fatal(const char *file, int line, const char *format, ...);

        protected:
            void log(flag f, const char *file, int line, const char *format, va_list arg_ptr);

        protected:
            FILE *m_fp;
            static const char *s_flag[FLAG_COUNT];
            static logger *m_instance; //日志实例
        };
    }
}