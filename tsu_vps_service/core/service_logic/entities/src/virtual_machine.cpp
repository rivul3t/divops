/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/entities/src

#include <tsu_vps_service/core/service_logic/entities/virtual_machine.h>
#include <tsu_vps_service/core/helpers/utils.h>

#include <iostream>

namespace service_logic
{

std::string RateTypeToString( const RateType& value )
{
    switch ( value )
    {
        case RateType::Type0:
            return "type0";
        case RateType::Type1:
            return "type1";
        case RateType::Type2:
            return "type2";
        default:
            throw std::runtime_error( "Invalid rate type" );
    }
}

RateType StringToRateType( const std::string& value )
{
    if( value == "type0" )
    {
        return RateType::Type0;
    }
    if( value == "type1" )
    {
        return RateType::Type1;
    }
    if( value == "type2" )
    {
        return RateType::Type2;
    }
    throw std::runtime_error( "Invalid role" );
}

void WriteJson( Json::Value& jsonValue, const NewVirtualMachine& value )
{
    jsonValue[ "user_id" ]  = value.userId_;
    jsonValue[ "address" ] = value.address_;
    jsonValue[ "password" ] = utils::Base64Decode( value.password_ );
    jsonValue[ "rate_type" ] = RateTypeToString( value.rateType_ );
}

void ReadJson( const Json::Value& jsonValue, NewVirtualMachine& value )
{
    try
    {
        value.userId_ = jsonValue[ "user_id" ].asInt();
        value.address_ = jsonValue[ "address" ].asString();
        value.password_ = utils::Base64Encode( jsonValue[ "password" ].asString() );
        value.rateType_ = StringToRateType( jsonValue[ "rate_type" ].asString() );
    }
    catch( ... )
    {
        std::cerr << "ReadJson exception";
    }
}


void WriteJson( Json::Value& jsonValue, const VirtualMachine& value )
{
    jsonValue[ "id" ] = value.id_;
    WriteJson( jsonValue, value.newVirtualMachine_ );
}
void ReadJson( const Json::Value& jsonValue, VirtualMachine& value )
{
    try
    {
        value.id_ = jsonValue[ "id" ].asInt();
        ReadJson( jsonValue, value.newVirtualMachine_ );
    }
    catch( ... )
    {
        std::cerr << "ReadJson exception";
    }
}

} //namespace service_logic