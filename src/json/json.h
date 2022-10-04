#pragma once

#include <string>
#include <vector>
#include <map>

namespace tubekit
{
    namespace json
    {
        class json{
        public:
            using iterator = std::vector<json>::iterator;
            using const_iterator = std::vector<json>::const_iterator;
            
            enum type_enum
            {
                json_null,
                json_bool,
                json_int,
                json_double,
                json_string,
                json_array,
                json_object
            };
        public:
            json();
            json(type_enum m_type);
            json(bool value);
            json(int value);
            json(double value);
            json(const char* value);
            json(const std::string& value);
            json(const json& other);
            ~json();
            
            type_enum type() const;
            
            bool is_null() const;
            bool is_bool() const;
            bool is_int() const;
            bool is_double() const;
            bool is_string() const;
            bool is_array() const;
            bool is_object() const;
            
            bool as_bool() const;
            int as_int() const;
            double as_double() const;
            std::string as_string() const;
            
            void copy(const json& other);
            
            size_t size() const;
            bool empty() const;
            void clear();
            
            bool remove(size_t index);
            bool remove(const char* key);
            bool remove(const std::string& key);
            
            bool has(size_t index);
            bool has(const char* key);
            bool has(const std::string& key);
            
            const json& get(size_t index) const;
            const json& get(const char* key) const;
            const json& get(const std::string& key) const;

            void set(const json& other);
            void set(bool value);
            void set(int value);
            void set(double value);
            void set(const std::string& value);
            void set(const char* value);

            json& append(const json& value);
            json& operator=(const json& other);
            json& operator=(const bool& value);
            json& operator=(const int& value);
            json& operator=(const double& value);
            json& operator=(const std::string& value);
            json& operator=(const char* value);

            bool operator==(const json& other);
            bool operator==(const bool& value);
            bool operator==(const int& value);
            bool operator==(const double& value);
            bool operator==(const std::string& value);
            bool operator==(const char* value);

            bool operator!=(const json& other);
            bool operator!=(const bool& value);
            bool operator!=(const int& value);
            bool operator!=(const double& value);
            bool operator!=(const std::string& value);
            bool operator!=(const char* value);


            json& operator[](int index);
            const json& operator[](int index)const;
            json& operator[](const char* key);
            const json& operator[](const char* key) const;
            json& operator[](const std::string& key);
            const json& operator[](const std::string& key) const;

            operator bool();
            operator int();
            operator double();
            operator std::string();
            operator std::string() const;

            static json const& null();

            void parse(const std::string& str);
            std::string to_string() const;

            iterator begin();
            iterator end();
            const_iterator begin() const;
            const_iterator end() const;

        private:
            type_enum m_type;
            int m_int;
            double m_double;
            bool m_bool;
            std::string m_string;
            std::vector<json> m_array;
            std::map<std::string,json> m_object;

        };
    }
}

