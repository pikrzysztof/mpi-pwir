cmake_minimum_required (VERSION 2.6)
project(kp332534pwir)

set (GENS densematgen.cpp)
add_library(generatory STATIC ${GENS})
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
set (EXECUTABLE_OUTPUT_PATH ".")

set (COMPILE_FLAGS "-Wall -Wextra -pedantic -std=gnu++11")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")

add_executable(matrixmul matrixmul.cpp)
target_link_libraries(matrixmul generatory ${MPI_LIBRARIES})
