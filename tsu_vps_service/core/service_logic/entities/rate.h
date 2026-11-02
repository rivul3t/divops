/// @brief Файл содержит модели данных для тарифов сервиса
///
/// @file divops — tsu_vps_service/core/service_logic/entities

#pragma once

#include <cstdint>
#include <string>

#include <json/json.h>

namespace service_logic
{

/// @brief Данные о новом тарифе
struct NewRate
{
    std::string name_;
    std::uint32_t ramAmount_;
    std::uint32_t hdAmount_;
    float price_;
};

void WriteJson( Json::Value& jsonValue, const NewRate& value );
void ReadJson( const Json::Value& jsonValue, NewRate& value );


/// @brief Данные о тарифе
struct Rate
{
    std::uint32_t id_;
    NewRate rate_;
};

void WriteJson( Json::Value& jsonValue, const Rate& value );
void ReadJson( const Json::Value& jsonValue, Rate& value );


} //namespace service_logic