project( service_logic_http )

file(GLOB_RECURSE SRC src/*.cpp)

add_library( ${PROJECT_NAME} STATIC ${SRC}
        rate_handler.h
        src/rate_handler.cpp
        vm_handler.h
        src/vm_handler.cpp)

target_link_libraries( ${PROJECT_NAME} PUBLIC
        service_logic_entities
        service_logic_logic
        Drogon::Drogon
)