#pragma once
#include <string>
#include <cstdint>
#include "connection/websocket_connection.h"
#include <set>
#include "thread/mutex.h"

// thread not safe : The function will be called by multiple threads simultaneously.
// thread safe : The function can only be called by the main thread or can be used in any thread.

namespace tubekit
{
    namespace app
    {
        class websocket_app
        {
        public:
            struct websocket_frame
            {
                uint8_t fin;
                uint8_t opcode;
                uint8_t mask;
                uint64_t payload_length;
                std::vector<uint8_t> masking_key;
                std::string payload_data;
            };

            enum class websocket_frame_type
            {
                CONNECTION_CLOSE_FRAME = 0,
                TEXT_FRAME = 1,
                BINARY_FRAME = 2,
                PONG = 3,
                PING = 4,
                CONTINUATION_FRAME = 5,
                FURTHER_NON_CONTROL = 6,
                FURTHER_CONTROL = 7,
                ERROR = 8
            };

            static websocket_frame_type n_2_websocket_frame_type(uint8_t n);
            static uint8_t websocket_frame_type_2_n(websocket_frame_type type, uint8_t idx = 0x0);

        public:
            /**
             * @brief thread not safe
             *
             * @param m_websocket_connection
             */
            static void process_connection(tubekit::connection::websocket_connection &m_websocket_connection);

            /**
             * @brief thread not safe
             *
             * @param m_websocket_connection
             * @param frame
             */
            static void process_frame(tubekit::connection::websocket_connection &m_websocket_connection,
                                      websocket_frame &frame);

            /**
             * @brief thread not safe
             *
             * @param m_websocket_connection
             */
            static void on_close_connection(tubekit::connection::websocket_connection &m_websocket_connection);

            /**
             * @brief thread not safe
             *
             * @param m_websocket_connection
             */
            static void on_new_connection(tubekit::connection::websocket_connection &m_websocket_connection);

            /**
             * @brief thread safe
             *
             * @param m_websocket_connection
             * @param first_byte
             * @param data
             * @param data_len
             * @param gid
             * @return true
             * @return false
             */
            static bool send_packet(tubekit::connection::websocket_connection *m_websocket_connection,
                                    uint8_t first_byte,
                                    const char *data,
                                    size_t data_len,
                                    uint64_t gid = 0);

            /**
             * @brief thread safe
             *
             * @return int
             */
            static int on_init();

            /**
             * @brief thread safe
             *
             */
            static void on_stop();

            /**
             * @brief thread safe
             *
             */
            static void on_tick();

            static std::set<uint64_t> global_player;
            static tubekit::thread::mutex global_player_mutex;
        };
    }
}
