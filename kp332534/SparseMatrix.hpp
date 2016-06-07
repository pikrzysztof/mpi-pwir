//
// Created by krzysztof on 03.06.16.
//

#ifndef KP332534PWIR_SPARSEMATRIX_HPP
#define KP332534PWIR_SPARSEMATRIX_HPP

#include "Matrix.hpp"
#include <vector>
#include <boost/serialization/base_object.hpp>
#include <tuple>


class SparseMatrix : public Matrix {
private:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive &ar, const unsigned version);

	std::vector<size_t> values, column_indices, row_offsets;
	SparseMatrix();
	SparseMatrix(size_t nrows, size_t ncols);
	size_t getRowBeginning(size_t row_idx) const;
	size_t getRowEnd(size_t row_idx) const;
	friend std::ostream& operator<<(std::ostream& os, const SparseMatrix& sm);
public:
	SparseMatrix(std::istream &os);
	std::vector<SparseMatrix> columnDivide(size_t ncols);
	std::vector<SparseMatrix> rowDivide(size_t nrows_div);
	std::vector<SparseMatrix> colDivide(size_t ncols_div);
};

template<typename Archive>
void SparseMatrix::serialize(Archive &ar, const unsigned)
{
	ar & boost::serialization::base_object<Matrix>(*this);
	ar & this->values & this-> row_offsets & this->column_indices;
}

std::ostream & operator<<(std::ostream& out, SparseMatrix const& sm);
#endif //KP332534PWIR_SPARSEMATRIX_HPP

