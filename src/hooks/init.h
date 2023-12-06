#pragma once

namespace tubekit
{
    namespace hooks
    {
        class init
        {
        public:
            /**
             * @brief executed in server, Before the system enters loop listening
             *
             * @return int
             */
            int run();
        };
    };
} // namespace tubekit
