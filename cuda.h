#ifndef CUDA_H
#define CUDA_H

#include <thrust/device_vector.h>
#include <vector>
#include <iostream>


using namespace std;

typedef thrust::device_vector<int>::iterator DVI;

void printData(int rows, int printWidth, thrust::host_vector<int> & data);
void printDataNoZeroes(int rows, int printWidth, thrust::host_vector<int> & data);
void printData(int rows, int cols, int printWidth, thrust::device_vector<int> & data);
void printData(int rows, int cols, int printWidth, thrust::host_vector<int> & data);
void printData(int rows, int cols, int printWidth, thrust::host_vector<float> & data);
bool generateRandomData(int rows, int cols, int max, thrust::host_vector<int> & data);
bool loadTextFile(FILE *infile, int xSize, int ySize, int zSize, int numvars, thrust::host_vector<float> & h_data, int bufferSize, int & xPos, int & yPos, int & zPos);
void doHistogramGPU(int xSize, int ySize, int zSize, int numvars, thrust::host_vector<float> & h_buffer, thrust::host_vector<int> & h_data, thrust::host_vector<int> & h_data2, int numBins);
void histogramMapReduceGPU(thrust::host_vector<int> & h_data, thrust::host_vector<int> & h_data2, thrust::pair<DVI, DVI> & endPosition, int numVars, int numBins);
std::vector<int> doHistogramCPU(int xSize, int ySize, int zSize, int numVars, thrust::host_vector<float> & h_data);
void printHistoData(int rows, int cols, int printWidth, thrust::host_vector<int> & multiDimKeys, thrust::host_vector<int> & counts);

//#define PRINT_INPUT 1
//#define IS_LOGGING 1
#define PRINT_RESULT 1

typedef thrust::tuple<int, int, int> Int3;
typedef thrust::tuple<int, int> Int2;
typedef thrust::tuple<int> Int;

typedef thrust::tuple<float, float, float> Float3;
typedef thrust::tuple<float, float> Float2;
typedef thrust::tuple<float> Float;


struct BinFinder
{
	float * rawMinVector;
	float * rawMaxVector;
	int numVars;
	int numBins;

	
	BinFinder(float * rawMinVector, float * rawMaxVector, int numVars, int numBins)
	{
		this -> rawMinVector = rawMinVector;
		this -> rawMaxVector = rawMaxVector;
		this -> numVars = numVars;
		this -> numBins = numBins;
	}


	//This kernel assigns each element to a bin group
	__host__ __device__ Int operator()(const Float & param1, const int & param2) const
	{
		

		float value = thrust::get<0>(param1);
		int id = param2;

		float min = rawMinVector[id % numVars];
		float max = rawMaxVector[id % numVars];

		

		float percentage = (value - min) / float(max - min);


		int bin = percentage * numBins;

		if (bin == numBins)
		{
			bin--;
		}

		
		return thrust::make_tuple(bin);

		
	}
	
};


struct MultiToSingleDim
{
	int * rawVector;
	int numBins;

	
	MultiToSingleDim(int * rawVector, int numBins)
	{
		this -> rawVector = rawVector;
		this -> numBins = numBins;
	}

	//This kernel assigns each element to a bin group
	template <typename Tuple>
	__device__ void operator()( Tuple param) 
	{
	

		//int data = thrust::get<0>(param);
		int singleDimIndex = thrust::get<0>(param);
		int cols = thrust::get<1>(param);
		
		int newValue = 0;
		int factor = 1;
		for (int j = cols - 1; j >= 0; j--)
		{
			newValue += (rawVector[singleDimIndex * cols + j]) * factor;

			factor *= numBins;


		}


		thrust::get<2>(param) = newValue;
		
	}
	
};

//TO DO: Port this
struct SingleToMultiDim
{
	int * rawVector;
	int numBins;

	
	SingleToMultiDim(int * rawVector, int numBins)
	{
		this -> rawVector = rawVector;
		this -> numBins = numBins;
	}

	template <typename Tuple>
	__device__ void operator()( Tuple param) 
	{
	

		int singleDimIndex = thrust::get<0>(param);
		int cols = thrust::get<1>(param);
		int dataValue = thrust::get<2>(param);
		
		for (int j = cols - 1; j >= 0; j--)
		{
			//newValue += (rawVector[singleDimIndex * cols + j] - 1) * factor;
			int moddedValue = dataValue % numBins;
			rawVector[singleDimIndex * cols + j] = moddedValue;
			dataValue /= numBins;


		}

		/*
		//Multidimensional representation reconstruction - CPU
	int i = 0;
	for (DVI it = d_single_data.begin(); it != endPosition.first; it++, i++)
	{
		int value = *it;

		for (int j = COLS - 1; j >= 0; j--)
		{
			int moddedValue = value % 4 + 1;
			final_data[i * COLS + j] = moddedValue;
			value /= 4;

		}
	}
	*/
		
	}
	
};






struct ZipComparator
{
	__host__ __device__
	inline bool operator() (const Int & a, const Int & b)
	{
		return thrust::get<0>(a) < thrust::get<0>(b);
	}
};


#endif