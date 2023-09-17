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
            void mark_close();
            bool is_close();

        private:
            bool close_flag;
        };
    }
}