#include <iostream>
#include <string> 
#include "../Mpi/SortOption.h"
#include <windows.h>

void runMPI(int processors, int option)
{
	std::string runString = "mpiexec -n ";
	runString += std::to_string(processors);


#ifdef _DEBUG
	runString += " ../Mpi/Debug/Mpi.exe ";
	runString += std::to_string(option);
	system(runString.c_str());
#else
	runString += " ../Mpi/Release/Mpi.exe ";
	runString += std::to_string(option);
	system(runString.c_str());
#endif

}

void printOption(int option, int processors)
{
	MPISortOption op = static_cast<MPISortOption>(option);

	switch (op)
	{
	case MPISortOption::ODD_EVEN:
		printf("OddEven Sort using %d processors", processors);
		break;
	case MPISortOption::RANK:
		printf("Rank Sort using %d processors", processors);
		break;
	case MPISortOption::BUCKET:
		printf("Bucket Sort using %d processors", processors);
		break;
	case MPISortOption::DIRECT:
		printf("Direct Sort using %d processors", processors);
		break;
	case MPISortOption::BITONIC:
		printf("Bitonic Sort using %d processors", processors);
		break;
	case MPISortOption::SHELL:
		// TODO
		break;
	default: throw("Select a valid sorting option! Dummy..");
	}

	printf("\n");
}

int main()
{
	int coreArray[4] = {2, 4, 8, 16};

	for (int op = 0; op < static_cast<int>(MPISortOption::Count); ++op)
	{
		for (int i = 0; i < 4; ++i)
		{
			printOption(op, coreArray[i]);
			printf("\n");

			runMPI(coreArray[i], op);
			printf("\n");
			
			Sleep(2000);
		}
		printf("\n");
	}
	
	std::cin.get();
	return 0;
}