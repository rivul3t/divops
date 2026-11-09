project( service_logic_logic )

file(GLOB_RECURSE SRC src/*.cpp)

add_library( ${PROJECT_NAME} STATIC ${SRC} )

target_link_libraries( ${PROJECT_NAME} PUBLIC
        helpers_lib
        service_logic_entities
        Drogon::Drogon
)