/// @brief Файл содержит объявление методов для работы с данными пользователей
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/logic

#pragma once

#include <tsu_vps_service/core/service_logic/entities/user.h>

namespace service_logic::logic
{

/// @brief Класс для получения данных о конкретном пользователе
class GetUser
{
public:

    /// @brief Конструктор
    explicit GetUser() = default;

    /// @brief Получить данные о пользователе
    /// @param Идентификатор пользователя
    /// @return Данные о пользователе
    User operator()( const std::int32_t& id ) const;

};

/// @brief Класс для получения данных обо всех пользователях
class GetUserList
{
public:

    /// @brief Конструктор
    explicit GetUserList() = default;

    /// @brief Получить данные обо всех пользователях
    /// @return Данные обо всех пользователях
    std::vector< User > operator()() const;

};

/// @brief Класс для добавления данных пользователя
class SetUser
{
public:

    explicit SetUser() = default;

    /// @brief Добавить данные о пользователе
    /// @param Данные пользователя
    void operator()( const User& userData ) const;

};

} //namespace service_logic::logic