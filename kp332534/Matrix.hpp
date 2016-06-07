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

class Matrix {
private:


	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive &ar, const unsigned version);

protected:
	size_t cols_from, cols_to, rows_from, rows_to;
	size_t nrows() const;
	size_t ncols() const;
	Matrix(size_t cols_from, size_t cols_to,
	       size_t rows_from, size_t rows_to);

public:
	Matrix(std::istream &istream);

	bool operator<(const Matrix& rhs) const;
};

template<typename Archive>
void Matrix::serialize(Archive &ar, const unsigned)
{
	ar &  rows_from & rows_to & cols_from & cols_to;
}




#endif //KP332534PWIR_MATRIX_HPP
