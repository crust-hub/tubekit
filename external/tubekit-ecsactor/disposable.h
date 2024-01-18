#pragma once

namespace tubekit::ecsactor
{
    /**
     * @brief 销毁接口
     *
     */
    class interface_disposable
    {
    public:
        virtual ~interface_disposable()
        {
        }
        virtual void dispose() = 0;
    };
};
