/// @brief Файл содержит объявление методов для работы с данными виртуальных машин
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/logic

#pragma once

#include <tsu_vps_service/core/service_logic/entities/virtual_machine.h>
#include <optional>

namespace service_logic::logic
{

/// @brief Класс для получения данных о конкретной виртуальной машине
class GetVM
{
public:

    /// @brief Конструктор
    explicit GetVM() = default;

    /// @brief Получить данные о конкретной виртуальной машине
    /// @param id Идентификатор ВМ
    /// @return Данные о ВМ или std::nullopt при ее отсутствии
    std::optional< VirtualMachine > operator()( const std::int32_t& id ) const;
};

/// @brief Класс для получения списка данных всех виртуальных машин
class GetVMList
{
public:

    /// @brief Конструктор
    explicit GetVMList() = default;

    /// @brief Получить данные о всех виртуальных машинах
    /// @return Список данных всех ВМ
    std::vector< VirtualMachine > operator()() const;
};

/// @brief Класс для добавления данных о новой виртуальной машине
class SetVM
{
public:

    /// @brief Конструктор
    explicit SetVM() = default;

    /// @brief Добавить данные о новой виртуальной машине
    /// @param vm Данные о новой ВМ
    void operator()( const NewVirtualMachine& vm ) const;
};

/// @brief Класс для удаления данных о виртуальной машине
class DeleteVM
{
public:

    /// @brief Конструктор
    explicit DeleteVM() = default;

    /// @brief Удалить данные о виртуальной машине
    /// @param id Идентификатор ВМ
    bool operator()( const std::int32_t& id ) const;
};

} //namespace service_logic::logic