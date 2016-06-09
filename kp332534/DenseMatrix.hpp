//
// Created by krzysztof on 07.06.16.
//

#ifndef KP332534PWIR_DENSEMATRIX_HPP
#define KP332534PWIR_DENSEMATRIX_HPP


#include <vector>
#include "Matrix.hpp"
#include "SparseMatrix.hpp"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

class DenseMatrix : public Matrix {
private:
	//	values[i][j] - wartosc w i-tym wierszu i j-tej kolumnie
	std::vector<std::vector<precision_type>> values;

	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive &ar, const unsigned)
	{
		ar & boost::serialization::base_object<Matrix>(*this);
		ar & this->values;
	}

protected:
	virtual void print(std::ostream& out) const override;
public:

	DenseMatrix(size_t cols_from,
	            size_t cols_to,
	            size_t rows_from,
	            size_t rows_to);
	DenseMatrix* fillValues(int seed = 0);
	precision_type getValOrZero(const size_t row_idx, const size_t col_idx) const override;
	DenseMatrix* fillWithZeros();
	void mla(const SparseMatrix &a, const Matrix &b);
	DenseMatrix(std::vector<DenseMatrix>& matrices);
	std::vector<DenseMatrix> colDivide(const size_t num_matrices) const;
	~DenseMatrix(){};
	size_t countGreater(precision_type threshold);
	DenseMatrix(){};

};


#endif //KP332534PWIR_DENSEMATRIX_HPP
