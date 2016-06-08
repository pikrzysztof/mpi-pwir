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
#include <iostream>
#include <stdlib.h>
#include "Utils.hpp"
#include "prettyprint.hpp"

static void guardVector(std::vector<std::pair<size_t, precision_type>>& vec, size_t max_index)
{
	const std::pair<size_t, precision_type> first = std::make_pair(0_z, 0.0),
		last = std::make_pair(max_index, 0.0);
	if (vec.size() == 0) {
		vec.push_back(first);
		vec.push_back(last);
	} else {
		if (vec.front().first != 0_z) {
			vec.insert(vec.begin(), first);
		}
		if (vec.back().first != max_index) {
			vec.push_back(last);
		}
	}
}

template<typename T>
static void readIntoVector(std::vector<T>& v, std::istream& is, const size_t numElements)
{
	v.reserve(numElements);
	T tmp;
	for (size_t i : boost::irange(0_z, numElements)) {
		std::tie(std::ignore, i);
		is>>tmp;
		v.push_back(tmp);
	}
}

SparseMatrix::SparseMatrix() :
		Matrix(0_z, 0_z, 0_z, 0_z),
                last_row_check(0_z),
                last_col_check(0_z)
{ }

SparseMatrix::SparseMatrix(std::istream &is):
	Matrix(is),
	last_row_check(0_z),
        last_col_check(0_z)
{
	size_t nnz, max_nnz_per_row;
	is>>nnz;
	is>>max_nnz_per_row;
	std::vector<precision_type> vals;
	std::vector<size_t> row_offsets,
		column_indices;
	readIntoVector(vals, is, nnz);
	readIntoVector(row_offsets, is, this->nrows() + 1);
	readIntoVector(column_indices, is, nnz);
	this->values.resize(this->nrows());
	const std::pair<size_t, precision_type> last_col = std::make_pair(this->ncols() - 1_z, 0.0);
	for (size_t row : boost::irange(0_z, row_offsets.size() - 1)) {
		for (size_t j : boost::irange(row_offsets[row], row_offsets[row + 1])) {
			std::pair<size_t, precision_type> para =
				std::make_pair(column_indices.at(j),
				               vals.at(j));
			this->values.at(row).push_back(para);
		}
//		wstawianie strażnika
		guardVector(values.at(row), this->ncols() - 1);
	}
}


std::vector<SparseMatrix> SparseMatrix::rowDivide(size_t num_matrices) const
{

	size_t nrows_div = this->nrows() / num_matrices;
	auto result = std::vector<SparseMatrix>();
	result.reserve(num_matrices);
	size_t extra_rows = this->nrows() % num_matrices,
		rows_from = 0,
		rows_to;
	for (const size_t i : boost::irange(0_z, extra_rows)) {
		std::tie(std::ignore, i);
		rows_to = rows_from + nrows_div + 1;
		result.push_back(SparseMatrix(this->getCols_from(), this->getCols_to(), rows_from, rows_to));

		for (size_t row : boost::irange(rows_from, rows_to)) {
			result.back().values[row - rows_from] = this->values[row];
		}
		rows_from = rows_to;
	}
	for (const size_t i : boost::irange(extra_rows, num_matrices)) {
		std::tie(std::ignore, i);
		rows_to = rows_from + nrows_div;
		result.push_back(SparseMatrix(this->getCols_from(), this->getCols_to(), rows_from, rows_to));
		for (size_t row : boost::irange(rows_from, rows_to)) {
			result.back().values[row - rows_from] = this->values[row];
		}
		rows_from = rows_to;
	}
	return result;

}

std::vector<SparseMatrix> SparseMatrix::colDivide(size_t num_matrices) const
{
	size_t ncols_div = this->ncols() / num_matrices;
	(void) ncols_div;
	auto result = std::vector<SparseMatrix>();
	result.reserve(num_matrices);
	size_t extra_cols = this->ncols() % num_matrices,
		cols_from = 0,
		cols_to;
	for (size_t i : boost::irange(0_z, extra_cols)) {
		std::tie(std::ignore, i);
		cols_to = cols_from + ncols_div + 1;
		result.push_back(SparseMatrix(cols_from, cols_to, this->getRows_from(), this->getRows_to()));
		cols_from = cols_to;
	}
	for (size_t i : boost::irange(extra_cols, num_matrices)) {
		std::tie(std::ignore, i);
		cols_to = cols_from + ncols_div;
		result.push_back(SparseMatrix(cols_from, cols_to, this->getRows_from(), this->getRows_to()));
		cols_from = cols_to;
	}
	for (size_t i : boost::irange(0_z, this->nrows())) {
		size_t smid = 0_z; //sparseMatrix id
		for (auto& val : this->values.at(i)) {
			while (val.first >= result[smid].getCols_to()) {
				++smid;
			}
			result[smid].values[i].
				push_back(std::make_pair(val.first - result[smid].getCols_from(),
				                         val.second));
		}
	}
	for (auto& sm : result) {
		for (auto& vec : sm.values) {
			guardVector(vec, sm.ncols() - 1);
		}
	}
	return result;
}

