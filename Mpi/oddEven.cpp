#include "oddEven.h"
#include "utility.h"
#include <utility>
#include <stdio.h>

int sort::MPI_OddEven_sort(int n, double* a, int root, MPI_Comm comm)
{
	double comm_time = 0.;
	double comp_time = 0.;
	double overhead = 0.;
	double time1;
	int rank, size;
	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);

	time1 = MPI_Wtime();
	double* aa = new double[n / size]();
	comp_time += MPI_Wtime() - time1;

	time1 = MPI_Wtime();
	MPI_Scatter(&a[0], n / size, MPI_DOUBLE, aa, n / size, MPI_DOUBLE, root, comm);
	comm_time += MPI_Wtime() - time1;

	time1 = MPI_Wtime();
	qsort(aa, n / size, sizeof(double), utility::compare);

	for (int i = 0; i < size; i++)
	{
		comp_time += MPI_Wtime() - time1;


		/* Is sorted? */
		int ok = utility::isSorted(n / size, aa, comm, &overhead);
		int totalOk;

		time1 = MPI_Wtime();
		MPI_Allreduce(&ok, &totalOk, 1, MPI_INT, MPI_MIN, comm);
		overhead += MPI_Wtime() - time1;

		time1 = MPI_Wtime();
		if (totalOk == 1)
			break;

		overhead += MPI_Wtime() - time1;

		//

		time1 = MPI_Wtime();
		if ((i + rank) % 2 == 0)
		{
			if (rank < size - 1)
				utility::exchange(n / size, aa, rank, rank + 1, comm);
		}
		else {
			if (rank > 0)
				utility::exchange(n / size, aa, rank - 1, rank, comm);
		}
		comp_time += MPI_Wtime() - time1;
		MPI_Barrier(comm);
	}

	time1 = MPI_Wtime();
	MPI_Gather(aa, n / size, MPI_DOUBLE, a, n / size, MPI_DOUBLE, root, comm);
	comm_time += MPI_Wtime() - time1;


	printf("Processor %d: computation - %lf , communication - %lf, overhead - %lf\n", rank, comp_time, comm_time, overhead);
	return MPI_SUCCESS;
}
