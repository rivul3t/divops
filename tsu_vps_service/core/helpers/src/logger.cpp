/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/helpers/src

#include <tsu_vps_service/core/helpers/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace utils
{

spdlog::level::level_enum GetSpdLogLevel( const LogLevel& logLevel )
{
    return static_cast< spdlog::level::level_enum >( logLevel );
}

std::shared_ptr<spdlog::logger> Logger::spdLogger;

void Logger::Init( const std::string& serviceName, const LogLevel& logLevel )
{
    auto console_sink = std::make_shared< spdlog::sinks::stdout_color_sink_mt >();

    std::vector<spdlog::sink_ptr> sinks { console_sink };
    spdLogger = std::make_shared< spdlog::logger >( serviceName, sinks.begin(), sinks.end() );

    spdlog::register_logger(spdLogger);
    spdlog::set_default_logger(spdLogger);

    // Паттерн (формат вывода)
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s] %v");

    spdlog::set_level( GetSpdLogLevel( logLevel ) );
    spdLogger->flush_on( spdlog::level::info );
}

std::shared_ptr< spdlog::logger >& Logger::GetLogger()
{
    return spdLogger;
}

} //namespace utils