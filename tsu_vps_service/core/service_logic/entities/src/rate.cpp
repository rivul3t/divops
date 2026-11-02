/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/entities/src

#include <iostream>
#include <tsu_vps_service/core/service_logic/entities/rate.h>

namespace service_logic
{

void WriteJson( Json::Value& jsonValue, const NewRate& value )
{
    jsonValue[ "name" ] = value.name_;
    jsonValue[ "ram_amount" ] = value.ramAmount_;
    jsonValue[ "hd_amount" ] = value.hdAmount_;
    jsonValue[ "price" ] = value.price_;
}

void ReadJson( const Json::Value& jsonValue, NewRate& value )
{
    try
    {
        value.name_ = jsonValue[ "name" ].asString();
        value.ramAmount_ = jsonValue[ "ram_amount" ].asInt();
        value.hdAmount_ = jsonValue[ "hd_amount" ].asInt();
        value.price_ = jsonValue[ "price" ].asInt();
    }
    catch( ... )
    {
        std::cerr << "ReadJson exception (NewRate)\n";
    }
}


void WriteJson( Json::Value& jsonValue, const Rate& value )
{
    jsonValue[ "id" ] = value.id_;
    WriteJson( jsonValue, value.rate_ );
}

void ReadJson( const Json::Value& jsonValue, Rate& value )
{
    try
    {
        value.id_ = jsonValue[ "id" ].asInt();
        ReadJson( jsonValue, value.rate_ );
    }
    catch( ... )
    {
        std::cerr << "ReadJson exception (Rate)\n";
    }
}

} //namespace service_logic