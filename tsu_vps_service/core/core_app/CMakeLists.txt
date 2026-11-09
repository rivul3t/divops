cmake_minimum_required(VERSION 3.6)

project(core_app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC "src/*.cpp")

add_executable( ${PROJECT_NAME} ${SRC} )

target_link_libraries( ${PROJECT_NAME}
        PRIVATE
        -Wl,--whole-archive
        service_logic_http
        service_logic_logic
        service_logic_entities
        -Wl,--no-whole-archive
        helpers_lib
        service_logic
)