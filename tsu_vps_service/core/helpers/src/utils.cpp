/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/helpers/src

#include <tsu_vps_service/core/helpers/utils.h>
#include <stdexcept>
#include <drogon/utils/Utilities.h>

namespace utils
{

std::string from_iso8601(const std::chrono::system_clock::time_point& timepoint ) {
    auto sec = std::chrono::floor< std::chrono::seconds >( timepoint );
    return "";
    // return std::format("{:%Y-%m-%dT%H:%M:%SZ}", sec);
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

TimePoint FromEpochMs( std::int64_t ms )
{
    return TimePoint{ std::chrono::milliseconds{ ms } };
}

std::int64_t ToEpochMs( TimePoint tp )
{
    return std::chrono::duration_cast< std::chrono::milliseconds >( tp.time_since_epoch() ).count();
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

} //namespace utils