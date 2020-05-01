#include <mpi.h>
#pragma once

namespace sort {
	int MPI_OddEven_sort(int n, double* a, int root, MPI_Comm comm);
}
