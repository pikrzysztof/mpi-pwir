//
// Created by krzysztof on 06.06.16.
//
#include <boost/archive/basic_text_iarchive.hpp>
#include "SparseMatrix.hpp"

int main()
{
	SparseMatrix sm = SparseMatrix(std::cin);
	std::cout<<sm;
	return 0;
}