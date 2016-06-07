//
// Created by krzysztof on 03.06.16.
//
#include <vector>
#include "SparseMatrix.hpp"
#include "Debug.hpp"
#include <boost/range/irange.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/serialization.hpp>
#include <math.h>
#include <tuple>
#include <sstream>


template<typename T>
void readIntoVector(std::vector<T>& v, std::istream& is, const size_t numElements)
{
	v.reserve(numElements);
	T tmp;
	for (size_t i : boost::irange(0UL, numElements)) {
		std::tie(std::ignore, i);
		is>>tmp;
		v.push_back(tmp);
	}
}

SparseMatrix::SparseMatrix() :
		Matrix(0, 0)
		{ }

SparseMatrix::SparseMatrix(std::istream &is):
	Matrix(is)
{
	size_t nnz, max_nnz_per_row;
	is>>nnz;
	is>>max_nnz_per_row;
	readIntoVector(this->values, is, nnz);
	readIntoVector(this->row_offsets, is, this->nrows + 1);
	readIntoVector(this->column_indices, is, nnz);
}

std::vector<SparseMatrix> SparseMatrix::rowDivide(size_t num_matrices)
{
	size_t nrows_div = this->nrows / num_matrices;
	auto result = std::vector<SparseMatrix>(num_matrices, SparseMatrix(nrows_div, this->ncols));
	size_t row_offset = 0,
		row_offset_val_offset = 0,
		extra_rows = this->nrows % nrows_div;
	for (const size_t i : boost::irange(0UL, result.size())) {
		result[i].number = i;
		result[i].cols_from = 0;
		result[i].cols_to = this->ncols;
	}
	for (const size_t i : boost::irange(0UL, extra_rows)) {
		result[i].rows_from = i * (nrows_div + 1);
		result[i].rows_to = result[i].rows_from + nrows_div + 1;
		result[i].nrows = result[i].rows_from - result[i].rows_to;
	}
	for (const size_t i : boost::irange(extra_rows, result.size())) {
		result[i].rows_from = (i * nrows_div) + extra_rows;
		result[i].rows_to = result[i].rows_from + nrows_div;
	}

//TODO zmienić at na []
	for (const size_t num_matrix : boost::irange(0UL, num_matrices)) {
		row_offset_val_offset = this->row_offsets.at(row_offset);
		for (size_t row : boost::irange(row_offset, row_offset + nrows_div)) {
			for (size_t val_idx : boost::irange(this->row_offsets.at(row), this->row_offsets.at(row + 1))) {
				result.at(num_matrix).values.push_back(this->values.at(val_idx));
				result.at(num_matrix).column_indices.push_back(this->column_indices.at(val_idx));
			}
			result.at(num_matrix).row_offsets.push_back(this->row_offsets.at(row) - row_offset_val_offset);
		}
		result.at(num_matrix).row_offsets.push_back(this->row_offsets.at(row_offset + nrows_div) - row_offset_val_offset);
		row_offset += nrows_div;
	}
	return result;
}

size_t SparseMatrix::getRowBeginning(size_t row_idx) const
{
	std::cout<<row_idx;
	assert(row_idx >= this->rows_from);
	assert(row_idx < this->rows_to);
	return this->row_offsets[row_idx];
}

size_t SparseMatrix::getRowEnd(size_t row_idx) const
{
	assert(row_idx >= this->rows_from);
	assert(row_idx < this->rows_to);
	return this->row_offsets[row_idx + 1];
}

SparseMatrix::SparseMatrix(size_t nrows, size_t ncols) : Matrix(ncols, nrows)
{
	this->row_offsets.reserve(nrows + 1UL);
}

std::vector<SparseMatrix> SparseMatrix::colDivide(size_t num_matrices)
{
	size_t ncols_div = this->ncols / num_matrices;
	auto result = std::vector<SparseMatrix>(num_matrices, SparseMatrix(nrows, ncols_div));
	size_t extra_cols = this->ncols % num_matrices;
	for (const size_t i : boost::irange(0UL, result.size())) {
		result[i].number = i;
		result[i].rows_from = 0;
		result[i].rows_to = this->nrows;
	}

	for (const size_t i : boost::irange(0UL, extra_cols)) {
		result[i].cols_from = i * (ncols_div + 1);
		result[i].cols_to = result[i].cols_from + ncols_div + 1;
		result[i].ncols = result[i].cols_to - result[i].cols_from;
	}

	for (const size_t i : boost::irange(extra_cols, result.size())) {
		result[i].cols_from = (i * ncols_div) + extra_cols;
		result[i].cols_to = result[i].cols_from + ncols_div;
	}
//TODO zmienić at na []
	for (size_t row : boost::irange(0UL, this->nrows)) {
		size_t it = this->getRowBeginning(row),
			end = this->getRowEnd(row),
			vec_it = 0;
		while (vec_it < result.size()) {
			if (it >= end) {
//				juz wszystkie wartosci w tym wierszu wypisalismy
				result[vec_it].row_offsets.push_back(result[vec_it].values.size());
				++vec_it;
			} else if (result[vec_it].cols_to <= this->column_indices[it]) {
//				nie wypisalem jeszcze wszystkiego, ale akurat ta podmacierz
//                              juz nie bedzie miala wiecej elementow w tym wierszu
				result[vec_it].row_offsets.push_back(result[vec_it].values.size());
				++vec_it;
			} else {
				result[vec_it].values.push_back(this->values[it]);
				result[vec_it].column_indices.push_back(this->column_indices[it]);
			}
		}
	}

	return result;
}

std::ostream &operator<<(std::ostream& out, SparseMatrix const& sm)
{
	for (size_t row : boost::irange(0UL, sm.nrows)) {
		size_t col_vec_idx = sm.getRowBeginning(row),
			end_row = sm.getRowEnd(row);
		for (size_t col : boost::irange(0UL, sm.ncols)) {
			if (col == col_vec_idx && col_vec_idx < end_row) {
				out<<col<<" ";
				++col_vec_idx;
			} else {
				if (col < sm.ncols - 1) {
					out << 0 << " ";
				} else {
					out << 0;
				}
			}
		}
		out<<std::endl;
	}
	return out;
}