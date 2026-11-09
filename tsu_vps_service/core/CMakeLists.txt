cmake_minimum_required(VERSION 3.6)

project( core )

list(APPEND CMAKE_PREFIX_PATH "/home/nobahu/libs/drogon")
find_package( Drogon REQUIRED )

add_subdirectory( helpers )
add_subdirectory( service_logic )
add_subdirectory( core_app )