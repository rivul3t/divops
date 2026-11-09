/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/helpers

#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <memory>
#include <spdlog/spdlog.h>
#include <fmt/format.h>

namespace utils
{

/// @brief Уровень логирования (соответствует уровням spdlog)
enum class LogLevel
{
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
    Critical = 5,
    Off = 6
};

/// @brief Класс для логгирования
class Logger
{
public:

    Logger() = default;
    ~Logger() = default;

    static void Init( const std::string& serviceName, const LogLevel& logLevel );

    static std::shared_ptr< spdlog::logger >& GetLogger();

private:

    static std::shared_ptr<spdlog::logger> spdLogger;
};

} //namespace utils

/// @brief Макросы для логирования
#define APP_LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(utils::Logger::GetLogger(), __VA_ARGS__)
#define APP_LOG_INFO(...)  SPDLOG_LOGGER_INFO(utils::Logger::GetLogger(), __VA_ARGS__)
#define APP_LOG_WARN(...)  SPDLOG_LOGGER_WARN(utils::Logger::GetLogger(), __VA_ARGS__)
#define APP_LOG_ERROR(...) SPDLOG_LOGGER_ERROR(utils::Logger::GetLogger(), __VA_ARGS__)