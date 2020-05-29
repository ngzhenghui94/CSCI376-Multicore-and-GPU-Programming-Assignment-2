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
	//For task 2b, file names for caesar cipher on openCL devices.
	std::string outputCipherTextCL = "cipherTextCL.txt";
	std::string outputDecryptedTextCL = "decryptedTextCL.txt";

	//Start of Assignment 2 Task 2b.
	//Caesar Cipher using OpenCL device
	//Read in pt file to plainText vector.
	std::cout << "Assignment 2 Task 2b: " << std::endl;
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
	
	//Task 2b get user's shift value.
	std::cout << "Please enter the shift value: ";
	std::cin >> key;
	//Check input, shift should be at most 26.
	while (1) {
		if (std::cin.fail() || key >= 26) {
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			std::cout << "Please enter a valid integer between 1 and 26. " << std::endl;
			std::cin >> key;
		}
		else {
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			break;
		}
	}
	std::cout << "You have input a shift value of: " << key << std::endl;
	std::cout << "\n";

	
	//Start of Caesar Cipher for OpenCL devices
	int ptSize = plainText.size();
	//std::cout << "PT Length: " << ptSize << std::endl;

	//Create Buffer
	cl::Buffer ptBuffer, ctBuffer;	//device side data obj
	//Create Vector
	std::vector<cl_uchar> plainTextCL(ptSize);
	std::vector<cl_uchar> cipherTextCL(ptSize);
	std::vector<cl_uchar> outputVectorCL(ptSize);
	
	//Copy the Plaintexts we had read from the other vector.
	plainTextCL = plainText;

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

		// define buffers
		//first buffer read-only (plainText)
		ptBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_uchar) * ptSize, &plainTextCL[0]);
		//second buffer write-only (cipherText)
		ctBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uchar) * ptSize);
		//shiftBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(key), &key);

		// build the program
		if (!build_program(&program, &context, "cipher.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}
		// create a kernel
		kernel = cl::Kernel(program, "cipher");

		// create command queue
		queue = cl::CommandQueue(context, device);

		// set the kernel args index 0, 1, 2 to the corresponding buffers.
		kernel.setArg(0, key);
		kernel.setArg(1, ptBuffer);
		kernel.setArg(2, ctBuffer);

		// set modifier and work-items
		cl::NDRange offset(0);
		cl::NDRange globalSize(ptSize);
		// enqueue Kernel
		queue.enqueueNDRangeKernel(kernel, offset, globalSize);

		std::cout << "Encrypt Kernel enqueued." << std::endl;
		std::cout << "--------------------" << std::endl;

		// enqueue command to read from device to host memory
		queue.enqueueReadBuffer(ctBuffer, CL_TRUE, 0, sizeof(cl_uchar) * ptSize, &outputVectorCL[0]);

		// output contents
		/*
		std::cout << "\nContents of ctBuffer: " << std::endl;
		for (int i = 0; i < ptSize; i++) {
			std::cout << outputVector[i] << " ";
		}
		std::cout << outputVector.size();
		*/

		//Saving encrypted text for task 2b.
		std::ofstream myfileThree;
		myfileThree.open(outputCipherTextCL);
		for (int i = 0; i < outputVectorCL.size(); i++) {
			myfileThree << outputVectorCL[i];
		}
		std::cout << "Output Encrypted Text from OpenCL Device to: " << outputCipherTextCL << std::endl;
		std::cout << "--------------------" << std::endl;
		myfileThree.close();
		

		//Decrypt text for task 2b
		// build the program
		std::cout << "\n";
		std::cout << "Rebuilding program for decryption...." << std::endl;
		if (!build_program(&program, &context, "decipher.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}
		
		//copy the outputVector we previously read from device to host memory to ciphertextVector.
		cipherTextCL = outputVectorCL;
		// re-define buffers (reverse direction) 
		//first buffer read only ( read ciphertext content)
		ctBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_uchar) * ptSize, &cipherTextCL[0]);
		//second buffer write only (ciphertext to plaintext)
		ptBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uchar) * ptSize);

		// create a kernel
		kernel = cl::Kernel(program, "decipher");

		// create command queue
		queue = cl::CommandQueue(context, device);

		// set the kernel args index 0, 1, 2 to the corresponding buffers.
		kernel.setArg(0, key);
		kernel.setArg(1, ptBuffer);
		kernel.setArg(2, ctBuffer);

		// enqueue Kernel
		queue.enqueueNDRangeKernel(kernel, offset, globalSize);

		std::cout << "Decrypt Kernel enqueued." << std::endl;
		std::cout << "--------------------" << std::endl;

		// enqueue command to read from device to host memory
		queue.enqueueReadBuffer(ptBuffer, CL_TRUE, 0, sizeof(cl_uchar) * ptSize, &outputVectorCL[0]);

		//Saving encrypted text for task 2b.
		std::ofstream myFileFour;
		myFileFour.open(outputDecryptedTextCL);
		for (int i = 0; i < outputVectorCL.size(); i++) {
			myFileFour << outputVectorCL[i];
		}
		std::cout << "Output Encrypted Text from OpenCL Device to: " << outputDecryptedTextCL << std::endl;
		std::cout << "--------------------" << std::endl;
		myFileFour.close();



	}
	catch (cl::Error err) {
		handle_error(err);
	}
}
