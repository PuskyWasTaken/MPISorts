#include "bitonic.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "utility.h"

int sort::MPI_Bitonic_sort(int n, double* a, int root, MPI_Comm comm)
{
	int rank, size;
	double comm_time = 0.;
	double comp_time = 0.;
	double overhead = 0.;
	double time1;

	// find rank size
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &size);

	// Cube Dimension
	int dimensions = (int)(log2(size));
	// Start Timer before starting first sort operation (first iteration)

	time1 = MPI_Wtime();

	// Sequential Sort
	qsort(a, n, sizeof(int),

	/* Compare */
	[](const void* a, const void* b) {

		if (*(double*)a > * (double*)b) 
			return 1;

		else if (*(double*)a < *(double*)b)
			return -1;

		return 0;
	});

	comp_time += MPI_Wtime() - time1;

	// Bitonic Sort follows
	for (int i = 0; i < dimensions; i++) {
		for (int j = i; j >= 0; j--) {

			/* Is sorted? */
			int ok = utility::isSorted(n / size, a, comm, &overhead);
			int totalOk;

			time1 = MPI_Wtime();
			MPI_Allreduce(&ok, &totalOk, 1, MPI_INT, MPI_MIN, comm);
			overhead += MPI_Wtime() - time1;

			// (window_id is even AND jth bit of process is 0) OR (window_id is odd AND jth bit of process is 1)
			if (((rank >> (i + 1)) % 2 == 0 && (rank >> j) % 2 == 0) || ((rank >> (i + 1)) % 2 != 0 && (rank >> j) % 2 != 0))
			{
				time1 = MPI_Wtime();
				CompareLow(rank, j, n, a, root, comm);
				comm_time += MPI_Wtime() - time1;
			}
			else
			{
				time1 = MPI_Wtime();
				CompareHigh(rank, j, n, a, root, comm);
				comm_time += MPI_Wtime() - time1;
			}
		}
	}

	printf("Processor %d: computation - %lf , communication - %lf, overhead - %lf\n", rank, comp_time, comm_time, overhead);

	return 0;
}

void sort::CompareLow(int rank, int j, int n, double* a, int root, MPI_Comm comm)
{
	int i, min;

	/* Sends the biggest of the list and receive the smallest of the list */
	int send_counter = 0;
	int* buffer_send = (int*)malloc((n + 1) * sizeof(int));
	MPI_Send(&a[n - 1], 1, MPI_INT, rank ^ (1 << j), 0, MPI_COMM_WORLD);

	int* buffer_recieve = (int*)malloc((n + 1) * sizeof(int));
	MPI_Recv(&min, 1, MPI_INT, rank ^ (1 << j), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	for (i = 0; i < n; i++) {
		if (a[i] > min) {
			buffer_send[send_counter + 1] = a[i];
			send_counter++;
		}
		else {
			break;
		}
	}

	buffer_send[0] = send_counter;

	// send partition to paired H process
	MPI_Send(buffer_send, send_counter, MPI_INT, rank ^ (1 << j), 0, MPI_COMM_WORLD);

	// receive info from paired H process
	MPI_Recv(buffer_recieve, n, MPI_INT, rank ^ (1 << j), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	for (i = 1; i < buffer_recieve[0] + 1; i++) {
		if (a[n - 1] < buffer_recieve[i])
			a[n - 1] = buffer_recieve[i];
		else
			break;
	}

	// Sequential Sort
	qsort(a, n, sizeof(int),
		/* COMPARE FUNCTION*/
		[](const void* a, const void* b)
		{
			if (*(double*)a > * (double*)b)
				return 1;

			else if (*(double*)a < *(double*)b)
				return -1;

			return 0;
		});

	free(buffer_send);
	free(buffer_recieve);
}

void sort::CompareHigh(int rank, int j, int n, double* a, int root, MPI_Comm comm)
{
	int i, max;

	// Recieve Max
	int recv_counter;
	int* buffer_recieve = (int*)malloc((n + 1) * sizeof(int));
	MPI_Recv(&max, 1, MPI_INT, rank ^ (1 << j), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	// Send min
	int send_counter = 0;
	int* buffer_send = (int*)malloc((n + 1) * sizeof(int));
	MPI_Send(&a[0], 1, MPI_INT, rank ^ (1 << j), 0, MPI_COMM_WORLD);

	// Buffer a list of values which are smaller than max value
	for (i = 0; i < n; i++)
	{
		if (a[i] < max)
		{
			buffer_send[send_counter + 1] = a[i];
			send_counter++;
		}
		else
			break;
	}

	// Receive blocks greater than min from paired slave
	MPI_Recv(buffer_recieve, n, MPI_INT, rank ^ (1 << j), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	recv_counter = buffer_recieve[0];

	// send partition to paired slave
	buffer_send[0] = send_counter;
	MPI_Send(
		buffer_send, send_counter, MPI_INT, rank ^ (1 << j), 0, MPI_COMM_WORLD);

	// Take received buffer of values from L Process which are greater than current min
	for (i = 1; i < recv_counter + 1; i++)
		if (buffer_recieve[i] > a[0])
			a[0] = buffer_recieve[i];
		else
			break;

	// Sequential Sort
	qsort(a, n, sizeof(int),

	/* Compare */
	[](const void* a, const void* b)
		{

			if (*(double*)a > * (double*)b)
				return 1;

			else if (*(double*)a < *(double*)b)
				return -1;

			return 0;
		});

	// Reset the state of the heap from Malloc
	free(buffer_send);
	free(buffer_recieve);
}
