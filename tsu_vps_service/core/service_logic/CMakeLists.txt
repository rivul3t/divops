cmake_minimum_required(VERSION 3.16)

project(service_logic)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon REQUIRED)

add_subdirectory(entities)
add_subdirectory(logic)
add_subdirectory(http)

add_library(${PROJECT_NAME} INTERFACE
        entities/src/rate.cpp)

target_link_libraries(${PROJECT_NAME} INTERFACE
        service_logic_entities
        service_logic_logic
        service_logic_http
)

target_include_directories(${PROJECT_NAME} INTERFACE
        ${CMAKE_SOURCE_DIR}
)
