#pragma once

namespace tubekit
{
    namespace connection
    {
        class connection
        {
        public:
            connection();
            virtual ~connection();
            virtual void close_before();

        public:
            virtual void on_mark_close() = 0;

        public:
            /**
             * @brief connection will be closed
             *
             */
            void mark_close();
            bool is_close();

        private:
            bool close_flag;
        };
    }
}