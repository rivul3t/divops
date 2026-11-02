/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/http

#pragma once

#include <drogon/HttpController.h>

namespace service_logic::http
{

/// @brief Обработчик метода получения данных о конкретном пользователе
class GetUser : public drogon::HttpController< GetUser >
{
public:

    METHOD_LIST_BEGIN
    ADD_METHOD_TO( GetUser::operator(), "/logic/user/{1}", drogon::Get );
    METHOD_LIST_END

    void operator()( const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback, uint32_t userId ) const;

};

/// @brief Обработчик метода получения данных о всех пользователях
class GetUserList : public drogon::HttpController< GetUserList >
{
public:

    METHOD_LIST_BEGIN
    ADD_METHOD_TO( GetUserList::operator(), "/logic/user", drogon::Get );
    METHOD_LIST_END

    void operator()( const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback ) const;

};

/// @brief Обработчик метода добавления данных о пользователе
class SetUser : public drogon::HttpController< SetUser >
{
public:

    METHOD_LIST_BEGIN
    ADD_METHOD_TO( SetUser::operator(), "/logic/user", drogon::Post );
    METHOD_LIST_END

    void operator()( const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback ) const;
};

} //namespace service_logic::http