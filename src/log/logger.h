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
             * @brief get instance
             *
             * @return logger*
             */
            static logger *instance();

            /**
             * @brief open log file
             *
             * @param log_file_path
             */

            void open(const string &log_file_path);

            /**
             * @brief close log file
             *
             */
            void close();

            /**
             * @brief out log in debug
             *
             * @param file content
             * @param line line number
             * @param format
             * @param ... param
             */
            void debug(const char *file, int line, const char *format, ...);

            /**
             * @brief out log in info
             *
             * @param file content
             * @param line line number
             * @param format
             * @param ... param
             */
            void info(const char *file, int line, const char *format, ...);

            /**
             * @brief out log in warn
             *
             * @param file content
             * @param line line number
             * @param format
             * @param ... param
             */
            void warn(const char *file, int line, const char *format, ...);

            /**
             * @brief out log in error
             *
             * @param file content
             * @param line line number
             * @param format
             * @param ... param
             */
            void error(const char *file, int line, const char *format, ...);

            /**
             * @brief out log in fatlal
             *
             * @param file content
             * @param line line number
             * @param format
             * @param ... param
             */
            void fatal(const char *file, int line, const char *format, ...);

        protected:
            void log(flag f, const char *file, int line, const char *format, va_list arg_ptr);

        protected:
            FILE *m_fp;
            static const char *s_flag[FLAG_COUNT];
            static logger *m_instance; // log instance
        };
    }
}