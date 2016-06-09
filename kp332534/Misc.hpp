//
// Created by krzysztof on 08.06.16.
//

#ifndef KP332534PWIR_MISC_HPP
#define KP332534PWIR_MISC_HPP

#include <boost/mpi/communicator.hpp>
#include "DenseMatrix.hpp"

const int MASTER = 0;

DenseMatrix* generateDenseMatrix(size_t rank,
                                size_t processes,
                                size_t size,
                                int seed);



#endif //KP332534PWIR_MISC_HPP
