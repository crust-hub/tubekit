#pragma once
#include <string>
#include <list>
#include <map>

namespace tubekit::json
{
    class json
    {
    public:
        enum Type
        {
            json_null = 0,
            json_bool,
            json_int,
            json_double,
            json_string,
            json_array,
            json_object
        };
        json();
        json(Type type);
        json(bool value);
        json(int value);
        json(double value);
        json(const char *value);
        json(const std::string &value);
        json(const json &other);
        ~json();

        Type type() const;
        bool isNull() const;
        bool isBool() const;
        bool isInt() const;
        bool isDouble() const;
        bool isString() const;
        bool isArray() const;
        bool isObject() const;

        bool asBool() const;
        int asInt() const;
        double asDouble() const;
        std::string asString() const;

        // array size
        int size() const;
        bool empty() const;
        void clear() const;
        void append(const json &value);

        bool has(int index) const;
        bool has(const char *key) const;
        bool has(const std::string &key) const;

        json get(int index) const;
        json get(const char *key) const;
        json get(const std::string &key) const;

        void remove(int index);
        void remove(const char *key);
        void remove(const std::string &key);

        json &operator=(const json &other);
        bool operator==(const json &other);
        bool operator!=(const json &other);

        json &operator[](int index);
        json &operator[](const char *key);
        json &operator[](const std::string &key);

        friend std::ostream &operator<<(std::ostream &os, const json &json_obj)
        {
            os << json_obj.str();
            return os;
        }

        operator bool();
        operator int();
        operator double();
        operator std::string();

        void parse(const std::string &str);
        std::string str() const;

        typedef std::list<json>::iterator iterator;

        iterator begin()
        {
            return (m_value.m_array)->begin();
        }

        iterator end()
        {
            return (m_value.m_array)->end();
        }

    private:
        void copy(const json &other);

    private:
        union Value
        {
            bool m_bool;
            int m_int;
            double m_double;
            std::string *m_string;
            std::list<json> *m_array;
            std::map<std::string, json> *m_object;
        };
        Type m_type;
        Value m_value;
    };
}