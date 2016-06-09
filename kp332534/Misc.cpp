//
// Created by krzysztof on 08.06.16.
//

#include "Misc.hpp"
#include "Utils.hpp"
namespace bmpi = boost::mpi;
DenseMatrix* generateDenseMatrix(size_t rank,
                                size_t processes,
                                size_t size,
                                int seed)
{
	size_t rows_from = 0_z,
		rows_to = size,
		extra_cols = size % processes,
		my_extra_cols = std::min(rank, extra_cols),
		cols_per_proc = size / processes,
		cols_from = rank * cols_per_proc + my_extra_cols,
		cols_to = cols_from + cols_per_proc;
	if (rank < extra_cols) {
		++cols_to;
	}
	return (new DenseMatrix(cols_from, cols_to, rows_from, rows_to))->fillValues(seed);
}