#ifndef CUDA_H
#define CUDA_H

#include <thrust/device_vector.h>
#include <vector>
#include <iostream>

using namespace std;


thrust::host_vector<int> doHistogramGPU(std::vector<int> numbers);
thrust::host_vector<int> doHistogramGPUB(std::vector<int> numberse);
std::vector<int> doHistogramCPU(std::vector<int> numbers);


#define IS_LOGGING 1


struct BinFinder
{
	//This kernel assigns each element to a bin group
	__host__ __device__ int operator()(const int & x, const int & y) const
	{
		//return x + y;
		

		//if (x >= 0 && x <=5)
		//{
		//	return 1;
		//}
		//else if (x >=6 && x <= 10)
		//{
		//	return 2;
		//}
		//else if (x >= 11 && x <= 15)
		//{
		//	return 3;
		//}
		//else
		//{
		//	return 4;
		//}

		return (x >= 0 && x <= 5) * 1 +
			(x >=6 && x <= 10) * 2 +
			(x >= 11 && x <= 15) * 3 +
			(x >= 16 && x <=20) * 4;
		
		

		//cout << x << " ";
		
	}
	
};

struct IndexFinder
{
	//This kernel looks at a bin element and assigns the counting index if it is not 0 and assigns -1 if it is 0
	__host__ __device__ int operator()(const int & x, const int & y) const
	{
		//if (x != 0)
		//{
		//	return y;
		//}
		//else
		//{
		//	return -1;
		//}
		return (x != 0) * y + (x == 0) * -1;
		
		
		
	}
	
};



#endif