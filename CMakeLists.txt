cmake_minimum_required(VERSION 3.16)
project(game-of-life CXX)

set(CMAKE_CXX_STANDARD 17)

if (EMSCRIPTEN OR PLATFORM STREQUAL "Web")
    set(PLATFORM "Web" CACHE STRING "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) 
    
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
    )
    FetchContent_MakeAvailable(raylib)

    add_executable(index main.cpp)
    target_link_libraries(index PRIVATE raylib)
    target_compile_definitions(index PRIVATE PLATFORM_WEB)

    set_target_properties(index PROPERTIES 
                          SUFFIX ".html" 
                          LINK_FLAGS "-s USE_GLFW=3 -s ASYNCIFY --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/shell.html"
                          )

    
else()
    find_package(raylib REQUIRED)
    add_executable(app main.cpp)
    target_link_libraries(app PRIVATE raylib)
endif()