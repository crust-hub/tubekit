#pragma once

namespace tubekit::ecsactor
{
    class interface_disposable
    {
    public:
        virtual ~interface_disposable()
        {
        }
        virtual void dispose() = 0;
    };
};
