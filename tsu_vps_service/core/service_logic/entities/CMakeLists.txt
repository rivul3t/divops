project( entities )

add_library( entities STATIC
        src/user.cpp
)

target_include_directories( entities PUBLIC
        ${CMAKE_SOURCE_DIR}/..
)

target_link_libraries( entities PUBLIC
        Drogon::Drogon
)