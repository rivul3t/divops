cmake_minimum_required(VERSION 3.6)

project(helpers_lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC src/*.cpp)

add_library(
        ${PROJECT_NAME}
        STATIC
        ${SRC}
)

target_include_directories( helpers_lib PUBLIC
        ${CMAKE_SOURCE_DIR}/..
)

target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        Drogon::Drogon
)