#include "ServicesManager.h"

#include <charconv>
#include <optional>
#include <utility>
#include <iostream>
void ServicesManager::registerService(std::unique_ptr<AbstractService> service)
{
    // TODO Question 3
    (void)service;
}

AbstractService* ServicesManager::findService(std::string_view url) const
{
    // TODO Question 3
    (void)url;
    return nullptr;
}

http::status ServicesManager::queryService(std::string& out, const std::string& in,
                                           std::string_view url, http::verb method)
{
    if (url != "/version") {
        throw ServiceException(http::status::not_found,"Invalid URL");
    }

    if (method != http::verb::get) {
        throw ServiceException(http::status::method_not_allowed,"Method not allowed");
    }
    json::value obj = {{"major", 1},{"minor", 0}};

    out= json::serialize(obj);

    return (http::status::ok);

}
