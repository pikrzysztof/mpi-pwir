//
// Created by krzysztof on 03.06.16.
//

#include <istream>
#include "Matrix.hpp"

Matrix::Matrix(size_t cols, size_t rows):
	ncols(cols),
	nrows(rows),
	rows_from(0),
	rows_to(rows),
	cols_from(0),
	cols_to(cols)
{}

Matrix::Matrix(std::istream& istream)
{
	istream>>this->nrows;
	istream>>this->ncols;
}


bool Matrix::operator<(const Matrix& rhs) const
{
	return this->number < rhs.number;
}