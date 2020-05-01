#include <mpi.h>
#pragma once

namespace utility {

	enum class GenerateMode 
	{
		SORTED,
		REVERSED
	};

	void print(double* array, int length);
	double* merge_array(int size, double* firstArray, int m, double* secondArray);
	void merge_sort(int size, double* array);
	void generateData(double* array, int size, double m, const GenerateMode mode);
	int isSorted(int n, double* aa, MPI_Comm comm, double* overhead);
	int compare(const void* firstArray, const void* secondArray);
	void exchange(int size, double* originalArray, int sender, int receiver, MPI_Comm comm);
}