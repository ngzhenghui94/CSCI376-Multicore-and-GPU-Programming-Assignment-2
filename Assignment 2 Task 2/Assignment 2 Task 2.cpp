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

	//Create data struct for Host (Task 2a)
	std::vector<cl_uchar> plainText;
	std::vector<cl_uchar> cipherText;
	std::vector<cl_uchar> decryptedText;

	//Our shift var
	signed int key;
	//Our plaintext filename.
	std::string fileName = "plaintext.txt";
	//Task 2a, file names for caesar cipher on Host/platform.
	std::string outputCipherTextHost = "cipherTextHost.txt";
	std::string outputDecryptedTextHost = "decryptedTextHost.txt";
	//For task 2b, file names for caesar cipher on openCL devices.
	std::string outputCipherTextCL = "cipherTextCL.txt";
	std::string outputDecryptedTextCL = "decryptedTextCL.txt";
	//For Task 2c, file names for substitute cipher on openCL device.
	std::string outputSubstitutedTextCL = "substitutedTextCL.txt";
	std::string outputDeSubstitutedTextCL = "deSubstitutedTextCL.txt";

	


	//Start of Assignment 2 Task 2a.
	//Caesar Cipher in Host
	//Read in pt file to plainText vector.
	std::cout << "Assignment 2 Task 2a: " << std::endl;
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
	
	//Task 2a get user's shift value.
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

	//Task 2a caesar cipher encryption.
	std::cout << "Encrypting Plain Text on Host from: " << fileName << std::endl;
	//var to store and check each char in plainText vector and add it into cipherText vector (along with it's shift)
	char getCharTwo;
	for (int i = 0; i < plainText.size(); i++) {
		getCharTwo = plainText[i];
		if (getCharTwo >= 'a' && getCharTwo <= 'z') {
			getCharTwo = getCharTwo + key; //add to. (shifting)
			if (getCharTwo > 'z') {			//if shifted past z
				getCharTwo = getCharTwo - 'z' + 'a' - 1;
			}
			else if (getCharTwo < 'a') {   //if shifted to negative, wrap around
				getCharTwo = getCharTwo + 26;
			}
			cipherText.push_back(getCharTwo);
		}
		else if (getCharTwo >= 'A' && getCharTwo <= 'Z') {
			getCharTwo = getCharTwo + key; //add to. (shifting)
			if (getCharTwo > 'Z') {			//if shifted past z
				getCharTwo = getCharTwo - 'Z' + 'A' - 1;
			}
			else if (getCharTwo < 'A') {   //if shifted to negative, wrap around
				getCharTwo = getCharTwo + 26; 
			}
			cipherText.push_back(getCharTwo);
		}
		//treating the other characters as it is.
		else if (isblank(getCharTwo)) {  
			cipherText.push_back(' ');
		}
		else if (ispunct(getCharTwo)) {
			cipherText.push_back(getCharTwo);
		}
		else if (isspace(getCharTwo)) {
			cipherText.push_back(getCharTwo);
		}
		else if (isdigit(getCharTwo)) {
			cipherText.push_back(getCharTwo);
		}
	}
	
	//Print out cipherText.
	/*
	std::cout << "Cipher text:" << std::endl;
	for (int i = 0; i < cipherText.size(); i++) {
		std::cout << cipherText[i];
	}
	std::cout << std::endl;
	*/
	//Saving cipher text for task 2a.
	std::ofstream myfile;
	myfile.open(outputCipherTextHost);
	for (int i = 0; i < cipherText.size(); i++) {
		myfile << cipherText[i];
	}
	std::cout << "Output Encrypted Text on `Host to: " << outputCipherTextHost << std::endl;
	myfile.close();

	//Task 2a caesar cipher decryption.
	std::cout << "Decrypting Cipher Text on Host... " << std::endl;
	char getCharThird;
	for (int i = 0; i < cipherText.size(); i++) {
		getCharThird = cipherText[i];
		if (getCharThird >= 'a' && getCharThird <= 'z') {
			getCharThird = getCharThird - key; //add to. (shifting)
			if (getCharThird < 'a') {			//if shifted past z
				getCharThird = getCharThird + 'z' - 'a' + 1;
			}
			else if (getCharThird > 'z') {
				getCharThird = getCharThird - 26;
			}
			decryptedText.push_back(getCharThird);
		}
		else if (getCharThird >= 'A' && getCharThird <= 'Z') {
			getCharThird = getCharThird - key; //add to. (shifting)
			if (getCharThird < 'A') {			//if shifted past z
				getCharThird = getCharThird + 'Z' - 'A' + 1;
			}
			else if (getCharThird > 'Z') {
				getCharThird = getCharThird - 26;
			}
			decryptedText.push_back(getCharThird);
		}
		//treating the other characters as it is.
		else if (isblank(getCharThird)) {
			decryptedText.push_back(' ');
		}
		else if (ispunct(getCharThird)) {
			decryptedText.push_back(getCharThird);
		}
		else if (isspace(getCharThird)) {
			decryptedText.push_back(getCharThird);
		}
		else if (isdigit(getCharThird)) {
			decryptedText.push_back(getCharThird);
		}
	}

	//Saving decrypted text for task 2a.
	std::ofstream myfileTwo;
	myfileTwo.open(outputDecryptedTextHost);
	for (int i = 0; i < decryptedText.size(); i++) {
		myfileTwo << decryptedText[i];
	}
	std::cout << "Output Decrypted Text on Host to: " << outputDecryptedTextHost << std::endl;
	myfileTwo.close();
	std::cout << "\n\n\n" << std::endl;
	//End of Caesar Cipher Host Program.


	/*
	###############################################################################################################
	###############################################################################################################
	###############################################################################################################
	*/

	//Start of Assignment 2 Task 2b
	std::cout << "Assignment 2 Task 2b: " << std::endl;
	//Start of Caesar Cipher for OpenCL devices
	//Get Size for defining the buffer size later, set as *2 the size cause for some reason, it couldn't hold the whole p.t.
	//for some reason ctSize was not big enough for the buffer to store all of the cipherText
	int ctSize = cipherText.size();
	//std::cout << "CT Length: " << ctSize << std::endl;

	//Create Buffer
	cl::Buffer ptBuffer, ctBuffer;	//device side data obj
	//Create Vector
	std::vector<cl_uchar> plainTextCL(ctSize);
	std::vector<cl_uchar> cipherTextCL(ctSize);
	std::vector<cl_uchar> outputVectorCL(ctSize);
	
	
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
		ptBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_uchar) * ctSize, &plainTextCL[0]);
		//second buffer write-only (cipherText)
		ctBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uchar) * ctSize);
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
		cl::NDRange globalSize(ctSize);
		// enqueue Kernel
		queue.enqueueNDRangeKernel(kernel, offset, globalSize);


		std::cout << "Encrypt Kernel enqueued." << std::endl;
		std::cout << "--------------------" << std::endl;

		// enqueue command to read from device to host memory
		queue.enqueueReadBuffer(ctBuffer, CL_TRUE, 0, sizeof(cl_uchar) * ctSize, &outputVectorCL[0]);

		// output contents
		/*
		std::cout << "\nContents of ctBuffer: " << std::endl;
		for (int i = 0; i < ctSize; i++) {
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
		// re-define buffers
		//second buffer read only ( read ciphertext content)
		ctBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_uchar) * ctSize, &cipherTextCL[0]);
		//first buffer write only (ciphertext to plaintext)
		ptBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uchar) * ctSize);


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
		queue.enqueueReadBuffer(ptBuffer, CL_TRUE, 0, sizeof(cl_uchar) * ctSize, &outputVectorCL[0]);

		//Saving encrypted text for task 2b.
		std::ofstream myFileFour;
		myFileFour.open(outputDecryptedTextCL);
		for (int i = 0; i < outputVectorCL.size(); i++) {
			myFileFour << outputVectorCL[i];
		}
		std::cout << "Output Encrypted Text from OpenCL Device to: " << outputDecryptedTextCL << std::endl;
		std::cout << "--------------------" << std::endl;
		myFileFour.close();
		std::cout << "\n\n\n" << std::endl;
		//End of Assignment 2 Task 2b
		/*
		###############################################################################################################
		###############################################################################################################
		###############################################################################################################
		*/

		//Start of Assignment 2 Task 2c
		std::cout << "Assignment 2 Task 2c: " << std::endl;
		//char plainTextChar[26] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
		//	'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
		//char substituteChar[26] = { 'G', 'X', 'S', 'Q', 'F', 'A', 'R', 'O', 'W', 'B', 'L',
		//	'M', 'T', 'H', 'C', 'V', 'P', 'N', 'Z', 'U', 'I', 'E', 'Y', 'D', 'K', 'J' };
		//Create Buffer
		cl::Buffer originalBuffer, substituteBuffer, charBufferA, charBufferB, substitutedBuffer, desubstitutedBuffer;	//device side data obj
		//Create Vector
		std::vector<cl_uchar> originalTextVector(ctSize);
		std::vector<cl_uchar> substitutedTextVector(ctSize);
		std::vector<cl_uchar> outputVector(ctSize);
		//Copy the plaintext we stored previously in plainText to OriginalTextVector here.
		originalTextVector = plainText;

		if (!build_program(&program, &context, "substitute.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}

		//first buffer read only ( read ciphertext content)
		originalBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_uchar) * ctSize, &originalTextVector[0]);
		//second buffer write only (ciphertext to plaintext)
		substituteBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uchar) * ctSize);
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
		queue.enqueueReadBuffer(substituteBuffer, CL_TRUE, 0, sizeof(cl_uchar) * ctSize, &outputVector[0]);

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

		substitutedBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_uchar) * ctSize, &originalTextVector[0]);
		desubstitutedBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uchar) * ctSize);


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
		queue.enqueueReadBuffer(desubstitutedBuffer, CL_TRUE, 0, sizeof(cl_uchar) * ctSize, &outputVector[0]);

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
