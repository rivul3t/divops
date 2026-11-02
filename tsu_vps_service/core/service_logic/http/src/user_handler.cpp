/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/http/src

#include <tsu_vps_service/core/service_logic/http/user_handler.h>
#include <tsu_vps_service/core/service_logic/logic/user_handler.h>
#include <tsu_vps_service/core/service_logic/entities/user.h>

namespace service_logic::http
{

void GetUser::operator()( const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback, uint32_t userId ) const try
{
    logic::GetUser handler;
    auto record = handler( userId );

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
catch( ... )
{

}

void GetUserList::operator()( const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback ) const try
{
    logic::GetUserList handler;
    auto recordList = handler();

    Json::Value jsonArray( Json::arrayValue );
    for( const auto& record : recordList )
    {
        Json::Value userJson;
        WriteJson( userJson, record );
        jsonArray.append( std::move( userJson ) );
    }

    auto response = drogon::HttpResponse::newHttpJsonResponse( jsonArray );
    response->setStatusCode( drogon::k200OK );
    callback( response );
}
catch( ... )
{

}

void SetUser::operator()( const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback ) const try
{
    auto json = request->getJsonObject();
    if( !json or json->empty() )
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode( drogon::k400BadRequest );
        resp->setBody( "Invalid JSON body" );
        callback( resp );
        return;
    }

    NewUser userInfo;
    ReadJson( *json, userInfo );
    logic::SetUser handler;
    handler( userInfo );

    auto response = drogon::HttpResponse::newHttpJsonResponse( *json );
    response->setStatusCode( drogon::k201Created );
    callback( response );
}
catch( ... )
{

}

} //namespace service_logic::http