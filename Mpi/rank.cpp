#include "rank.h"
#include <malloc.h>
#include <stdio.h>
#include "utility.h"

int sort::MPI_Ranking_sort(int n, double* a, int root, MPI_Comm comm)
{
	int rank, size;
	double comm_time = 0.;
	double comp_time = 0.;
	double overhead = 0.;
	double time1;

	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);

	time1 = MPI_Wtime();
	
	/* Assignments */
	double* ranking = new double[n / size]();
	double* overallRanking = new double[n]();

	comp_time += MPI_Wtime() - time1;

	/* Ranking */
	time1 = MPI_Wtime();

	MPI_Bcast(&a[0], n, MPI_DOUBLE, root, comm);

	comm_time += MPI_Wtime() - time1;


	time1 = MPI_Wtime();
	for (int i = 0; i < n / size; i++)
	{
		ranking[i] = 0;
		for (int j = 0; j < n; j++)
		{
			/* Is sorted? */
			int ok = utility::isSorted(n / size, ranking, comm, &overhead);
			int totalOk;

			time1 = MPI_Wtime();
			MPI_Allreduce(&ok, &totalOk, 1, MPI_DOUBLE, MPI_MIN, comm);
			overhead += MPI_Wtime() - time1;

			time1 = MPI_Wtime();
			if (totalOk == 1)
				break;

			overhead += MPI_Wtime() - time1;

			//


			if (a[i + rank * n / size] > a[j])
				ranking[i]++;
		}
	}
	comp_time += MPI_Wtime() - time1;


	time1 = MPI_Wtime();

	/* Gather */
	MPI_Gather(ranking, n / size, MPI_DOUBLE, overallRanking, n / size, MPI_DOUBLE, root, comm);
	comm_time += MPI_Wtime() - time1;

	/* Processor 0 merges size-1 chunks */
	if (rank == 0)
	{
		double* tmp = new double[n]();
		for (int i = 0; i < n; i++)
			tmp[(int)overallRanking[i]] = a[i];
		
		for (int i = 0; i < n; i++)
			a[i] = tmp[i];
	}

	printf("Processor %d: computation - %lf , communication - %lf, overhead - %lf\n", rank, comp_time, comm_time, overhead);
	return MPI_SUCCESS;
}

