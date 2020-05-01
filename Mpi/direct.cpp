#include "direct.h"
#include <malloc.h>
#include "utility.h"
#include <stdio.h>

int sort::MPI_Direct_sort(int n, double* a, int root, MPI_Comm comm)
{
	int rank, size;
	double comm_time = 0.;
	double comp_time = 0.;
	double time1;

	/* Rank size */
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &size);

	/* Allocation */
	time1 = MPI_Wtime();
	double* local_a = new double[n / size]();
	comp_time += MPI_Wtime() - time1;


	/* Array Scattering */
	time1 = MPI_Wtime();
	MPI_Scatter(a, n / size, MPI_DOUBLE, local_a, n / size, MPI_DOUBLE, root, comm);
	comm_time += MPI_Wtime() - time1;

	/* Sorting */
	time1 = MPI_Wtime();
	utility::merge_sort(n / size, local_a);
	comp_time += MPI_Wtime() - time1;

	/* Gather */
	time1 = MPI_Wtime();
	MPI_Gather(local_a, n / size, MPI_DOUBLE, a, n / size, MPI_DOUBLE, root, comm);
	comm_time += MPI_Wtime() - time1;

	/* Processor 0 merges size-1 chunks */
	if (rank == 0) {
		for (int i = 1; i < size; i++) {
			double* tmp = utility::merge_array(i * n / size, a, n / size, a + i * n / size);
			for (int j = 0; j < (i + 1) * n / size; j++)a[j] = tmp[j];
		}
	}

	printf("Processor %d: computation - %lf , communication - %lf\n", rank, comp_time, comm_time);
	return MPI_SUCCESS;
}
