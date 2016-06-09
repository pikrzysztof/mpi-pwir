//
// Created by krzysztof on 08.06.16.
//
#include <boost/mpi.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/range/irange.hpp>
#include <boost/mpi/request.hpp>
#include "Utils.hpp"
#include "Misc.hpp"
#include "impl.hpp"

namespace bmpi = boost::mpi;

//TODO wywalic shared_pointera z DenseMatrixB
//std::shared_ptr<DenseMatrix> colMultiply(int exponent,
//                                         SparseMatrix *sparse,
//                                         std::shared_ptr<DenseMatrix> matrixB,
//                                         bmpi::communicator comm,
//                                         int my_rank,
//                                         int comm_size);


SparseMatrix redistributeSparseColumns(size_t rank,
                                       size_t processes,
                                       SparseMatrix *sparse,
                                       bmpi::communicator comm)
{
	assert((sparse == nullptr) xor (rank == MASTER));
//	TODO -- wyciek pamięci sm podczas odbierania?
	SparseMatrix sm;
	if (rank == MASTER) {
		std::vector<SparseMatrix> matrices = sparse->colDivide(processes);
		delete sparse;
		bmpi::scatter(comm, matrices, sm, MASTER);
	} else {
		bmpi::scatter(comm, std::vector<SparseMatrix>(), sm, MASTER);
	}
	return sm;
}

//comm to komunikator, posrod ktorego trzeba zreplikowac
//SparseMatrix* replicateSparse(size_t rank, size_t processes, SparseMatrix *sparse, bmpi::communicator comm)
//{
//	(void) rank;
//	(void) processes;
//	(void) sparse;
//	(void) comm;
//	return nullptr;
//}

//DenseMatrix* inner(bmpi::communicator communicator,
//                                   size_t rank,
//                                   size_t processes,
//                                   SparseMatrix *sparse,
//                                   std::shared_ptr<DenseMatrix> matrixB,
//                                   int exponent,
//                   size_t repl_fact)
//{
//	(void) communicator; (void) rank; (void) processes; (void)sparse; (void) matrixB; (void) exponent;
//	return nullptr;
//}

//std::shared_ptr<DenseMatrix> blockedColumn(boost::mpi::communicator world,
//                           size_t rank,
//                           size_t processes,
//                           SparseMatrix *sparse,
//                           std::shared_ptr<DenseMatrix> matrixB,
//                           int exponent,
//                           size_t repl_fact)
//{
//
//
//	return colMultiply(exponent,
//	                   repl_sm,
//	                   matrixB,
//	                   horizontal,
//	                   horizontal_rank,
//	                   horizontal_size);
//}


////TODO wywalic shared_pointera z DenseMatrixB
DenseMatrix* colMultiply(int exponent,
                          SparseMatrix *sparse,
                          DenseMatrix* matrixB,
                          bmpi::communicator comm,
                          int my_rank,
                          int comm_size)
{
	DenseMatrix* matC(new DenseMatrix(matrixB->getCols_from(),
	                                  matrixB->getCols_to(),
	                                  matrixB->getRows_from(),
	                                  matrixB->getRows_to()));
	matC->fillWithZeros();
	int SEND_TO_RANK = (my_rank + 1) % comm_size,
		RECV_FROM_RANK = my_rank - 1,
		SHIFT_TAG = 1315613;
	if (RECV_FROM_RANK < 0) {
		RECV_FROM_RANK += comm_size;
	}
	const bool sending = comm_size > 1;
	for (size_t exp : boost::irange(0_z, (size_t) exponent)) {
		(void) exp;
		for (int shifts : boost::irange(0, comm_size)) {
			(void) shifts;
			if (sending) {
				SparseMatrix *recv = new SparseMatrix();
				bmpi::request a, b;
				a = comm.isend(SEND_TO_RANK, SHIFT_TAG, *sparse);
				b = comm.irecv(RECV_FROM_RANK, SHIFT_TAG, *recv);
				matC->mla(*sparse, *matrixB);
				a.wait();
				delete sparse;
				b.wait();
				sparse = recv;
			} else {
				matC->mla(*sparse, *matrixB);
			}
		}
	}
	delete sparse;
	delete matrixB;
	return matC;
}