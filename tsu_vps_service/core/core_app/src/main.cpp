/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/core_app/src

#include <cstdlib>
#include <string>

#include <drogon/drogon.h>
#include <tsu_vps_service/core/helpers/logger.h>

#include "core/helpers/exception.h"

namespace
{
std::string Env( const char* key, const char* fallback )
{
    const char* value = std::getenv( key );
    return ( value && *value ) ? value : fallback;
}
}

int main( int argc, char* argv[] )
{
    try
    {
        drogon::app().createDbClient(
            "postgresql",
            Env( "DB_HOST", "127.0.0.1" ),
            static_cast< unsigned short >( std::stoi( Env( "DB_PORT", "5432" ) ) ),
            Env( "DB_NAME", "service_db" ),
            Env( "DB_USER", "postgres" ),
            Env( "DB_PASSWORD", "11111111" ),
            1
        );

        drogon::app().addListener( "0.0.0.0", 8080 );

        utils::Logger::Init( "tsu_vps_service", utils::LogLevel::Debug );

        APP_LOG_INFO( "tsu_service_app has started..." );
        drogon::app().run();
    }
    catch ( ... )
    {
        APP_LOG_ERROR( "tsu_service_app crashed..." );
        return -1;
    }

    APP_LOG_INFO( "tsu_service_app has successfully stopped..." );
    return 0;
}
