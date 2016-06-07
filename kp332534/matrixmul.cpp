#include <cstdio>
#include <stdlib.h>
#include <mpi.h>
#include <cassert>
#include <getopt.h>

#include "densematgen.hpp"
#include "SparseMatrix.hpp"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

typedef void *sparse_type;

class a {
private:
	std::vector<int> x;
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive &ar, const unsigned version) {
		ar & x;
	}
public:
	a(): x({0}) {}
	a(int aliczba): x({aliczba, 102}) {}
	int get() {
		std::cout<<this->x[1];
		return this->x[0];
	}
};

class b: public a {
//	int x;
	friend class boost::serialization::access;
	template<typename Archive>
		void serialize(Archive &ar, const unsigned version) {
		ar & boost::serialization::base_object<a>(*this);

	}

public:
	b(int val): a(val) {}
};

bool test = true;

int main(int argc, char *argv[]) {
	if (test) {

		SparseMatrix sm = SparseMatrix(std::cin);
		boost::archive::text_oarchive ar(std::cout);
		b baca = b(100);
		b baca2 = b(105);
		ar & sm;
		//boost::archive::binary_iarchive ar2(std::cin);//, boost::archive::archive_flags::no_header);
		//ar2 & baca2;
		//std::cout<<baca2.get()<<std::endl;
//		std::vector<int> v2;
//		ar2 & v2;
//		for (auto i = v2.begin(); i != v2.end(); ++i) {
//			std::cout<<*i<<std::endl;
//		}
//		ar & sm;
	}

	if (test) {
		return 0;
	}
	int show_results = 0;
	int use_inner = 0;
	int gen_seed = -1;
	int repl_fact = 1;

	int option = -1;

	double comm_start = 0,
		comm_end = 0, comp_start = 0, comp_end = 0;
	int num_processes = 1;
	int mpi_rank = 0;
	int exponent = 1;
	double ge_element = 0;
	int count_ge = 0;

	sparse_type sparse = NULL;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

	while ((option = getopt(argc, argv, "vis:f:c:e:g:")) != -1) {
		switch (option) {
			case 'v':
				show_results = 1;
				break;
			case 'i':
				use_inner = 1;
				break;
			case 'f':
				if ((mpi_rank) == 0) {
					// FIXME: Process 0 should read the CSR sparse matrix here
					sparse = NULL;
				}
				break;
			case 'c':
				repl_fact = atoi(optarg);
				break;
			case 's':
				gen_seed = atoi(optarg);
				break;
			case 'e':
				exponent = atoi(optarg);
				break;
			case 'g':
				count_ge = 1;
				ge_element = atof(optarg);
				break;
			default:
				fprintf(stderr, "error parsing argument %c exiting\n", option);
				MPI_Finalize();
				return 3;
		}
	}
	if ((gen_seed == -1) || ((mpi_rank == 0) && (sparse == NULL))) {
		fprintf(stderr, "error: missing seed or sparse matrix file; exiting\n");
		MPI_Finalize();
		return 3;
	}


	comm_start = MPI_Wtime();
	// FIXME: scatter sparse matrix; cache sparse matrix; cache dense matrix
	MPI_Barrier(MPI_COMM_WORLD);
	comm_end = MPI_Wtime();

	comp_start = MPI_Wtime();
	// FIXME: compute C = A ( A ... (AB ) )
	MPI_Barrier(MPI_COMM_WORLD);
	comp_end = MPI_Wtime();

	if (show_results) {
		// FIXME: replace the following line: print the whole result matrix
		printf("1 1\n42\n");
	}
	if (count_ge) {
		// FIXME: replace the following line: count ge elements
		printf("54\n");
	}

	MPI_Finalize();
	return 0;
}
