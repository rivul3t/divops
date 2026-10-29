/// @brief Файл содержит модели данных для информации о пользователях
///
/// @file divops — tsu_vps_service/core/service_logic/entities

#pragma once

#include <string>
#include <chrono>

#include <json/json.h>
#include <drogon/utils/Utilities.h>

namespace service_logic
{

/// @brief Роли пользователей сервиса
enum class UserRole
{
    ServiceAdministrator = 0,
    ServiceAuthUser = 1,
    ServiceUser
};

/// @brief Данные о новом пользователе
struct NewUser
{
    std::string email_;
    std::vector< std::uint8_t > password_;
    UserRole role_;
    std::chrono::time_point<std::chrono::system_clock> creationDate_;
    std::chrono::time_point<std::chrono::system_clock> updatedDate_;
};

void WriteJson( Json::Value& jsonValue, const NewUser& value );
void ReadJson( const Json::Value& jsonValue, NewUser& value );


/// @brief Данные о пользователе
struct User
{
    NewUser user_;
    std::int32_t id_;
};

void WriteJson( Json::Value& jsonValue, const User& value );
void ReadJson( const Json::Value& jsonValue, User& value );


} //namespace service_logic