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
	size_t ncols, nrows, rows_from, rows_to, cols_from, cols_to, number;
public:
	Matrix(size_t cols, size_t rows);
	Matrix(std::basic_istream<char, std::char_traits<char>> &istream);

	bool operator<(const Matrix& rhs) const;
};

template<typename Archive>
void Matrix::serialize(Archive &ar, const unsigned)
{
	ar & ncols & nrows & rows_from & rows_to & cols_from & cols_to;
}


#endif //KP332534PWIR_MATRIX_HPP
