#include "util_uuid.h"
#include <sys/time.h>
#include <uuid/uuid.h>

std::string tubekit::ecsactor::generate_uuid()
{
    uuid_t uuid;
    uuid_generate(uuid);
    char key[36];
    uuid_unparse(uuid, key);
    std::string tokenkey = key;
    return tokenkey;
}
