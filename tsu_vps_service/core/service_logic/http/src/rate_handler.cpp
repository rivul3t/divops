/// @brief Файл содержит реализацию методов для работы с данными тарифов
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/http/src

#include <tsu_vps_service/core/service_logic/http/rate_handler.h>
#include <tsu_vps_service/core/service_logic/logic/rate_handler.h>

namespace service_logic::http
{

void RateHandler::GetRate( const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback, std::uint32_t id ) const try
{
    logic::GetRate handler;
    auto record = handler( id );

    if( !record.has_value() )
    {
        auto response = drogon::HttpResponse::newHttpResponse();
        response->setStatusCode( drogon::k404NotFound );
        callback( response );
        return;
    }

    Json::Value jsonValue;
    WriteJson( jsonValue, record.value() );

    auto response = drogon::HttpResponse::newHttpJsonResponse( jsonValue );
    response->setStatusCode( drogon::k200OK );
    callback( response );
}
catch( const std::exception& e )
{

}

void RateHandler::GetRateList( const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback ) const try
{
    logic::GetRateList handler;
    auto recordList = handler();

    Json::Value jsonArray( Json::arrayValue );
    for( const auto& rate : recordList )
    {
        Json::Value rateJson;
        WriteJson( rateJson, rate );
        jsonArray.append( std::move( rateJson ) );
    }

    auto response = drogon::HttpResponse::newHttpJsonResponse( jsonArray );
    response->setStatusCode( drogon::k200OK );
    callback( response );
}
catch( ... )
{

}

void RateHandler::SetRate( const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback ) const try
{
    auto jsonPtr = request->getJsonObject();
    if( !jsonPtr )
    {
        auto response = drogon::HttpResponse::newHttpResponse();
        response->setStatusCode( drogon::k400BadRequest );
        callback( response );
        return;
    }

    NewRate newRate;
    ReadJson( *jsonPtr, newRate );

    logic::SetRate handler;
    handler( newRate );

    auto response = drogon::HttpResponse::newHttpResponse();
    response->setStatusCode( drogon::k201Created );
    callback( response );
}
catch( ... )
{
}

void RateHandler::UpdateRate( const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback, std::uint32_t id ) const try
{
    auto jsonPtr = request->getJsonObject();
    if( !jsonPtr )
    {
        auto response = drogon::HttpResponse::newHttpResponse();
        response->setStatusCode( drogon::k400BadRequest );
        callback( response );
        return;
    }

    Rate rate;
    rate.id_ = id;
    ReadJson( *jsonPtr, rate.rate_ );

    logic::UpdateRate handler;
    bool isUpdated = handler( rate );

    auto response = drogon::HttpResponse::newHttpResponse();
    response->setStatusCode( isUpdated ? drogon::k200OK : drogon::k404NotFound );
    callback( response );
}
catch( ... )
{

}

void RateHandler::DeleteRate( const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback, std::uint32_t id ) const try
{
    logic::DeleteRate handler;
    bool isDeleted = handler( id );

    auto response = drogon::HttpResponse::newHttpResponse();
    response->setStatusCode( isDeleted ? drogon::k200OK : drogon::k404NotFound );
    callback( response );
}
catch( ... )
{

}

} //namespace service_logic::http