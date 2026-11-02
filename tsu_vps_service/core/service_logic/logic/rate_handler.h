/// @brief Файл содержит объявление методов для работы с данными тарифов
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/logic

#pragma once

#include <optional>
#include <tsu_vps_service/core/service_logic/entities/rate.h>

namespace service_logic::logic
{

/// @brief Класс для получения данных о конкретном тарифе
class GetRate
{
public:

    /// @brief Конструктор
    explicit GetRate() = default;

    /// @brief Получить данные о конкретном тарифе
    /// @param id Идентификатор тарифа
    /// @return Данные о тарифе или std::nullopt при его отсутствии
    std::optional< Rate > operator()( const std::uint32_t& id ) const;
};

/// @brief Класс для получения списка данных всех тарифов
class GetRateList
{
public:

    /// @brief Конструктор
    explicit GetRateList() = default;

    /// @brief Получить список данных всех тарифе
    /// @return Список данных всех тарифов
    std::vector< Rate > operator()() const;
};

/// @brief Класс для добавления данных о новом тарифе
class SetRate
{
public:

    /// @brief Конструктор
    explicit SetRate() = default;

    /// @brief Добавить данные о новом тарифе
    /// @param rate Данные нового тарифа
    void operator()( const NewRate& rate ) const;
};

/// @brief Класс для обновления данных существующего тарифа
class UpdateRate
{
public:

    /// @brief Конструктор
    explicit UpdateRate() = default;

    /// @brief Обновить данные тарифа
    /// @param rate Обновленные данные тарифа
    bool operator()( const Rate& rate ) const;
};

/// @brief Класс для удаления данных о тарифе
class DeleteRate
{
public:

    /// @brief Конструктор
    explicit DeleteRate() = default;

    /// @brief Удалить данные о тарифе
    /// @param id Идентификатор тарифа
    bool operator()( const std::uint32_t& id ) const;
};

} //namespace service_logic::logic