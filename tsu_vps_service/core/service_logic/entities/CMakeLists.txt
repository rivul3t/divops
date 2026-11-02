project( service_logic_entities )

file(GLOB_RECURSE SRC src/*.cpp)

add_library( ${PROJECT_NAME} STATIC ${SRC})

target_link_libraries( ${PROJECT_NAME} PUBLIC
        Drogon::Drogon
        helpers_lib
)