cmake_minimum_required(VERSION 3.6)

project(service_logic)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "src/*.cpp")

list(APPEND CMAKE_PREFIX_PATH "/home/nobahu/libs/drogon")
find_package( Drogon REQUIRED )

add_executable( service_logic main.cpp
        entities/src/user.cpp
        entities/src/user.cpp
        logic/user_handler.h
        logic/src/user_handler.cpp)

target_include_directories( service_logic PRIVATE
        ${CMAKE_SOURCE_DIR}/..
)

target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        Drogon::Drogon
)

add_subdirectory( entities )
add_subdirectory( logic )
add_subdirectory( http )
