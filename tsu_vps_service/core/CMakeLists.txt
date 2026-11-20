cmake_minimum_required(VERSION 3.6)

project( core )

find_package( Drogon REQUIRED )

add_subdirectory( helpers )
add_subdirectory( service_logic )
add_subdirectory( core_app )