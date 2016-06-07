//
// Created by krzysztof on 03.06.16.
//

#include <istream>
#include "Matrix.hpp"

Matrix::Matrix(std::istream& istream):
	cols_from(0), rows_from(0)
{
	istream>>this->cols_to;
	istream>>this->rows_to;
}


bool Matrix::operator<(const Matrix& rhs) const
{
	if (this->rows_from == rhs.rows_from and this->rows_to == rhs.rows_to) {
		return this->cols_from < rhs.cols_from;
	} else {
		return this->rows_from < rhs.rows_from;
	}
}

size_t Matrix::nrows() const
{
	return this->rows_to - this->rows_from;
}

size_t Matrix::ncols() const
{
	return this->cols_to - this->cols_from;
}

Matrix::Matrix(size_t cols_from, size_t cols_to,
               size_t rows_from, size_t rows_to):
	cols_from(cols_from), cols_to(cols_to),
	rows_from(rows_from), rows_to(rows_to)
{
}