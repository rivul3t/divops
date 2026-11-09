/// @brief
///
/// @file tsu_vps_service — tsu_vps_service/core/helpers/src

#pragma once

#include <exception>
#include <string>
#include <drogon/HttpTypes.h>

namespace utils
{

enum class ExceptionType
{
    InternalError = 0,
    WrongArgument = 1,
    Forbidden = 2,
    AlreadyExists = 3,
    DatabaseError = 4
};

drogon::HttpStatusCode ExceptionToStatusCode( const ExceptionType& type );

class Exception : public std::exception
{
public:

    explicit Exception( ExceptionType type, std::string  = "" );

    const char* what() const noexcept override;

    ExceptionType GetType() const noexcept;

private:
    ExceptionType type_;
    std::string message_;
};

[[noreturn]] void RethrowException( const utils::Exception& e );

} //namespace utils
