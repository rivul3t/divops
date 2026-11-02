/// @brief Файл содержит реализацию методов для работы с данными виртуальных машин
///
/// @file tsu_vps_service — tsu_vps_service/core/service_logic/logic/src

#include <tsu_vps_service/core/service_logic/logic/vm_handler.h>
#include <drogon/drogon.h>

namespace service_logic::logic
{

std::optional< VirtualMachine > GetVM::operator()( const std::int32_t& id ) const
{
    auto db = drogon::app().getDbClient();

    auto sqlQuery = db->execSqlSync(
        "SELECT id, user_id, rate_id, address, password "
        "FROM virtual_machines WHERE id = $1;",
        id
    );

    if( sqlQuery.empty() )
    {
        return std::nullopt;
    }

    const auto& row = sqlQuery[ 0 ];
    std::string vmPassword = row[ "password" ].as< std::string >();

    VirtualMachine vm {
        .id_ = row[ "id" ].as< std::int32_t >(),
        .newVirtualMachine_ = {
            .userId_ = row[ "user_id" ].as< std::int32_t >(),
            .rateId_ = row[ "rate_id" ].as< std::int32_t >(),
            .address_ = row[ "address" ].as< std::string >(),
            .password_ = std::vector< std::uint8_t >( vmPassword.begin(), vmPassword.end() )
        }
    };
    return vm;
}

std::vector< VirtualMachine > GetVMList::operator()() const
{
    auto db = drogon::app().getDbClient();

    auto sqlQuery = db->execSqlSync(
        "SELECT id, user_id, rate_id, address, password "
        "FROM virtual_machines;"
    );

    std::vector< VirtualMachine > vmList;
    vmList.reserve( sqlQuery.size() );

    for( size_t i = 0; i < sqlQuery.size(); ++i )
    {
        const auto& row = sqlQuery[ i ];
        std::string vmPassword = row[ "password" ].as< std::string >();

        VirtualMachine vm {
            .id_ = row[ "id" ].as< std::int32_t >(),
            .newVirtualMachine_ = {
                .userId_ = row[ "user_id" ].as< std::int32_t >(),
                .rateId_ = row[ "rate_id" ].as< std::int32_t >(),
                .address_ = row[ "address" ].as< std::string >(),
                .password_ = std::vector< std::uint8_t >( vmPassword.begin(), vmPassword.end() )
            }
        };
        vmList.push_back( std::move( vm ) );
    }
    return vmList;
}

void SetVM::operator()( const NewVirtualMachine& vm ) const
{
    auto db = drogon::app().getDbClient();

    std::string passwordStr( vm.password_.begin(), vm.password_.end() );

    db->execSqlSync(
        "INSERT INTO virtual_machines ( user_id, rate_id, address, password ) "
        "VALUES ( $1, $2, $3, $4 );",
        vm.userId_,
        vm.rateId_,
        vm.address_,
        passwordStr
    );
}

bool DeleteVM::operator()( const std::int32_t& id ) const
{
    auto db = drogon::app().getDbClient();

    auto sqlQuery = db->execSqlSync(
        "DELETE FROM virtual_machines WHERE id = $1;",
        id
    );
    return sqlQuery.affectedRows() > 0;
}

} //namespace service_logic::logic