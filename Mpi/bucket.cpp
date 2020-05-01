#include "bucket.h"
#include <malloc.h>
#include "utility.h"
#include <utility>
#include <stdio.h>

int sort::MPI_Bucket_sort(int n, double m, double* a, int root, MPI_Comm comm)
{
	double comm_time = 0.;
	double comp_time = 0.;
	double time1;
	int size, rank, count = 0;
	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);

	time1 = MPI_Wtime();
	int* counters = new int[size]();
	int* displ = new int[size]();

	double* bucket = new double[n]();
	comp_time += MPI_Wtime() - time1;

	time1 = MPI_Wtime();
	MPI_Bcast(&a[0], n, MPI_DOUBLE, root, comm);
	comm_time += MPI_Wtime() - time1;

	time1 = MPI_Wtime();
	for (int i = 0; i < n; i++)
	{
		if (rank * m / size <= a[i] && a[i] < (rank + 1) * m / size)
			bucket[count++] = a[i];
	}

	qsort(bucket, count, sizeof(double), utility::compare);
	comp_time += MPI_Wtime() - time1;

	time1 = MPI_Wtime();
	MPI_Gather(&count, 1, MPI_INT, counters, 1, MPI_INT, root, comm);
	comm_time += MPI_Wtime() - time1;

	time1 = MPI_Wtime();
	// create displacement array
	displ[0] = 0;
	for (int i = 0; i < size - 1; i++)
	{
		displ[i + 1] = displ[i] + counters[i];
	}
	comp_time += MPI_Wtime() - time1;

	time1 = MPI_Wtime();
	MPI_Gatherv(bucket, count, MPI_DOUBLE, a, counters, displ, MPI_DOUBLE, root, comm);
	comm_time += MPI_Wtime() - time1;

	printf("Processor %d: computation - %lf , communication - %lf\n", rank, comp_time, comm_time);
	return MPI_SUCCESS;
}
