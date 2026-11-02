cmake_minimum_required(VERSION 3.6)

project(core_app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC "src/*.cpp")

add_executable( ${PROJECT_NAME} ${SRC} )

target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        Drogon::Drogon
        service_logic
)