cmake_minimum_required(VERSION 3.10)

project(Nanostore VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
# -O3: Maximum optimization (loop unrolling, inlining)
# -march=native: cpu native instructions
# -Wall: Show warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O3 -march=native -Wall -pthread)
endif()
find_package(Threads REQUIRED)
include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(Nanostore ${SOURCES})
target_link_libraries(Nanostore PRIVATE Threads::Threads)