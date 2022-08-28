#pragma once
#include <iostream>
#include <string>
#include "element.h"

namespace tubekit
{
    namespace xml
    {
        using std::string;
        /**
         * @brief xml document object
         *
         */
        class document
        {
        public:
            document();
            ~document();
            void load_file(const string &path);
            void load_string(const string &str);
            void skip_white_spaces();
            element parse();

        private:
            bool parse_declaration();
            bool parse_comment();
            string parse_element_name();
            string parse_element_next();
            string parse_element_attr_key();
            string parse_element_attr_val();

        private:
            string m_str;
            size_t m_idx;
        };
    }
}