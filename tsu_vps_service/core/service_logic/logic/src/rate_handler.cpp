/// @brief Файл содержит реализацию методов для работы с данными тарифов
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/logic/src

#include <tsu_vps_service/core/service_logic/logic/rate_handler.h>
#include <tsu_vps_service/core/helpers/logger.h>
#include <tsu_vps_service/core/helpers/exception.h>

#include <drogon/drogon.h>

namespace service_logic::logic
{

std::optional< Rate > GetRate::operator()( const std::uint32_t& id ) const try
{
    auto db = drogon::app().getDbClient();

    auto sqlQuery = db->execSqlSync(
        "SELECT name, ram_amount, hd_amount, price FROM service_rates WHERE id = $1; ",
        static_cast< int >( id )
    );

    if( sqlQuery.empty() )
    {
        return std::nullopt;
    }

    auto row = sqlQuery[ 0 ];
    Rate rate {
        .id_ = id,
        .rate_ = {
            .name_ = row[ "name" ].as< std::string >(),
            .ramAmount_ = row[ "ram_amount" ].as< std::uint32_t >(),
            .hdAmount_ = row[ "hd_amount" ].as< std::uint32_t >(),
            .price_ = row[ "price" ].as< float >(),
        }
    };

    return rate;
}
catch ( const drogon::orm::DrogonDbException& e )
{
    throw utils::Exception( utils::ExceptionType::DatabaseError );
}
catch ( const drogon::orm::DrogonDbException& e )
{
    APP_LOG_ERROR( fmt::format("Get db exception in method: {}", "service_logic::logic::GetRate" ) );
    throw utils::Exception( utils::ExceptionType::DatabaseError, e.base().what() );
}
catch( ... )
{
    APP_LOG_ERROR( fmt::format("Get exception in method: {}", "service_logic::logic::GetRate" ) );
    utils::RethrowException( utils::Exception( utils::ExceptionType::InternalError, "service_logic::logic::GetRate" ) );
}

std::vector< Rate > GetRateList::operator()() const try
{
    auto db = drogon::app().getDbClient();

    auto sqlQuery = db->execSqlSync(
        "SELECT id, name, ram_amount, hd_amount, price FROM service_rates; "
    );

    std::vector< Rate > rates;
    rates.reserve( sqlQuery.size() );
    for( size_t i = 0; i < sqlQuery.size(); ++i )
    {
        auto row = sqlQuery[ i ];
        Rate rate {
            .id_ = row[ "id" ].as< std::uint32_t >(),
            .rate_ = {
                .name_ = row[ "name" ].as< std::string >(),
                .ramAmount_ = row[ "ram_amount" ].as< std::uint32_t >(),
                .hdAmount_ = row[ "hd_amount" ].as< std::uint32_t >(),
                .price_ = row[ "price" ].as< float >(),
            }
        };

        rates.push_back( rate );
    }

    return rates;
}
catch ( const drogon::orm::DrogonDbException& e )
{
    APP_LOG_ERROR( fmt::format("Get exception in method: {}", "service_logic::logic::GetRateList" ) );
    throw utils::Exception( utils::ExceptionType::DatabaseError );
}
catch( ... )
{
    APP_LOG_ERROR( fmt::format("Get exception in method: {}", "service_logic::logic::GetRateList" ) );
    utils::RethrowException( utils::Exception( utils::ExceptionType::InternalError, "service_logic::logic::SetRate" ) );
}

void SetRate::operator()( const NewRate& rate ) const try
{
    auto db = drogon::app().getDbClient();

    auto sqlQuery = db->execSqlSync(
     "INSERT INTO service_rates ( name, ram_amount, hd_amount, price ) "
        "VALUES ( $1, $2, $3, $4 ); ",
        rate.name_,
        static_cast< int >( rate.ramAmount_ ),
        static_cast< int >( rate.hdAmount_ ),
        static_cast< int >( rate.price_ )
    );
}
catch ( const drogon::orm::DrogonDbException& e )
{
    APP_LOG_ERROR( fmt::format("Get exception in method: {}", "service_logic::logic::SetRate" ) );
    throw utils::Exception( utils::ExceptionType::DatabaseError, e.base().what() );
}
catch( ... )
{
    APP_LOG_ERROR( fmt::format("Get exception in method: {}", "service_logic::logic::SetRate" ) );
    utils::RethrowException( utils::Exception( utils::ExceptionType::InternalError, "service_logic::logic::SetRate" ) );
}

bool UpdateRate::operator()( const Rate& rate ) const try
{
    auto db = drogon::app().getDbClient();

    auto sqlQuery = db->execSqlSync(
     "UPDATE service_rates "
        "SET name = $1, ram_amount = $2, hd_amount = $3, price = $4 "
        "WHERE id = $5;",
        rate.rate_.name_,
        static_cast< int >( rate.rate_.ramAmount_ ),
        static_cast< int >( rate.rate_.hdAmount_ ),
        static_cast< int >( rate.rate_.price_ ),
        static_cast< int >( rate.id_ )
    );

    return sqlQuery.affectedRows() > 0;
}
catch ( const drogon::orm::DrogonDbException& e )
{
    APP_LOG_ERROR( fmt::format("Get db exception in method: {}", "service_logic::logic::UpdateRate" ) );
    throw utils::Exception( utils::ExceptionType::DatabaseError, e.base().what() );
}
catch( ... )
{
    APP_LOG_ERROR( fmt::format("Get exception in method: {}", "service_logic::logic::UpdateRate" ) );
    utils::RethrowException( utils::Exception( utils::ExceptionType::InternalError, "service_logic::logic::UpdateRate" ) );
}

bool DeleteRate::operator()( const std::uint32_t& id ) const try
{
    auto db = drogon::app().getDbClient();

    auto sqlQuery = db->execSqlSync(
     "DELETE FROM service_rates WHERE id = $1;",
        static_cast< int >( id )
    );

    return sqlQuery.affectedRows() > 0;
}
catch ( const drogon::orm::DrogonDbException& e )
{
    APP_LOG_ERROR( fmt::format("Get db exception in method: {}", "service_logic::logic::DeleteRate" ) );
    throw utils::Exception( utils::ExceptionType::DatabaseError, e.base().what() );
}
catch( ... )
{
    APP_LOG_ERROR( fmt::format("Get exception in method: {}", "service_logic::logic::DeleteRate" ) );
    utils::RethrowException( utils::Exception( utils::ExceptionType::InternalError, "service_logic::logic::DeleteRate" ) );
}

} //namespace service_logic::logic