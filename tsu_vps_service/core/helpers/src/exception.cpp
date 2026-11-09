/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/helpers/src

#include <tsu_vps_service/core/helpers/exception.h>
#include <tsu_vps_service/core/helpers/logger.h>

namespace utils
{

drogon::HttpStatusCode ExceptionToStatusCode( const ExceptionType& type )
{
    drogon::HttpStatusCode httpCode;
    switch ( type ) {
        case ExceptionType::WrongArgument:
            httpCode = drogon::k400BadRequest;
            break;
        case ExceptionType::Forbidden:
            httpCode = drogon::k403Forbidden;
            break;
        case ExceptionType::AlreadyExists:
            httpCode = drogon::k409Conflict;
            break;
        case ExceptionType::DatabaseError:
            [[fallthrough]];
        case ExceptionType::InternalError:
            [[fallthrough]];
        default:
            httpCode = drogon::k500InternalServerError;
            break;
    }

    return httpCode;
}

Exception::Exception( ExceptionType type, std::string message ) : type_( type ), message_( std::move( message ) )
{

}

const char* Exception::what() const noexcept
{
    return message_.c_str();
}

ExceptionType Exception::GetType() const noexcept
{
    return type_;
}

[[noreturn]] void RethrowException( const utils::Exception& e )
{
    APP_LOG_ERROR( fmt::format( "Exception [Type: {}]: {}", static_cast< int >( e.GetType() ), e.what() ) );
    throw e;
}

} //namespace utils