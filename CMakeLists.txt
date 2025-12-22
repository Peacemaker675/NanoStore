cmake_minimum_required(VERSION 3.10)

project(Nanostore VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
find_package(Threads REQUIRED)
include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(Nanostore ${SOURCES})
target_link_libraries(Nanostore PRIVATE Threads::Threads)