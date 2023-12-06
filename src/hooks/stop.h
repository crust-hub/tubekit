#pragma once

namespace tubekit
{
    namespace hooks
    {
        class stop
        {
        public:
            /**
             * @brief executed in socket_handler
             *
             */
            void run();
        };
    };
} // namespace tubekit
