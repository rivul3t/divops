/// @brief Файл содержит реализацию методов для работы с данными виртуальных машин
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/http/src

#include <tsu_vps_service/core/service_logic/http/vm_handler.h>
#include <tsu_vps_service/core/service_logic/logic/vm_handler.h>
#include <tsu_vps_service/core/service_logic/entities/virtual_machine.h>

namespace service_logic::http
{

void VMHandler::GetVM( const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback, std::uint32_t id ) const try
{
    logic::GetVM handler;
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
catch( ... )
{

}

void VMHandler::GetVMList( const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback ) const try
{
    logic::GetVMList handler;
    auto recordList = handler();

    Json::Value jsonArray( Json::arrayValue );
    for( const auto& vm : recordList )
    {
        Json::Value vmJson;
        WriteJson( vmJson, vm );
        jsonArray.append( std::move( vmJson ) );
    }

    auto response = drogon::HttpResponse::newHttpJsonResponse( jsonArray );
    response->setStatusCode( drogon::k200OK );
    callback( response );
}
catch( ... )
{

}

void VMHandler::SetVM( const drogon::HttpRequestPtr& request,
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

    NewVirtualMachine newVm;
    ReadJson( *jsonPtr, newVm );

    logic::SetVM handler;
    handler( newVm );

    auto response = drogon::HttpResponse::newHttpResponse();
    response->setStatusCode( drogon::k201Created );
    callback( response );
}
catch( ... )
{

}

void VMHandler::DeleteVM( const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback, std::uint32_t id ) const try
{
    logic::DeleteVM handler;
    bool isDeleted = handler( id );

    auto response = drogon::HttpResponse::newHttpResponse();
    response->setStatusCode( isDeleted ? drogon::k200OK : drogon::k404NotFound );
    callback( response );
}
catch( ... )
{

}

} // namespace service_logic::http