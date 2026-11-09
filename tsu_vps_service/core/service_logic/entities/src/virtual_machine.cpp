/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/entities/src

#include <tsu_vps_service/core/service_logic/entities/virtual_machine.h>
#include <tsu_vps_service/core/helpers/utils.h>
#include <tsu_vps_service/core/helpers/exception.h>

namespace service_logic
{

void WriteJson( Json::Value& jsonValue, const NewVirtualMachine& value )
{
    jsonValue[ "user_id" ]  = value.userId_;
    jsonValue[ "rate_type" ] = value.rateId_;
    jsonValue[ "address" ] = value.address_;
    jsonValue[ "password" ] = utils::Base64Decode( value.password_ );
}

void ReadJson( const Json::Value& jsonValue, NewVirtualMachine& value )
{
    try
    {
        value.userId_ = jsonValue[ "user_id" ].asInt();
        value.rateId_ = jsonValue[ "rate_type" ].asInt();
        value.address_ = jsonValue[ "address" ].asString();
        value.password_ = utils::Base64Encode( jsonValue[ "password" ].asString() );
    }
    catch( ... )
    {
        utils::RethrowException( utils::Exception( utils::ExceptionType::WrongArgument, "ReadJson(NewVirtualMachine)" ) );
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
        utils::RethrowException( utils::Exception( utils::ExceptionType::WrongArgument, "ReadJson(VirtualMachine)" ) );
    }
}

} //namespace service_logic