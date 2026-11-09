cmake_minimum_required(VERSION 3.6)

project(helpers_lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC src/*.cpp)

list(APPEND CMAKE_PREFIX_PATH "/home/nobahu/libs/drogon")
find_package( spdlog REQUIRED )

add_library( ${PROJECT_NAME}
        STATIC
        ${SRC}
)

target_include_directories( helpers_lib PUBLIC
        ${CMAKE_SOURCE_DIR}/..
)

target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        Drogon::Drogon
        spdlog::spdlog
)