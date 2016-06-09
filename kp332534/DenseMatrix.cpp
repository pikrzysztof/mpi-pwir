//
// Created by krzysztof on 07.06.16.
//

#include <iostream>
#include "DenseMatrix.hpp"
#include "densematgen.hpp"
#include "Utils.hpp"
#include <boost/range/irange.hpp>

DenseMatrix::DenseMatrix(size_t cols_from,
                         size_t cols_to,
                         size_t rows_from,
                         size_t rows_to) : Matrix(cols_from, cols_to, rows_from, rows_to),
                                           values(rows_to - rows_from)
{
	size_t siz = this->ncols();
	for (size_t i : boost::irange(0_z, this->nrows())) {
		values[i].reserve(siz);
	}
}

DenseMatrix* DenseMatrix::fillValues(int seed)
{
	for (const size_t row : boost::irange(0_z, this->nrows())) {
		for (const size_t col : boost::irange(0_z, this->ncols())) {
			this->values[row][col] =
				generate_double(seed,
				                (int) (row + this->getRows_from()),
				                (int) (col + this->getCols_from()));
		}
	}
	return this;
}

precision_type DenseMatrix::getValOrZero(size_t row_idx, size_t col_idx) const
{
	return this->values[row_idx][col_idx];
}

DenseMatrix* DenseMatrix::fillWithZeros()
{
	for (const size_t row : boost::irange(0_z, this->nrows())) {
		for (const size_t col : boost::irange(0_z, this->ncols())) {
			(void) col;
			this->values[row].push_back(0.0);
		}
	}
	return this;
}

//this = A * B, A is sparse, B is dense
void DenseMatrix::mla(const SparseMatrix &a, const Matrix &b)//, precision_type getValOrZero)
{
//	jestem w stanie policzyc taki prostokat o kolumnach [b.cols_from, b.cols_to)
//	i wierszach [a.rows_from, a.rows_to)
//	przy czym a[wierszA][kolumnaA] * b[wierszB][kolumnaB] idzie do
//	C[wierszA][kolumnaB]

//TODO: zoptymalizowac odwolania do pamieci
	for (const size_t rowIdx : boost::irange(0_z, a.nrows())) {
		const std::vector<std::pair<size_t, precision_type>>& row = a.getRow(rowIdx);
		std::vector<precision_type>& c_row = this->values[rowIdx + a.getRows_from() - this->getRows_from()];
		for (const auto& val_a : row) {
			for (const size_t col : boost::irange(0_z, b.ncols())) {
				c_row[col] += val_a.second * b.getValOrZero(val_a.first + a.getCols_from(), col);
			}
		}
	}
}

#include "prettyprint.hpp"
void DenseMatrix::print(std::ostream& os) const
{
	const size_t ncols = this->ncols();
	for (size_t row : boost::irange(0_z, this->nrows())) {
		for (size_t col : boost::irange(0_z, ncols)) {
			os<<this->values[row][col];
			if (col == ncols - 1) {
				os<<"\n";
			} else {
				os<<"\t";
			}
		}
	}
}

DenseMatrix::DenseMatrix(std::vector<DenseMatrix>& matrices):
	Matrix(matrices.front().getCols_from(),
	matrices.back().getCols_to(),
	matrices.front().getRows_from(),
	matrices.back().getRows_to()),
        values(matrices.front().nrows())
{
	assert(matrices.size() > 0);
	assert(std::is_sorted(matrices.begin(), matrices.end()));
	assert(matrices.front().getRows_from() == matrices.back().getRows_from());
	assert(matrices.front().getRows_from() == matrices.back().getRows_from());
	size_t ncols =  matrices.back().getCols_to() - matrices.front().getCols_from();
	if (matrices.size() == 1){
		values = matrices.front().values;
	} else {
		for (std::vector<precision_type>& vec : this->values) {
			vec.reserve(ncols);
		}
		for (const DenseMatrix& dm : matrices) {
			for (size_t row : boost::irange(0_z, dm.nrows())) {
				for (size_t col : boost::irange(0_z, dm.ncols())) {
					values.at(row).push_back(dm.getValOrZero(row, col));
				}
			}
		}
	}
}

std::vector<DenseMatrix> DenseMatrix::colDivide(const size_t num_matrices) const
{
	std::vector<DenseMatrix> result;
	result.reserve(num_matrices);
	const size_t extra_cols = this->ncols() % num_matrices,
		ncols_div = this->ncols() / num_matrices;
	size_t cols_from = this->getCols_from(),
		cols_to;
	for (size_t i : boost::irange(0_z, extra_cols)) {
		(void) i;
		cols_to = cols_from + ncols_div + 1;
		result.emplace_back(cols_from, cols_to, this->getRows_from(), this->getRows_to());
		cols_from = cols_to;
	}
	for (size_t i : boost::irange(extra_cols, num_matrices)) {
		(void) i;
		cols_to = cols_from + ncols_div;
		result.emplace_back(cols_from, cols_to, this->getRows_from(), this->getRows_to());
		cols_from = cols_to;
	}
	for (DenseMatrix& dm : result) {
		for (size_t row : boost::irange(0_z, dm.nrows())) {
			for (size_t col : boost::irange(0_z, dm.ncols())) {
				dm.values.at(row).push_back(
					this->getValOrZero(row,
					                   col + dm.getCols_from() - this->getCols_from()));
			}
		}
	}
	return result;
}

size_t DenseMatrix::countGreater(precision_type threshold)
{
	size_t result = 0;
	for (std::vector<precision_type> &row : this->values) {
		for (precision_type val : row) {
			if (val >= threshold) {
				result += 1;
			}
		}
	}
	return result;
}





