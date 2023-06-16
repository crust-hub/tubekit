#include <stdexcept>
#include <iostream>
#include "redis.h"
#include "hiredis.h"

using namespace tubekit::redis;

redis::redis(const std::string &host, const std::string &pwd, const int &port) : host(host), port(port), pwd(pwd)
{
}

redis::~redis()
{
}

void redis::test()
{
    redisContext *context = redisConnect(host.c_str(), port);
    if (context == nullptr || context->err)
    {
        if (context)
        {
            std::string msg("connect error: ");
            std::string err(context->errstr);
            redisFree(context);
            throw std::runtime_error(msg + err);
        }
        else
        {
            throw std::runtime_error("error: can not allocate redisContext");
        }
    }
    // AUTH
    std::string command = "AUTH " + this->pwd;
    redisReply *reply = nullptr;
    reply = (redisReply *)redisCommand(context, command.c_str());
    if (reply == nullptr)
    {
        redisFree(context);
        throw std::runtime_error("error: auth error");
    }
    freeReplyObject(reply);
    // SELECT
    command = "SELECT 0";
    reply = (redisReply *)redisCommand(context, command.c_str());
    if (reply == nullptr)
    {
        redisFree(context);
        throw std::runtime_error("error: select 0 error");
    }
    freeReplyObject(reply);
    // SET example
    std::string key = "mkey";
    std::string value = "mValue";
    command = "SET " + key + " " + value;
    reply = (redisReply *)redisCommand(context, command.c_str());
    if (reply == nullptr)
    {
        redisFree(context);
        throw std::runtime_error("error: set key-value error");
    }
    freeReplyObject(reply);
    // get example
    command = "GET " + key;
    reply = (redisReply *)redisCommand(context, command.c_str());
    if (reply == nullptr)
    {
        redisFree(context);
        throw std::runtime_error("error: get key-value error");
    }

    std::cout << reply->str << std::endl;

    freeReplyObject(reply);
    // disconnect
    redisFree(context);
}