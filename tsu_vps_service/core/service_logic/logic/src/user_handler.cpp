/// @brief Файл содержит объявление методов для работы с данными пользователей
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/logic/src

#include <tsu_vps_service/core/service_logic/logic/user_handler.h>
#include <tsu_vps_service/core/helpers/utils.h>
#include <drogon/drogon.h>

namespace service_logic::logic
{

std::optional< User > GetUser::operator()( const std::int32_t& id ) const try
{
    auto db = drogon::app().getDbClient();

    auto sqlQuery  = db->execSqlSync(
     "SELECT id, email, password, role,"
        "(EXTRACT(EPOCH FROM created_at) * 1000)::bigint AS created_at, "
        "(EXTRACT(EPOCH FROM updated_at) * 1000)::bigint AS updated_at "
        "FROM user_data WHERE id = $1; ",
        static_cast< int >( id )
    );

    if ( sqlQuery.empty() )
    {
        return std::nullopt;
    }

    auto row = sqlQuery[ 0 ];
    std::string userPassword = row[ "password" ].as< std::string >();

    User user {
        .user_ = {
            .email_ = row[ "email" ].as< std::string >(),
            .password_ = std::vector< uint8_t >( userPassword.begin(), userPassword.end() ),
            .role_ = StringToRole( row[ "role" ].as< std::string >() ),
            .creationDate_ = utils::FromEpochMs( row[ "created_at" ].as< std::int64_t >() ),
            .updatedDate_ = utils::FromEpochMs( row[ "updated_at" ].as< std::int64_t >() )
        },
        .id_ = id
    };

    return user;
}
catch( ... )
{

}

std::vector< User > GetUserList::operator()() const try
{
    auto db = drogon::app().getDbClient();

    auto sqlQuery = db->execSqlSync(
     "SELECT id, email, password, role, "
        "(EXTRACT(EPOCH FROM created_at) * 1000)::bigint AS created_at, "
        "(EXTRACT(EPOCH FROM updated_at) * 1000)::bigint AS updated_at "
        "FROM user_data; "
    );

    std::vector< User > userList;
    userList.reserve( sqlQuery.size() );
    for( size_t i = 0; i < sqlQuery.size(); ++i )
    {
        auto row = sqlQuery[ i ];
        std::string userPassword = row[ "password" ].as< std::string >();

        User user {
            .user_ = {
                .email_ = row[ "email" ].as< std::string >(),
                .password_ = std::vector< uint8_t >( userPassword.begin(), userPassword.end() ),
                .role_ = StringToRole( row[ "role" ].as< std::string >() ),
                .creationDate_ = utils::FromEpochMs( row[ "created_at" ].as< std::int64_t >() ),
                .updatedDate_ = utils::FromEpochMs( row[ "updated_at" ].as< std::int64_t >() )
            },
            .id_ = row[ "id" ].as< std::int32_t >()
        };
        userList.push_back( user );
    }

    return userList;
}
catch( ... )
{

}

void SetUser::operator()( NewUser& userData ) const try
{
    auto db = drogon::app().getDbClient();

    userData.updatedDate_ = userData.creationDate_ = std::chrono::system_clock::now();

    auto sqlQuery = db->execSqlSync(
     "INSERT INTO user_data (email, password, role, created_at, updated_at) "
        "VALUES ($1, $2, $3, $4, $5)",
        userData.email_, utils::Base64Decode( userData.password_ ), RoleToString( userData.role_ ),
        utils::ToEpochMs( userData.creationDate_ ), utils::ToEpochMs( userData.updatedDate_ )
    );
}
catch( ... )
{

}

} //namespace service_logic::logic