precision_type SparseMatrix::getValOrZero(const size_t row_idx, const size_t col_idx) const
{
	assert(row_idx < this->nrows());
	assert(col_idx < this->ncols());
	size_t &lcc = const_cast<size_t &>(this->last_col_check),
		&lrc = const_cast<size_t &>(this->last_row_check);
	if (lrc != row_idx) {
		lcc = 0;
		lrc = row_idx;
	}
	const std::vector<std::pair<size_t, precision_type>> *const row = &(this->values.at(row_idx));
	if (row->at(lcc).first > col_idx) {
		--lcc;
	}
	while (col_idx > row->at(lcc).first) {
		++lcc;
	}
	if (col_idx == row->at(lcc).first) {
		return row->at(this->last_col_check).second;
	} else {
		return 0.0;
	}
}

void SparseMatrix::print(std::ostream& out) const
{
	for (const size_t row : boost::irange(0_z, this->nrows())) {
		for (const size_t col : boost::irange(0_z, this->ncols())) {
			precision_type d = this->getValOrZero(row, col);
			if (debug) {
				if (d == 0.0) {
					out << "......";
				} else {
					out<<d;
				}
			} else {
				out<<d;
			}
			if (col == this->ncols() - 1) {
				out<<std::endl;
			} else {
				out<<"\t";
			}
		}
	}
}

SparseMatrix::SparseMatrix(size_t cols_from, size_t cols_to, size_t rows_from, size_t rows_to) :
	Matrix(cols_from, cols_to, rows_from, rows_to),
	values(rows_to - rows_from),
        last_row_check(0),
        last_col_check(0)
{
}


SparseMatrix::SparseMatrix(std::vector<SparseMatrix> const& vec):
	Matrix(vec.front().getCols_from(),
	       vec.back().getCols_to(),
	       vec.front().getRows_from(),
	       vec.back().getRows_to())
{

	assert(std::is_sorted(vec.begin(), vec.end()));
	assert(vec.size() > 0);
	bool column_join = false,
		row_join = false;
	if (vec.size() == 1) {
		this->values = vec[0].values;
	} else {
		if (vec[0].getCols_to() == vec[1].getCols_from()) {
			column_join = true;
		}
		if (vec[0].getRows_to() == vec[1].getRows_from()) {
			row_join = true;
		}
		assert(column_join xor row_join);
	}

	if (column_join) {
		values.resize(vec.back().getRows_to() - vec.front().getRows_from());
		const size_t num_submatrices = vec.size();
		for (const size_t i : boost::irange(0_z, num_submatrices)) {
			const size_t nrows = vec.at(i).nrows();
			for (const size_t row : boost::irange(0_z, nrows)) {
				for (const std::pair<size_t, precision_type> &para : vec.at(i).values.at(row)) {
					bool wrzuc = false;
					if (para.second == 0.0) {
						if ((i == 0 and para.first == 0)
						    or (i == num_submatrices - 1
						        and para.first == vec.at(i).ncols() - 1)) {
							wrzuc = true;
						}
					} else {
						wrzuc = true;
					}
					if (wrzuc) {
						this->values.at(row).push_back(
							std::make_pair(para.first +
								               vec.at(i).getCols_from() -
								               vec.at(0).getCols_from(),
							               para.second));
					}
				}
			}
		}
	}
	if (row_join) {
		for (const SparseMatrix& sm : vec) {
			this->values.insert(
				this->values.end(),
				sm.values.begin(),
				sm.values.end());
		}
	}
}

const std::vector<std::pair<size_t, precision_type>> &SparseMatrix::getRow(size_t rowId) const
{
	assert(rowId < this->nrows());
	return this->values.at(rowId);
}