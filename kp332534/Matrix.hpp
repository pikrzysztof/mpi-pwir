//
// Created by krzysztof on 03.06.16.
//

#ifndef KP332534PWIR_MATRIX_HPP
#define KP332534PWIR_MATRIX_HPP


#include <cstddef>
#include <istream>
#include <boost/serialization/access.hpp>
#include <tuple>
#include <ostream>
#include "Debug.hpp"

class Matrix {
private:
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive &ar, const unsigned version);
	size_t cols_from, cols_to, rows_from, rows_to;
protected:
	virtual void print(std::ostream& out) const = 0;
	Matrix(size_t cols_from, size_t cols_to,
	       size_t rows_from, size_t rows_to);
public:
	size_t nrows() const;
	size_t ncols() const;
	Matrix(std::istream &istream);
	virtual double getValOrZero(const size_t row_idx, const size_t col_idx) const = 0;
	bool operator<(const Matrix& rhs) const;

	inline size_t getCols_from() const { return cols_from; }

	inline size_t getCols_to() const { return cols_to; }

	inline size_t getRows_from() const { return rows_from; }

	inline size_t getRows_to() const { return rows_to; }

	friend std::ostream& operator<<(std::ostream& str, Matrix const& matrix)
	{
		str.precision(precision);
		if (fixed) {
			str<<std::fixed;
		}
		matrix.print(str);
		return str;
	}
};

template<typename Archive>
void Matrix::serialize(Archive &ar, const unsigned)
{
	ar &  rows_from & rows_to & cols_from & cols_to;
}



#endif //KP332534PWIR_MATRIX_HPP
