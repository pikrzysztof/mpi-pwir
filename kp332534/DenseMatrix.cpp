//
// Created by krzysztof on 07.06.16.
//

#include <iostream>
#include "DenseMatrix.hpp"
#include "densematgen.hpp"
#include "Utils.hpp"

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

void DenseMatrix::fillValues(int seed)
{
	for (const size_t row : boost::irange(0_z, this->nrows())) {
		for (const size_t col : boost::irange(0_z, this->ncols())) {
			this->values[row][col] =
				generate_double(seed,
				                (int) (row + this->getRows_from()),
				                (int) (col + this->getCols_from()));
		}
	}
}

double DenseMatrix::getValOrZero(size_t row_idx, size_t col_idx) const
{
	return this->values[row_idx][col_idx];
}

void DenseMatrix::fillWithZeros()
{
	for (const size_t row : boost::irange(0_z, this->nrows())) {
		for (const size_t col : boost::irange(0_z, this->ncols())) {
			(void) col;
			this->values[row].push_back(0.0);
		}
	}
}

//this = A * B, A is sparse, B is dense
void DenseMatrix::mla(const Matrix &a, const Matrix &b)//, double getValOrZero)
{
//	jestem w stanie policzyc taki prostokat o kolumnach [b.cols_from, b.cols_to)
//	i wierszach [a.rows_from, a.rows_to)
//	przy czym a[wierszA][kolumnaA] * b[wierszB][kolumnaB] idzie do
//	C[wierszA][kolumnaB]

	for (size_t row : boost::irange(0_z, a.nrows())) {
		for (size_t col : boost::irange(0_z, b.ncols())) {
//			std::cout<<"jade po rzadku "<<row<<" i kolumnie "<<col<<"\n";
			for (size_t i : boost::irange(0_z, a.ncols())) {
				double tmp = b.getValOrZero(i + a.getCols_from(), col) * a.getValOrZero(row, i);
//				std::cout<<" mla "<<row + a.getRows_from()<<" "<<col<<" += a["<<row<<", "<<i<<"] * b["<<i + a.getCols_from()
//				<<", "<<col<<"] = "<<tmp<<"bo a = "<<a.getValOrZero(row, i)<<", b="<<b.getValOrZero(i + a.getCols_from(), col)<<"\n";
				values[row + a.getRows_from()][col] +=
					b.getValOrZero(i + a.getCols_from(), col) * a.getValOrZero(row, i);
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