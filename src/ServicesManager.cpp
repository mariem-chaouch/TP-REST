#include "ServicesManager.h"

#include <charconv>
#include <optional>
#include <utility>

void ServicesManager::registerService(std::unique_ptr<AbstractService> service)
{
    services_.push_back(std::move(service));
}

AbstractService* ServicesManager::findService(std::string_view url) const
{
    for (const auto& service : services_)
    {
        const std::string pattern = service->getPattern();
        if (url == pattern ||((url.starts_with(pattern)) && url[pattern.size()] == '/'))
        {
            return (service.get());
        }
    }
    return nullptr;
}

http::status ServicesManager::queryService(std::string& out, const std::string& in,
                                           std::string_view url, http::verb method)
{
    url = url.substr(0, url.find('?'));
    AbstractService* service =findService(url);
    if (service == nullptr) {
        throw ServiceException(http::status::not_found,"Service not found");
    }
    std::optional<int> id = std::nullopt;
    const std::string& pattern = service->getPattern();
    if (url.size() > pattern.size()+1)
    {
        std::string_view texte = url.substr(pattern.size() + 1);
        int value=0;
        auto [ptr, ec] = std::from_chars(texte.data(), texte.data() + texte.size(), value);
        if (ec != std::errc() || ptr != texte.data()+texte.size())
        {
            throw ServiceException(http::status::bad_request,"Invalid ID");
        }
        id=value;
    }
    json::value obj ;
    http::status status ;
    switch (method)
    {
    case http::verb::get:
        status=service->get(obj, id);break;
    case http::verb::post:
        {
            if (id.has_value())
            {
                throw ServiceException(http::status::method_not_allowed,"POST requires a collection URL");
            }
            boost::system::error_code ec;
            json::value input = json::parse(in, ec);
            if (ec)
            {
                throw ServiceException(http::status::bad_request,ec.message());
            }
            status=service->post(obj, input);
            ;break;
        }

        default:throw ServiceException(http::status::not_implemented,"Method not implemented");
    }

    out= json::serialize(obj);

    return status;

}
