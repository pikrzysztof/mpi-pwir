cmake_minimum_required (VERSION 2.8)
project(kp332534pwir)


set (MPI_CXX_COMPILER /opt/mpich2/bin/mpicxx)
set (MPI_C_COMPIER /opt/mpich2/bin/mpicc)
set (MPI_C_LIBRARIES /opt/mpich2)
set (MPI_C_INCLUDE_PATH /opt/mpich2/include)
find_package(MPI REQUIRED)

include_directories (${MPI_C_INCLUDE_PATH}
        ${Boost_INCLUDE_PATH}
        SYSTEM)



set (EXECUTABLE_OUTPUT_PATH ".")
find_package (Boost REQUIRED serialization)

set (COMPILE_FLAGS "-Wall -Wextra -pedantic -std=gnu++11 -O0 -ggdb3 -DDEBUG -Wno-unused-but-set-variable")


set (moje
        Matrix.cpp Matrix.hpp
        SparseMatrix.cpp SparseMatrix.hpp
        Utils.hpp prettyprint.hpp
        DenseMatrix.cpp DenseMatrix.hpp
        Debug.hpp
        densematgen.hpp densematgen.cpp)
add_library (moje_biblioteki STATIC ${moje})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")
# set (CMAKE_LINK_LIBRARY_FLAG "${CMAKE_LINK_LIBRARY_FLAG}")

add_executable(matrixmul matrixmul.cpp)
add_executable(testowanie test.cpp)
target_link_libraries(matrixmul
        moje_biblioteki
        /opt/mpich2/lib/libmpich.so
        ${Boost_LIBRARIES})

target_link_libraries(testowanie
        moje_biblioteki
        /opt/mpich2/lib/libmpich.so
        ${Boost_LIBRARIES})