#include "UserService.h"

UserService::UserService(UserDB& userDB) : AbstractService("/user"), userDB_(userDB) {}

http::status UserService::get(json::value& out, std::optional<int> id) const
{
    if (!id.has_value())
    {
        throw ServiceException(
            http::status::bad_request, "Missing ID");
    }
    const User* user = userDB_.getUser(*id);
    if (user == nullptr)
    {
        throw ServiceException(
            http::status::not_found, "User not found");
    }
    out = json::value_from(*user);
    return (http::status::ok);
}

http::status UserService::post(json::value& out, const json::value& in)
{
    if (!in.is_object())
    {
        throw ServiceException(http::status::bad_request, "Expected a JSON object");
    }
    const json::object& obj=in.as_object();

    const json::value* name = obj.if_contains("name");
    const json::value* age = obj.if_contains("age");
    if (name == nullptr || !name->is_string() || name->as_string().empty())
    {
        throw ServiceException(http::status::bad_request, "Invalid name");
    }
    if (age == nullptr || !age->is_int64() || age->as_int64() < 0 || age->as_int64() > 150)
    {
        throw ServiceException(http::status::bad_request, "Invalid age");
    }
    User user{.name = std::string(name->as_string()), .age = static_cast<int>(age->as_int64())};
    int id =userDB_.addUser(user);
    out={{"id",id}};
    return http::status::created;
}

http::status UserService::patch(std::optional<int> id, const json::value& in)
{
    // TODO Question 10
    (void)id; (void)in;
    throw ServiceException(http::status::not_implemented, "Not implemented yet");
}

http::status UserService::remove(std::optional<int> id)
{
    // TODO Question 12
    (void)id;
    throw ServiceException(http::status::not_implemented, "Not implemented yet");
}
