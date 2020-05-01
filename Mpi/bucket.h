#include <mpi.h>
#pragma once

namespace sort {
	int MPI_Bucket_sort(int n, double m, double* a, int root, MPI_Comm comm);
}
