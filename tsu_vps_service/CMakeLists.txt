cmake_minimum_required(VERSION 3.6)

project(tsu_vps_service)

if(DEFINED ENV{DROGON_ROOT})
    list(APPEND CMAKE_PREFIX_PATH "$ENV{DROGON_ROOT}")
endif()

add_subdirectory( core )