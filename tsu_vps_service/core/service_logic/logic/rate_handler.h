/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/logic

#pragma once

#include <optional>
#include <tsu_vps_service/core/service_logic/entities/rate.h>

namespace service_logic::logic
{

class GetRate
{
public:

    explicit GetRate() = default;

    std::optional< Rate > operator()( const std::uint32_t& id ) const;
};

class GetRateList
{
public:

    explicit GetRateList() = default;

    std::vector< Rate > operator()() const;
};

class SetRate
{
public:

    explicit SetRate() = default;

    void operator()( const NewRate& rate ) const;
};

class UpdateRate
{
public:

    explicit UpdateRate() = default;

    bool operator()( const Rate& rate ) const;
};

class DeleteRate
{
public:

    explicit DeleteRate() = default;

    bool operator()( const std::uint32_t& id ) const;
};

} //namespace service_logic::logic