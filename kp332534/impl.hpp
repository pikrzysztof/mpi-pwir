//
// Created by krzysztof on 08.06.16.
//

#ifndef KP332534PWIR_IMPL_HPP
#define KP332534PWIR_IMPL_HPP


#include "DenseMatrix.hpp"

//DenseMatrix* inner(boost::mpi::communicator communicator,
//                   size_t rank,
//                   size_t processes,
//                   SparseMatrix* sparse,
//                   std::shared_ptr<DenseMatrix> matrixB,
//                   int exponent,
//                   size_t repl_fact
//);

SparseMatrix redistributeSparseColumns(size_t rank,
                                       size_t processes,
                                       SparseMatrix *sparse,
                                       boost::mpi::communicator comm);

DenseMatrix* colMultiply(int exponent,
                          SparseMatrix *sparse,
                          DenseMatrix* matrixB,
                          boost::mpi::communicator comm,
                          int my_rank,
                          int comm_size);

#endif //KP332534PWIR_IMPL_HPP
