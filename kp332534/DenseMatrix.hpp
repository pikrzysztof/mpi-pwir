//
// Created by krzysztof on 07.06.16.
//

#ifndef KP332534PWIR_DENSEMATRIX_HPP
#define KP332534PWIR_DENSEMATRIX_HPP


#include <vector>
#include <boost/range/irange.hpp>
#include "Matrix.hpp"
#include "SparseMatrix.hpp"

class DenseMatrix : public Matrix {
private:
//	values[i][j] - wartosc w i-tym wierszu i j-tej kolumnie
	std::vector<std::vector<double>> values;

protected:
	virtual void print(std::ostream& out) const override;
public:

	DenseMatrix(size_t cols_from,
	            size_t cols_to,
	            size_t rows_from,
	            size_t rows_to);
	void fillValues(int seed = 0);
	double getValOrZero(const size_t row_idx, const size_t col_idx) const override;
	void fillWithZeros();
	void mla(const Matrix &a, const Matrix &b);
	DenseMatrix(std::vector<DenseMatrix>& matrices);
	std::vector<DenseMatrix> colDivide(const size_t num_matrices) const;
};

#endif //KP332534PWIR_DENSEMATRIX_HPP
