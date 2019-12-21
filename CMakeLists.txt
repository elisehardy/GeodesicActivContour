cmake_minimum_required(VERSION 3.13)
project(activ_contour)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MY_PKG REQUIRED IMPORTED_TARGET MLV)

set(SRCS
        main.cpp
        Snake.cpp
        Point.cpp
        )
set(HEADERS
        Snake.hpp
        Point.cpp)


add_executable(activ_contour ${SRCS} ${HEADERS})

target_link_libraries(activ_contour PUBLIC
    PkgConfig::MY_PKG)
