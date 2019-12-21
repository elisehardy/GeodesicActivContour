cmake_minimum_required(VERSION 3.13)
project(activ_contour)

set(SRCS
        main.cpp
        Snake.cpp
        Point.cpp
        )
set(HEADERS
        Snake.hpp
        Point.cpp)


add_executable(activ_contour ${SRCS} ${HEADERS})