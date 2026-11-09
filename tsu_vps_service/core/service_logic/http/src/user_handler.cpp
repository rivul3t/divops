/// @brief Файл содержит объявление методов для работы с данными пользователей
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/http/src

#include <tsu_vps_service/core/service_logic/http/user_handler.h>
#include <tsu_vps_service/core/service_logic/logic/user_handler.h>
#include <tsu_vps_service/core/helpers/exception.h>
#include <tsu_vps_service/core/helpers/logger.h>

namespace service_logic::http
{

void GetUser::operator()( const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback, std::uint32_t id ) const try
{
    logic::GetUser handler;
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
catch( utils::Exception& e )
{
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode( utils::ExceptionToStatusCode( e.GetType() ) );
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setBody(std::string("{\"error\": \"") + e.what() + "\"}");
    callback(resp);
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
catch( utils::Exception& e )
{
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode( utils::ExceptionToStatusCode( e.GetType() ) );
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setBody(std::string("{\"error\": \"") + e.what() + "\"}");
    callback(resp);
}

void SetUser::operator()( const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback ) const try
{
    auto json = request->getJsonObject();
    if( !json or json->empty() )
    {
        throw utils::Exception( utils::ExceptionType::WrongArgument, "Invalid JSON body" );
    }

    NewUser userInfo;
    ReadJson( *json, userInfo );
    logic::SetUser handler;
    handler( userInfo );

    auto response = drogon::HttpResponse::newHttpJsonResponse( *json );
    response->setStatusCode( drogon::k201Created );
    callback( response );
}
catch( utils::Exception& e )
{
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode( utils::ExceptionToStatusCode( e.GetType() ) );
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setBody(std::string("{\"error\": \"") + e.what() + "\"}");
    callback(resp);
}

} //namespace service_logic::http