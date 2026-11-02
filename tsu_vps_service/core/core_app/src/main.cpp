/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/core_app/src

#include <drogon/drogon.h>

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
        drogon::app().run();
    }
    catch ( ... )
    {
        std::cerr << "Something went wrong";
        return -1;
    }
    return 0;
}