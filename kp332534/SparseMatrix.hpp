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

//      values[i][k].second to wartosc w i-tym wierszu i values[i][k].first kolumnie
	std::vector<std::vector<std::pair<size_t, precision_type>>> values;
	size_t last_row_check, last_col_check;
	SparseMatrix();
	SparseMatrix(size_t cols_from, size_t cols_to, size_t rows_from, size_t rows_to);

protected:
	virtual void print(std::ostream& out) const override;

public:
	SparseMatrix(std::istream &os);
	std::vector<SparseMatrix> rowDivide(const size_t num_matrices) const;
	std::vector<SparseMatrix> colDivide(const size_t num_matrices) const;

	virtual precision_type getValOrZero(const size_t row_idx, const size_t col_idx) const override;
	SparseMatrix(std::vector<SparseMatrix> const& vec);
	const std::vector<std::pair<size_t, precision_type>>& getRow(size_t rowId) const;
};

template<typename Archive>
void SparseMatrix::serialize(Archive &ar, const unsigned)
{
	ar & boost::serialization::base_object<Matrix>(*this);
	ar & this->values;
}



#endif //KP332534PWIR_SPARSEMATRIX_HPP

