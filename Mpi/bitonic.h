#pragma once
#include <mpi.h>

namespace sort {

	int MPI_Bitonic_sort(int n, double* a, int root, MPI_Comm comm);


	void CompareLow(int rank, int j, int n, double* a, int root, MPI_Comm comm);
	void CompareHigh(int rank, int j, int n, double* a, int root, MPI_Comm comm);

}