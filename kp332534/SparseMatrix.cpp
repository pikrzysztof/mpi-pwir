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
#include "Utils.hpp"
#include "prettyprint.hpp"

static void guardVector(std::vector<std::pair<size_t, double>>& vec, size_t max_index)
{
	const std::pair<size_t, double> first = std::make_pair(0_z, 0.0),
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
		Matrix(0_z, 0_z, 0_z, 0_z)
{ }

SparseMatrix::SparseMatrix(std::istream &is):
	Matrix(is)
{
	size_t nnz, max_nnz_per_row;
	is>>nnz;
	is>>max_nnz_per_row;
	std::vector<double> vals;
	std::vector<size_t> row_offsets,
		column_indices;
	readIntoVector(vals, is, nnz);
	readIntoVector(row_offsets, is, this->nrows() + 1);
	readIntoVector(column_indices, is, nnz);
	this->values.resize(this->nrows());
	const std::pair<size_t, double> last_col = std::make_pair(this->ncols() - 1_z, 0.0);
	for (size_t row : boost::irange(0_z, row_offsets.size() - 1)) {
		for (size_t j : boost::irange(row_offsets[row], row_offsets[row + 1])) {
			std::pair<size_t, double> para =
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
	size_t extra_rows = this->nrows() % nrows_div,
		rows_from = 0,
		rows_to;
	for (const size_t i : boost::irange(0_z, extra_rows)) {
		std::tie(std::ignore, i);
		rows_to = rows_from + nrows_div + 1;
		result.push_back(SparseMatrix(this->cols_from, this->cols_to, rows_from, rows_to));

		for (size_t row : boost::irange(rows_from, rows_to)) {
			result.back().values[row - rows_from] = this->values[row];
		}
		rows_from = rows_to;
	}
	for (const size_t i : boost::irange(extra_rows, num_matrices)) {
		std::tie(std::ignore, i);
		rows_to += nrows_div;
		result.push_back(SparseMatrix(this->cols_from, this->cols_to, rows_from, rows_to));
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
		result.push_back(SparseMatrix(cols_from, cols_to, this->rows_from, this->rows_to));
		cols_from = cols_to;
	}
	for (size_t i : boost::irange(extra_cols, num_matrices)) {
		std::tie(std::ignore, i);
		cols_to = cols_from + ncols_div;
		result.push_back(SparseMatrix(cols_from, cols_to, this->rows_from, this->rows_to));
		cols_from = cols_to;
	}
	for (size_t i : boost::irange(0_z, this->nrows())) {
		size_t smid = 0_z; //sparseMatrix id
		for (auto& val : this->values.at(i)) {
			while (val.first >= result[smid].cols_to) {
				++smid;
			}
			result[smid].values[i].
				push_back(std::make_pair(val.first - result[smid].cols_from,
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

double SparseMatrix::getValOrZero(const size_t row_idx, const size_t col_idx) const
{
	static size_t last_col_check, last_row_check;
	if (last_row_check != row_idx) {
		last_col_check = 0;
		last_row_check = row_idx;
	}
	const std::vector<std::pair<size_t, double>> *const row = &(this->values.at(row_idx));
	while (col_idx > row->at(last_col_check).first) {
		++last_col_check;
	}
	if (col_idx == row->at(last_col_check).first) {
		return row->at(last_col_check).second;
	} else {
		return 0.0;
	}
}

std::ostream &operator<<(std::ostream& out, SparseMatrix const& sm)
{
	out.precision(4);
	out<<std::fixed;
	for (const size_t row : boost::irange(0_z, sm.nrows())) {
		for (const size_t col : boost::irange(0_z, sm.ncols())) {
			if (debug) {
				double d = sm.getValOrZero(row, col);
				if (d == 0.0) {
					out << "......";
				} else {
					out<<d;
				}
			} else {
				out<<sm.getValOrZero(row, col);
			}
			if (col == sm.ncols() - 1) {
				out<<std::endl;
			} else {
				out<<" ";
			}
		}
	}
	return out;
}

SparseMatrix::SparseMatrix(size_t cols_from, size_t cols_to, size_t rows_from, size_t rows_to) :
	Matrix(cols_from, cols_to, rows_from, rows_to),
	values(rows_to - rows_from)
{
}


SparseMatrix::SparseMatrix(std::vector<SparseMatrix> const& vec):
	Matrix(vec.front().cols_from,
	       vec.back().cols_to,
	       vec.front().rows_from,
	       vec.back().rows_to)
{

	assert(std::is_sorted(vec.begin(), vec.end()));
	assert(vec.size() > 0);
	bool column_join = false,
		row_join = false;
	if (vec.size() == 1) {
		this->values = vec[0].values;
	} else {
		if (vec[0].cols_to == vec[1].cols_from) {
			column_join = true;
		}
		if (vec[0].rows_to == vec[1].rows_from) {
			row_join = true;
		}
		assert(column_join xor row_join);
	}

	if (column_join) {
		values.resize(vec.back().rows_to - vec.front().rows_from);
		const size_t num_submatrices = vec.size();
		for (const size_t i : boost::irange(0_z, num_submatrices)) {
			const size_t nrows = vec.at(i).nrows();
			for (const size_t row : boost::irange(0_z, nrows)) {
				for (const std::pair<size_t, double> &para : vec.at(i).values.at(row)) {
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
								               vec.at(i).cols_from -
								               vec.at(0).cols_from,
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
	for (auto& a: this->values) {
		std::cout<<a<<"\n";
	}
	std::cout<<this->cols_from<<" "<<this->cols_to<<" "<<this->rows_from<<" "<<this->rows_to<<"\n";
}