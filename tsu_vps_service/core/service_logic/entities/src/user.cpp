/// @brief
///
/// @file divops — tsu_vps_service/core/service_logic/entities/src

#include <tsu_vps_service/core/service_logic/entities/user.h>
#include <tsu_vps_service/core/helpers/utils.h>
#include <tsu_vps_service/core/helpers/exception.h>

namespace service_logic
{

std::string RoleToString( const UserRole& role )
{
    switch (role)
    {
        case UserRole::ServiceAdministrator: return "administrator";
        case UserRole::ServiceAuthUser: return "auth_user";
        case UserRole::ServiceUser:   return "user";
    }
    throw std::runtime_error( "Invalid role" );
}

UserRole StringToRole( const std::string& role )
{
      if( role == "administrator" )
      {
            return UserRole::ServiceAdministrator;
      }
      if( role == "auth_user" )
      {
            return UserRole::ServiceAuthUser;
      }
      if( role == "user" )
      {
            return UserRole::ServiceUser;
      }
      throw std::runtime_error( "Invalid role" );
}

void WriteJson( Json::Value& jsonValue, const NewUser& value )
{
    jsonValue[ "email" ] = value.email_;
    jsonValue[ "password" ] = utils::Base64Decode( value.password_ );
    jsonValue[ "role" ] = RoleToString( value.role_ );
    jsonValue[ "created_at" ] = utils::from_iso8601( value.creationDate_ );
    jsonValue[ "updated_at" ] = utils::from_iso8601( value.updatedDate_ );
}
void ReadJson( const Json::Value& jsonValue, NewUser& value )
{
    try
    {
        value.email_ = jsonValue[ "email" ].asString();
        value.password_ = utils::Base64Encode( jsonValue[ "password" ].asString() );
        value.role_ = StringToRole( jsonValue[ "role" ].asString() );
    }
    catch( ... )
    {
        utils::RethrowException( utils::Exception( utils::ExceptionType::WrongArgument, "ReadJson(NewUser)" ) );
    }
}


void WriteJson( Json::Value& jsonValue, const User& value )
{
    jsonValue[ "id" ] = value.id_;
    WriteJson( jsonValue, value.user_ );
}
void ReadJson( const Json::Value& jsonValue, User& value )
{
    try
    {
        value.id_ = jsonValue[ "id" ].asInt();
        ReadJson( jsonValue, value.user_ );
    }
    catch( ... )
    {
        utils::RethrowException( utils::Exception( utils::ExceptionType::WrongArgument, "ReadJson(User)" ) );
    }
}

} //namespace service_logic