#include "UserService.h"

UserService::UserService(UserDB& userDB) : AbstractService("/user"), userDB_(userDB) {}

http::status UserService::get(json::value& out, std::optional<int> id) const
{
    if (!id.has_value())
    {
        json::array users;
        for (const auto& [userId, user] : userDB_.getUsers())
        {
            json::value obj = json::value_from(user);
            obj.as_object()["id"] = userId;
            users.push_back(obj);
        }
        out = users;
        return http::status::ok;
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
    if (!id.has_value())
    {
        throw ServiceException( http::status::method_not_allowed, "Missing ID");
    }
    const User* user = userDB_.getUser(*id);

    if (user == nullptr)
    {
        throw ServiceException(http::status::not_found, "User not found");
    }
    if (!in.is_object())
    {
        throw ServiceException(http::status::bad_request,"Expected a JSON object");
    }
    User updatedUser = *user;
    const json::object& obj = in.as_object();

    for (const auto& [cle, valeur] : obj)
    {
        if (cle == "name")
        {
            if (!valeur.is_string() ||valeur.as_string().empty())
            {
                throw ServiceException(http::status::bad_request,"Invalid name");
            }
            updatedUser.name =std::string(valeur.as_string());
        }
        else if (cle == "age")
        {
            if (!valeur.is_int64() ||valeur.as_int64() < 0 ||valeur.as_int64() > 150)
            {
                throw ServiceException(http::status::bad_request,"Invalid age");
            }
            updatedUser.age =static_cast<int>(valeur.as_int64());
        }
        else
        {
            throw ServiceException(http::status::bad_request,"Unknown field");
        }
    }
    userDB_.setUser(*id, updatedUser);
    return (http::status::no_content);
}

http::status UserService::remove(std::optional<int> id)
{
    if (!id.has_value())
    {
        throw ServiceException( http::status::method_not_allowed, "Missing ID");
    }
    if (!userDB_.removeUser(*id))
    {
        throw ServiceException(http::status::not_found, "User not found");
    }
    return (http::status::no_content);
}

http::status UserService::put(std::optional<int> id, const json::value& in)
{
    if (!id.has_value())
        throw ServiceException(http::status::method_not_allowed, "Missing user ID");

    const User* existingUser = userDB_.getUser(*id);

    if (existingUser == nullptr)
        throw ServiceException(http::status::not_found, "User not found");

    if (!in.is_object())
        throw ServiceException(http::status::bad_request, "Expected JSON object");

    const auto& obj = in.as_object();

    auto name = obj.if_contains("name");
    auto age = obj.if_contains("age");

    if (name == nullptr || !name->is_string() || name->as_string().empty())
        throw ServiceException(http::status::bad_request, "Invalid name");

    if (age == nullptr || !age->is_int64())
        throw ServiceException(http::status::bad_request, "Invalid age");

    int64_t ageValue = age->as_int64();

    if (ageValue < 0 || ageValue > 150)
        throw ServiceException(http::status::bad_request, "Age must be between 0 and 150");

    User updatedUser{
        std::string(name->as_string()),
        static_cast<int>(ageValue)
    };

    if (!userDB_.setUser(*id, updatedUser))
        throw ServiceException(http::status::not_found, "User not found");

    return http::status::no_content;
}