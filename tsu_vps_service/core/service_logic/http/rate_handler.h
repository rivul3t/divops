/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/http

#pragma once

#include <drogon/HttpController.h>
#include <cstdint>

namespace service_logic::http
{

class RateHandler : public drogon::HttpController< RateHandler >
{
public:

    METHOD_LIST_BEGIN
    ADD_METHOD_TO( RateHandler::GetRate, "/logic/rate/{1}", drogon::Get );
    ADD_METHOD_TO( RateHandler::GetRateList, "/logic/rate", drogon::Get );
    ADD_METHOD_TO( RateHandler::SetRate, "/logic/rate", drogon::Post );
    ADD_METHOD_TO( RateHandler::UpdateRate, "/logic/rate/{1}", drogon::Put );
    ADD_METHOD_TO( RateHandler::DeleteRate, "/logic/rate/{1}", drogon::Delete );
    METHOD_LIST_END

    void GetRate( const drogon::HttpRequestPtr& request,
                  std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                  std::uint32_t rateId ) const;

    void GetRateList( const drogon::HttpRequestPtr& request,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback ) const;

    void SetRate( const drogon::HttpRequestPtr& request,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback ) const;

    void UpdateRate( const drogon::HttpRequestPtr& request,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                     std::uint32_t rateId ) const;

    void DeleteRate( const drogon::HttpRequestPtr& request,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                     std::uint32_t rateId ) const;
};

} //namespace service_logic::http