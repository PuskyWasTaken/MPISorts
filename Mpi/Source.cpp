/**
 * MPI Program that performs simple sorting
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <string.h>
#include "utility.h"
#include "oddEven.h"
#include "rank.h"
#include "bitonic.h"
#include "bucket.h"
#include "direct.h"
#include <map>
#include <unordered_map>
#include <functional>
#include "SortOption.h"
#include <string>



void callSortByOption(MPISortOption option, int n, double* a, int root, MPI_Comm comm, double m) {

	switch (option)
	{
		case MPISortOption::ODD_EVEN:
			sort::MPI_OddEven_sort(n, a, root, comm);
			break;
		case MPISortOption::RANK:
			sort::MPI_Ranking_sort(n, a, root, comm);
			break;
		case MPISortOption::BUCKET:
			sort::MPI_Bucket_sort(n, m, a, root, comm);
			break;
		case MPISortOption::DIRECT:
			sort::MPI_Direct_sort(n, a, root, comm);
			break;
		case MPISortOption::BITONIC:
			sort::MPI_Bitonic_sort(n, a, root, comm);
			break;
		case MPISortOption::SHELL:
			break;
	default: throw("Select a valid sorting option! Dummy..");
	}
}


int main(int argc, char* argv[])
{

	/* Get the sorting options */
	MPISortOption option = MPISortOption::ODD_EVEN;

	if (argc >= 2)
		option = static_cast<MPISortOption>(std::atoi(argv[1])); // Lazy to do checks here

	int rank, size;

	int n = 1000000;
	double m = 200.0;
	double* array = NULL;

	/* Init */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	/* Alloc the array */
	array = new double[n]();

	// for bucket sort in best and worst case scenario
	// m = n;

	if (rank == 0)
	{
		/* Random seed */
		srand(8);

		/* Generation mode. Default is RANDOM */
		utility::generateData(array, n, m, utility::GenerateMode::SORTED);

	}

	double time1 = MPI_Wtime();

	callSortByOption(option, n, array, 0, MPI_COMM_WORLD, m);

	// printf("Processor %d worked for %lf\n", rank, MPI_Wtime() - time1);

	/*if (rank == 0)
	{
		for (int i = 0; i < n; i++)printf("%lf ", array[i]);
	}*/

	MPI_Finalize();
}




