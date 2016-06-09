cmake_minimum_required (VERSION 2.8)
project(kp332534pwir)

set (EXECUTABLE_OUTPUT_PATH ".")

include_directories (${MPI_C_INCLUDE_PATH}
        SYSTEM)

set (Boost_LIBRARY_DIR /tmp/boost16/lib/)
set (Boost_INCLUDE_DIR /tmp/boost16/include/)
find_package(Boost COMPONENTS mpi serialization REQUIRED)
include_directories(${BOOST_INCLUDE_DIR})
link_directories(${BOOST_LIBRARY_DIR})

set (COMPILE_FLAGS "-Wall -Wextra -pedantic -std=gnu++11 -O0 -ggdb3 -DDBG -Wno-unused-but-set-variable")

set (zagraniczne_src
        prettyprint.hpp
        densematgen.hpp densematgen.cpp
        Debug.hpp
        Misc.hpp Misc.cpp
        Utils.hpp
        )

set (macierze_src
        Matrix.cpp Matrix.hpp
        SparseMatrix.cpp SparseMatrix.hpp
        DenseMatrix.cpp DenseMatrix.hpp
        )

add_library(zagraniczne ${zagraniczne_src})
add_library(macierze ${macierze_src})
set (moje_src
        impl.cpp impl.hpp
        )

add_library (moje_biblioteki STATIC ${moje_src})

set (CMAKE_CXX_FLAGS "-I/tmp/boost16/include ${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")
# set (CMAKE_LINK_LIBRARY_FLAG "${CMAKE_LINK_LIBRARY_FLAG}")

add_executable(matrixmul matrixmul.cpp ${moje_src})
#add_executable(testowanie test.cpp ${moje_src})
target_link_libraries(matrixmul
        zagraniczne
        macierze
        moje_biblioteki
        zagraniczne
        ${Boost_LIBRARIES}
        ${MPI_LIBRARIES}
        mpich
        )

#target_link_libraries(testowanie
#        zagraniczne
#        macierze
#        moje_biblioteki
#        zagraniczne
#        ${Boost_LIBRARIES}
#        ${MPI_LIBRARIES}
#        mpich
#        )
