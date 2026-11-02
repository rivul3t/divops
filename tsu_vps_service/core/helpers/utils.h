/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/helpers

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <chrono>

namespace utils
{

std::string from_iso8601(const std::chrono::system_clock::time_point& timepoint );
std::chrono::system_clock::time_point to_iso8601( const std::string& time );

using TimePoint = std::chrono::system_clock::time_point;

TimePoint FromEpochMs( std::int64_t ms );
std::int64_t ToEpochMs( TimePoint tp );


std::vector< uint8_t > Base64Encode( const std::string& value );
std::string Base64Decode( const std::vector< uint8_t >& value );

} //namespace utils