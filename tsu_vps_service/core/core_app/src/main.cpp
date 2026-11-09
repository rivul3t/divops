/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/core_app/src

#include <drogon/drogon.h>
#include <tsu_vps_service/core/helpers/logger.h>

#include "core/helpers/exception.h"

int main( int argc, char* argv[] )
{
    try
    {
        drogon::app().createDbClient(
            "postgresql",
            "127.0.0.1",
            5432,
            "service_db",
            "postgres",
            "11111111",
            1
        );

        drogon::app().addListener( "0.0.0.0", 8080 );

        utils::Logger::Init( "tsu_vps_service", utils::LogLevel::Debug );

        APP_LOG_INFO( "tsu_service_app has started..." );
        drogon::app().run();
    }
    catch ( utils::Exception& e )
    {
        APP_LOG_ERROR( "tsu_service_app crashed..." );
        return -1;
    }

    APP_LOG_INFO( "tsu_service_app has successfully stopped..." );
    return 0;
}
