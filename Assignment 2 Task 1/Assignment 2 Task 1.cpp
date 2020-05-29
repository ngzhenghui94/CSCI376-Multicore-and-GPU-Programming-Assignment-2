#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>


// OpenCL header, depending on OS
#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.hpp>
#endif

#include "common.h"


template<typename T, size_t n>
void print_array(T const(&arr)[n])
{
	for (size_t i = 0; i < n; i++)
		std::cout << arr[i] << ' ';
	std::cout << std::endl;
}



int main() {
	cl::Platform platform;			// device's platform
	cl::Device device;				// device used
	cl::Context context;			// context for the device
	cl::Program program;			// OpenCL program object
	cl::Kernel kernel;				// a single kernel object
	cl::CommandQueue queue;			// commandqueue for a context and device

	// declare data and memory objects
	cl::Buffer arrayOneBuffer;
	cl::Buffer arrayTwoBuffer;
	cl::Buffer outputBuffer;
	int arrayOne[8];
	int arrayTwo[16];
	int outputArray[32];
	//Using time as the seed
	srand(time(NULL));
	for (int i = 0; i < 8; i++) {
		arrayOne[i] = (rand() % 11 + 9); //Rand ints between 10 and 20 (10 and 20 not included) 
		//arrayOne[i] = (rand() % 10 + 11); //Rand ints from 10 to 20 (10 and 20 included)
	}
	int value = 2;
	int negValue = -9;
	for (int i = 0; i < 16; i++) {
		arrayTwo[i] = value;
		value++;
		if (i >= 8) {
			arrayTwo[i] = negValue;
			negValue++;
		}
	}

	std::cout << "Assignment 2 Task 1:" << std::endl;
	std::cout << "--------------------" << std::endl;
	try {
		// select an OpenCL device
		if (!select_one_device(&platform, &device))
		{
			// if no device selected
			quit_program("Device not selected.");
		}

		// create a context from device
		context = cl::Context(device);

		// build the program
		if (!build_program(&program, &context, "kernelAssignment2T1.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}
		// create a kernel
		kernel = cl::Kernel(program, "kernelAssignment2T1");

		// create command queue
		queue = cl::CommandQueue(context, device);

		// create buffers
		arrayOneBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(arrayOne), arrayOne);
		arrayTwoBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(arrayTwo), arrayTwo);
		outputBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(outputArray));

		// set the kernel args index 0, 1, 2 to the corresponding buffers.
		kernel.setArg(0, arrayOneBuffer);
		kernel.setArg(1, arrayTwoBuffer);
		kernel.setArg(2, outputBuffer);

		// enqueue Kernel
		queue.enqueueTask(kernel);
		std::cout << "Kernel enqueued." << std::endl;
		std::cout << "--------------------" << std::endl;

		// enqueue command to read from device to host memory
		queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(outputArray), outputArray);

		//Print output
		std::cout << "Content of Array One from Host : ";
		print_array(arrayOne);
		std::cout << std::endl;
		std::cout << "Content of Array Two from Host : ";
		print_array(arrayTwo);
		std::cout << std::endl;
		std::cout << "Content of Output Buffer from Device Memory:" << std::endl;
		std::cout << "Condition: if v > 15, take from v1 else take from v2." << std::endl;
		std::cout << "Condition2: if any(v) > 15, fill first 4 element with v1 then next 4 with v2." << std::endl;
		std::cout << "v : ";
		for (int i = 0; i < 8; i++) {
			std::cout << outputArray[i];
			if (i != 7) {
				std::cout << ",";
			}
			else {
				std::cout << std::endl;
			}
		}
		std::cout << "v1: ";
		for (int i = 8; i < 16; i++) {
			std::cout << outputArray[i];
			if (i != 15) {
				std::cout << ",";
			}
			else {
				std::cout << std::endl;
			}
		}
		std::cout << "v2: ";
		for (int i = 16; i < 24; i++) {
			std::cout << outputArray[i];
			if (i != 23) {
				std::cout << ",";
			}
			else {
				std::cout << std::endl;
			}
		}
		std::cout << "R : ";
		for (int i = 24; i < 32; i++) {
			std::cout << outputArray[i];
			if (i != 31) {
				std::cout << ",";
			}
			else {
				std::cout << std::endl;
			}
		}
	}
	// catch any OpenCL function errors
	catch (cl::Error e) {
		// call function to handle errors
		handle_error(e);
	}

#ifdef _WIN32
	// wait for a keypress on Windows OS before exiting
	std::cout << "\npress a key to quit...";
	std::cin.ignore();
#endif

	return 0;
	

}