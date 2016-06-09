#include <cstdio>
#include <string>
#include <stdlib.h>
#include <getopt.h>
#include <mpi.h>

#include <boost/mpi.hpp>

#include "SparseMatrix.hpp"
#include "DenseMatrix.hpp"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include "Misc.hpp"
#include "impl.hpp"

namespace bmpi = boost::mpi;


int main(int argc, char *argv[]) {
	bool show_results = false;
	bool use_inner = false;
	int gen_seed = -1;
	int repl_fact = 1;
	int option = -1;

	double comm_time = 0, comp_time = 0;
	int num_processes = 1;
	int mpi_rank = 0;
	int exponent = 1;
	precision_type ge_element = 0;
	bool count_ge = false;

	SparseMatrix *sparse = nullptr;

	bmpi::environment mpienv(argc, argv, true);
	bmpi::communicator world = bmpi::communicator();
	num_processes = world.size();
	mpi_rank = world.rank();
	while ((option = getopt(argc, argv, "vis:f:c:e:g:")) != -1) {
		switch (option) {
			case 'v':
				show_results = true;
				break;
			case 'i':
				use_inner = true;
				break;
			case 'f':
				if ((mpi_rank) == MASTER) {
					try {
						std::ifstream in(optarg, std::ios_base::in);
						if (in.good()) {
							sparse = new SparseMatrix(in);
						} else {
							std::cerr<<"FILE "<<optarg<<" DOES NOT EXIST, EXITING\n";
							world.abort(123);
						}
					} catch (...) {
						std::cerr<<"ERROR PARSING FILE "<<optarg<<" EXITING\n";
						world.abort(5);
					}
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
				count_ge = true;
				ge_element = atof(optarg);
				break;
			default:
				fprintf(stderr, "error parsing argument %c exiting\n", option);
				return 3;
		}
	}
	if ((gen_seed == -1) || (mpi_rank == MASTER && sparse == nullptr)) {
		std::cerr<<"error: missing seed or sparse matrix file; exiting\n";
		world.abort(13);
	}

//genrate dense matrix B

	bmpi::timer comm_timer = bmpi::timer();
	DenseMatrix *matrixB(nullptr);
	// scatter sparse matrix; cache sparse matrix; cache dense matrix
	if (use_inner) {
//		inner(world, mpi_rank, num_processes, sparse, matrixB, exponent, (size_t) repl_fact);
		throw "Not implemented yet";
	} else {
//		GENERATING MATRIX B
		int cols;
		if (mpi_rank == MASTER) {
			cols = (int) sparse->ncols();
		}
		bmpi::broadcast(world, cols, MASTER);
		matrixB = generateDenseMatrix((size_t) mpi_rank, (size_t) num_processes, (size_t) cols, gen_seed);

//		REDISTRIBUTING MATRIX A
		SparseMatrix sm(redistributeSparseColumns(mpi_rank, num_processes, sparse, world));
//		REPLICATING MATRIX A
		bmpi::communicator replgroup(world.split(mpi_rank / repl_fact));
		std::vector<SparseMatrix> repl_sms;
		bmpi::all_gather(replgroup, sm, repl_sms);
		sparse = new SparseMatrix(repl_sms);
		repl_sms.clear();
	}
	world.barrier();
	comm_time = comm_timer.elapsed();

	bmpi::timer comp_start = bmpi::timer();
	DenseMatrix* matC(nullptr);
	// compute C = A ( A ... (AB ) )
	{
		bmpi::communicator horizontal(world.split(mpi_rank % repl_fact));
		int horizontal_rank = horizontal.rank(),
			horizontal_size = horizontal.size();
		matC = colMultiply(exponent, sparse, matrixB, horizontal, horizontal_rank, horizontal_size);
	}
	world.barrier();
	comp_time = comp_start.elapsed();
	size_t greater_result = 0;
	if (count_ge) {
		// replace the following line: count ge elements
		size_t greater = matC->countGreater(ge_element);
		if (use_inner) {
			throw "NOT IMPLEMENTED YET";
		} else {
			bmpi::reduce(world, greater, greater_result, std::plus<size_t>(), MASTER);
		}
	}

	if (show_results) {
		std::vector<DenseMatrix> c_joined;
		bmpi::gather(world, *matC, c_joined, MASTER);
		delete matC;
		if (mpi_rank == MASTER) {
			assert((int) c_joined.size() == num_processes);

			matC = new DenseMatrix(c_joined);
			std::cout<<*matC;
			delete matC;
		}
	}
	if (count_ge) {
		if (mpi_rank == MASTER) {
			std::cout << greater_result << "\n";
		}
	}

	return 0;
}
