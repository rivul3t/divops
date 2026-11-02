/// @brief Файл содержит объявление методов для работы с данными виртуальных машин
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/http

#pragma once

#include <drogon/HttpController.h>

namespace service_logic::http
{

class VMHandler : public drogon::HttpController< VMHandler >
{
public:

    METHOD_LIST_BEGIN
    ADD_METHOD_TO( VMHandler::GetVM, "/logic/vm/{1}", drogon::Get );
    ADD_METHOD_TO( VMHandler::GetVMList, "/logic/vm", drogon::Get );
    ADD_METHOD_TO( VMHandler::SetVM, "/logic/vm", drogon::Post );
    ADD_METHOD_TO( VMHandler::DeleteVM, "/logic/vm/{1}", drogon::Delete );
    METHOD_LIST_END

    void GetVM( const drogon::HttpRequestPtr& request,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                std::uint32_t id ) const;

    void GetVMList( const drogon::HttpRequestPtr& request,
                    std::function<void(const drogon::HttpResponsePtr &)> &&callback ) const;

    void SetVM( const drogon::HttpRequestPtr& request,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback ) const;

    void DeleteVM( const drogon::HttpRequestPtr& request,
                   std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                   std::uint32_t id ) const;
};

} // namespace service_logic::http