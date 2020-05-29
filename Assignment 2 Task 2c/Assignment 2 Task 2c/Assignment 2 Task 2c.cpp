#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <regex>


// OpenCL header, depending on OS
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include "common.h"


int main() {
	cl::Platform platform;			// device's platform
	cl::Device device;				// device used
	cl::Context context;			// context for the device
	cl::Program program;			// OpenCL program object
	cl::Kernel kernel;				// a single kernel object
	cl::CommandQueue queue;			// commandqueue for a context and device

	//Create data struct for Host to read plaintext.txt and store it's content.
	std::vector<cl_uchar> plainText;

	//Our shift var
	signed int key;
	//Our plaintext filename.
	std::string fileName = "plaintext.txt";
	//For Task 2c, file names for substitute cipher on openCL device.
	std::string outputSubstitutedTextCL = "substitutedTextCL.txt";
	std::string outputDeSubstitutedTextCL = "deSubstitutedTextCL.txt";


	//Read in pt file to plainText vector.
	std::cout << "Assignment 2 Task 2c: " << std::endl;
	std::ifstream plainTextFile;
	plainTextFile.open(fileName, std::ifstream::in);
	//char var to store and check each Character in the file.
	char getCharOne;
	if (plainTextFile.is_open()) {
		while (plainTextFile.get(getCharOne)) {
			if (getCharOne >= 'a' && getCharOne <= 'z') {
				getCharOne = getCharOne - 32;
				plainText.push_back(getCharOne);
			}
			else if (getCharOne >= 'A' && getCharOne <= 'Z') {
				plainText.push_back(getCharOne);
			}
			else if (isblank(getCharOne)) {
				plainText.push_back(' ');
			}
			else if (ispunct(getCharOne)) {
				plainText.push_back(getCharOne);
			}
			else if (isspace(getCharOne)) {
				plainText.push_back(getCharOne);
			}
			else if (isdigit(getCharOne)){
				plainText.push_back(getCharOne);
			}
		}
		plainTextFile.close();
	}

	//Print out the plaintext we have read into plainText (vector)
	/*
	std::cout << "Plain text:" << std::endl;
	for (int i = 0; i < plainText.size(); i++) {
		std::cout << plainText[i];
	}
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	*/
	

	int ptSize = plainText.size();
	//std::cout << "CT Length: " << ptSize << std::endl;


	//OpenCL standards > select device, create context, build prog, define buffer, create kernels, set args,
	//command queue
	try {
		// select an OpenCL device
		if (!select_one_device(&platform, &device))
		{
			// if no device selected
			quit_program("Device not selected.");
		}

		// create a context from device
		context = cl::Context(device);


		// set modifier and work-items
		cl::NDRange offset(0);
		cl::NDRange globalSize(ptSize);
	
		//Create Buffer
		cl::Buffer originalBuffer, substituteBuffer, substitutedBuffer, desubstitutedBuffer;	//device side data obj
		//Create Vector
		std::vector<cl_uchar> originalTextVector(ptSize);
		std::vector<cl_uchar> substitutedTextVector(ptSize);
		std::vector<cl_uchar> outputVector(ptSize);
		//Copy the plaintext we stored previously in plainText to OriginalTextVector here.
		originalTextVector = plainText;

		if (!build_program(&program, &context, "substitute.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}

		//first buffer read only ( read ciphertext content)
		originalBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_uchar) * ptSize, &originalTextVector[0]);
		//second buffer write only (ciphertext to plaintext)
		substituteBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uchar) * ptSize);
		//charBuffers
//	    charBufferA = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(26), &plainTextChar);
//		charBufferB = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(26), &substituteChar);

		// create a kernel
		kernel = cl::Kernel(program, "substitute");

		// create command queue
		queue = cl::CommandQueue(context, device);

		// set the kernel args index 0, 1, 2 to the corresponding buffers.
		kernel.setArg(0, originalBuffer);
		kernel.setArg(1, substituteBuffer);
//		kernel.setArg(2, charBufferA);
//		kernel.setArg(3, charBufferB);


		// enqueue Kernel
		queue.enqueueNDRangeKernel(kernel, offset, globalSize);

		std::cout << "Substitute Kernel enqueued." << std::endl;
		std::cout << "--------------------" << std::endl;

		// enqueue command to read from device to host memory
		queue.enqueueReadBuffer(substituteBuffer, CL_TRUE, 0, sizeof(cl_uchar) * ptSize, &outputVector[0]);

		//Saving encrypted text for task 2c.
		std::ofstream myFileFive;
		myFileFive.open(outputSubstitutedTextCL);
		for (int i = 0; i < outputVector.size(); i++) {
			myFileFive << outputVector[i];
		}
		std::cout << "Output Encrypted Text from OpenCL Device to: " << outputSubstitutedTextCL << std::endl;
		std::cout << "--------------------" << std::endl;
		myFileFive.close();

		originalTextVector = outputVector;

		if (!build_program(&program, &context, "desubstitute.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}

		substitutedBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_uchar) * ptSize, &originalTextVector[0]);
		desubstitutedBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uchar) * ptSize);


		// create a kernel
		kernel = cl::Kernel(program, "desubstitute");

		// create command queue
		queue = cl::CommandQueue(context, device);

		// set the kernel args index 0, 1, 2 to the corresponding buffers.
		kernel.setArg(0, substitutedBuffer);
		kernel.setArg(1, desubstitutedBuffer);
		//		kernel.setArg(2, charBufferA);
		//		kernel.setArg(3, charBufferB);

		// enqueue Kernel
		queue.enqueueNDRangeKernel(kernel, offset, globalSize);

		std::cout << "De-Substitute Kernel enqueued." << std::endl;
		std::cout << "--------------------" << std::endl;

		// enqueue command to read from device to host memory
		queue.enqueueReadBuffer(desubstitutedBuffer, CL_TRUE, 0, sizeof(cl_uchar) * ptSize, &outputVector[0]);

		//Saving encrypted text for task 2c.
		std::ofstream myFileSix;
		myFileFive.open(outputDeSubstitutedTextCL);
		for (int i = 0; i < outputVector.size(); i++) {
			myFileFive << outputVector[i];
		}
		std::cout << "Output Encrypted Text from OpenCL Device to: " << outputDeSubstitutedTextCL << std::endl;
		std::cout << "--------------------" << std::endl;
		myFileFive.close();

	}
	catch (cl::Error err) {
		handle_error(err);
	}
}
