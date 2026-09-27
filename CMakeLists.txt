cmake_minimum_required(VERSION 3.16)
project(game-of-life CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(raylib REQUIRED)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE raylib)