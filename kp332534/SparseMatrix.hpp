//
// Created by krzysztof on 03.06.16.
//

#ifndef KP332534PWIR_SPARSEMATRIX_HPP
#define KP332534PWIR_SPARSEMATRIX_HPP

#include "Matrix.hpp"
#include <vector>
#include <boost/serialization/base_object.hpp>
#include <tuple>
#include <boost/serialization/utility.hpp>


class SparseMatrix : public Matrix {
private:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive &ar, const unsigned version);

//values[i][k].second to wartosc w i-tym wierszu i values[i][k].first kolumnie
	std::vector<std::vector<std::pair<size_t, double>>> values;

	SparseMatrix();
	SparseMatrix(size_t cols_from, size_t cols_to, size_t rows_from, size_t rows_to);
	friend std::ostream& operator<<(std::ostream& os, const SparseMatrix& sm);
public:
	SparseMatrix(std::istream &os);
	std::vector<SparseMatrix> rowDivide(const size_t num_matrices) const;
	std::vector<SparseMatrix> colDivide(const size_t num_matrices) const;

	double getValOrZero(const size_t row_idx, const size_t col_idx) const;
	SparseMatrix(std::vector<SparseMatrix> const& vec);
};

template<typename Archive>
void SparseMatrix::serialize(Archive &ar, const unsigned)
{
	ar & boost::serialization::base_object<Matrix>(*this);
	ar & this->values;
}




std::ostream & operator<<(std::ostream& out, SparseMatrix const& sm);
#endif //KP332534PWIR_SPARSEMATRIX_HPP

