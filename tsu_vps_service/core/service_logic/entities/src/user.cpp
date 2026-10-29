/// @brief
///
/// @file divops — tsu_vps_service/core/service_logic/entities/src

#include <iostream>
#include <tsu_vps_service/core/service_logic/entities/user.h>

namespace service_logic
{

std::string from_iso8601(const std::chrono::system_clock::time_point& timepoint ) {
    auto sec = std::chrono::floor< std::chrono::seconds >( timepoint );
    return std::format("{:%Y-%m-%dT%H:%M:%SZ}", sec);
}

std::chrono::system_clock::time_point to_iso8601( const std::string& time ) {
    std::tm tm{};
    char* end = strptime( time.c_str(), "%Y-%m-%dT%H:%M:%SZ", &tm );
    if ( !end || *end != '\0' ) {
        throw std::runtime_error( "invalid ISO 8601: " + time );
    }
    std::time_t t = timegm( &tm );
    return std::chrono::system_clock::from_time_t( t );
}

std::vector< uint8_t > Base64Encode( const std::string& value )
{
    std::string encodedValue = drogon::utils::base64Encode( value );
    std::vector< uint8_t > result( encodedValue.begin(), encodedValue.end() );

    return result;
}

std::string Base64Decode( const std::vector< uint8_t >& value )
{
    const std::string encodedValue( value.begin(), value.end() );
    return drogon::utils::base64Decode( encodedValue );
}

std::string RoleToString( const UserRole& role )
{
    switch (role) {
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
    jsonValue[ "password" ] = Base64Decode( value.password_ );
    jsonValue[ "role" ] = RoleToString( value.role_ );
    jsonValue[ "created_at" ] = from_iso8601( value.creationDate_ );
    jsonValue[ "updated_at" ] = from_iso8601( value.updatedDate_ );
}
void ReadJson( const Json::Value& jsonValue, NewUser& value )
{
    try
    {
        value.email_ = jsonValue[ "email" ].asString();
        value.password_ = Base64Encode( jsonValue[ "password" ].asString() );
        value.role_ = StringToRole( jsonValue[ "role" ].asString() );
        value.creationDate_ = to_iso8601( jsonValue[ "created_at" ].asString() );
        value.updatedDate_ = to_iso8601( jsonValue[ "updated_at" ].asString() );
    }
    catch( ... )
    {
        std::cerr << "ReadJson exception";
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
        std::cerr << "ReadJson exception";
    }
}

} //namespace service_logic