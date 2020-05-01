#include <mpi.h>
#pragma once

namespace sort
{
	int MPI_Ranking_sort(int n, double* a, int root, MPI_Comm comm);
}