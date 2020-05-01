#include <mpi.h>
#pragma once

namespace sort 
{
	int MPI_Direct_sort(int n, double* a, int root, MPI_Comm comm);
}