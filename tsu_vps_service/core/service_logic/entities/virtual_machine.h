/// @brief Файл содержит модели данных для описания виртуальных машин
///
/// @file divops — tsu_vps_service/core/service_logic/entities

#pragma once

#include <tsu_vps_service/core/service_logic/entities/rate.h>
#include <json/json.h>

namespace service_logic
{

/// @brief Данные о новой виртуальной машине
struct NewVirtualMachine
{
    std::int32_t userId_;
    std::int32_t rateId_;
    std::string address_;
    std::vector< std::uint8_t > password_;
};

void WriteJson( Json::Value& jsonValue, const NewVirtualMachine& value );
void ReadJson( const Json::Value& jsonValue, NewVirtualMachine& value );


/// @brief Данные о виртуальной машине
struct VirtualMachine
{
    std::int32_t id_;
    NewVirtualMachine newVirtualMachine_;
};

void WriteJson( Json::Value& jsonValue, const VirtualMachine& value );
void ReadJson( const Json::Value& jsonValue, VirtualMachine& value );


} //namespace service_logic