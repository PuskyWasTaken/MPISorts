#include "utility.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <utility>
#include <stdio.h>

namespace utility
{
	void print(double* array, int length)
	{
		int amount = 10;
		if (length < amount)
			amount = length;

		printf("\nFirst 10 elements: ");
		for (int i = 0; i < amount; i++)
			printf("%d ", array[i]);
	}

	void merge_sort(int size, double* array) {

		double* c;
		int i;

		if (size <= 1) return;

		if (size == 2) {

			if (array[0] > array[1])std::swap(array[0], array[1]);
			return;
		}

		merge_sort(size / 2, array); merge_sort(size - size / 2, array + size / 2);

		c = merge_array(size / 2, array, size - size / 2, array + size / 2);

		for (i = 0; i < size; i++)array[i] = c[i];

		return;
	}
	double* merge_array(int size, double* firstArray, int m, double* secondArray) {

		int i, j, k;
		double* c = new double[size + m]();

		for (i = j = k = 0; (i < size) && (j < m);)

			if (firstArray[i] <= secondArray[j])c[k++] = firstArray[i++];
			else c[k++] = secondArray[j++];

		if (i == size)for (; j < m;)c[k++] = secondArray[j++];
		else for (; i < size;)c[k++] = firstArray[i++];

		return c;
	}
	
	// TODO: Use C++ style comparator
	int compare(const void* firstArray, const void* secondArray)
	{
		if (*(double*)firstArray > * (double*)secondArray)
			return 1;
		else if (*(double*)firstArray < *(double*)secondArray)
			return -1;

		return 0;
	}
	
	void exchange(int size, double* originalArray, int sender, int receiver, MPI_Comm comm)
	{
		int rank;
		MPI_Comm_rank(comm, &rank);
		MPI_Status status;

		double* swapedArray = new double[size]();
		double* tempArray;

		if (rank == sender)
		{
			MPI_Send(originalArray, size, MPI_DOUBLE, receiver, 1, comm);
			MPI_Recv(swapedArray, size, MPI_DOUBLE, receiver, 2, comm, &status);

			tempArray = merge_array(size, originalArray, size, swapedArray);

			for (int i = 0; i < size; i++)
				originalArray[i] = tempArray[i];
		}
		else
		{
			MPI_Recv(swapedArray, size, MPI_DOUBLE, sender, 1, comm, &status);
			MPI_Send(originalArray, size, MPI_DOUBLE, sender, 2, comm);

			tempArray = merge_array(size, originalArray, size, swapedArray);

			for (int i = 0; i < size; i++)
				originalArray[i] = tempArray[i + size];
		}
	}

	int isSorted(int n, double* aa, MPI_Comm comm, double* overhead)
	{
		double time1 = MPI_Wtime();
		int rank, size, ok = 1;
		double temp;
		MPI_Status status;
		MPI_Comm_rank(comm, &rank);
		MPI_Comm_size(comm, &size);
		if (rank < size - 1) {
			MPI_Send(&aa[n - 1], 1, MPI_DOUBLE, rank + 1, rank, comm);
		}
		if (rank > 0) {
			MPI_Recv(&temp, 1, MPI_DOUBLE, rank - 1, rank - 1, comm, &status);
			if (temp > aa[0])
			{
				ok = 0;
			}
		}
		*overhead += MPI_Wtime() - time1;

		return ok;
	}
	void generateData(double* array, int size, double m, const GenerateMode mode) {
		for (size_t i = 0; i < size; i++)
		{
			switch (mode)
			{
				case GenerateMode::SORTED:
					array[i] = i;
					break;
				case GenerateMode::REVERSED:
					array[i] = (size - i);
					break;
				default:
					array[i] = ((double)rand() / RAND_MAX) * m;
					break;
			}
		}
	}
}
