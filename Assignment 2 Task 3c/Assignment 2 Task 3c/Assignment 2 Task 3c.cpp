#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <iostream>
#include <vector>
#include <fstream>

// OpenCL header, depending on OS
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include "common.h"
#include "bmpfuncs.h"


int main(void)
{
	cl::Platform platform;			// device's platform
	cl::Device device;				// device used
	cl::Context context;			// context for the device
	cl::Program program;			// OpenCL program object
	cl::Kernel kernel;				// a single kernel object
	cl::CommandQueue queue;			// commandqueue for a context and device

	// declare data and memory objects
	unsigned char* Image1a;
	unsigned char* Image1b;
	unsigned char* Image1c;
	unsigned char* outputImage;
	unsigned char* outputImageTwo;
	int imgWidth, imgHeight, imageSize;
	float luminance;
	boolean inputCheck = true;
	cl::ImageFormat imgFormat;
	cl::Image2D ImgBufferA, ImgBufferB, ImgBufferC, outputImgBuffer, outputImgBufferTwo;

	while (inputCheck == true) {
		std::cout << "Input a valid threshold luminance value." << std::endl;
		std::cout << "Pixels above the threshold luminance value are kept, " << std::endl;
		std::cout << "while pixels below this luminance value are set to black (1~255): " << std::endl;
		std::cin >> luminance;
		if (luminance <= 255 && luminance >= 0) {
			luminance = luminance / 255;
			inputCheck = false;
		}
	}
	std::cin.clear();
	std::cin.ignore(1000, '\n');
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
		if (!build_program(&program, &context, "task3c.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}
		// create a kernel
		kernel = cl::Kernel(program, "darken");

		// create command queue
		queue = cl::CommandQueue(context, device);

		// read input image
		Image1a = read_BMP_RGB_to_RGBA("peppers.bmp", &imgWidth, &imgHeight);

		// allocate memory for output image
		imageSize = imgWidth * imgHeight * 4;
		outputImage = new unsigned char[imageSize];
		Image1b = new unsigned char[imageSize];
		Image1c = new unsigned char[imageSize];
		outputImageTwo = new unsigned char[imageSize];

		// image format
		imgFormat = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);

		// create image objects
		ImgBufferA = cl::Image2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)Image1a);
		ImgBufferB = cl::Image2D(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)Image1b);

		// set kernel arguments
		kernel.setArg(0, ImgBufferA);
		kernel.setArg(1, ImgBufferB);
		kernel.setArg(2, luminance);

		// enqueue kernel
		cl::NDRange offset(0, 0);
		cl::NDRange globalSize(imgWidth, imgHeight);

		queue.enqueueNDRangeKernel(kernel, offset, globalSize);

		std::cout << "Kernel enqueued." << std::endl;
		std::cout << "--------------------" << std::endl;

		// enqueue command to read image from device to host memory
		cl::size_t<3> origin, region;
		origin[0] = origin[1] = origin[2] = 0;
		region[0] = imgWidth;
		region[1] = imgHeight;
		region[2] = 1;

		queue.enqueueReadImage(ImgBufferB, CL_TRUE, origin, region, 0, 0, outputImage);

		// output results to image file
		write_BMP_RGBA_to_RGB("darken.bmp", outputImage, imgWidth, imgHeight);

		std::cout << "Completed darkening image for Task 3c(b)." << std::endl;

		std::cout << "Commencing Horizontal and Vertical Filter... " << std::endl;

		ImgBufferB = cl::Image2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)outputImage);
		outputImgBuffer = cl::Image2D(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)outputImage);
		// create a kernel
		kernel = cl::Kernel(program, "horizontalBlur");
		// set kernel arguments
		kernel.setArg(0, ImgBufferB);
		kernel.setArg(1, outputImgBuffer);
		queue.enqueueNDRangeKernel(kernel, offset, globalSize);

		queue.enqueueReadImage(outputImgBuffer, CL_TRUE, origin, region, 0, 0, outputImage);

		write_BMP_RGBA_to_RGB("horizontalBlur.bmp", outputImage, imgWidth, imgHeight);
		std::cout << "Completed horizontal blur on image for Task 3c(c)." << std::endl;
		// create a kernel
		kernel = cl::Kernel(program, "verticalBlur");
		outputImgBuffer = cl::Image2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)outputImage);
		ImgBufferC = cl::Image2D(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)Image1c);
		// set kernel arguments
		kernel.setArg(0, outputImgBuffer);
		kernel.setArg(1, ImgBufferC);
		queue.enqueueNDRangeKernel(kernel, offset, globalSize);

		queue.enqueueReadImage(ImgBufferC, CL_TRUE, origin, region, 0, 0, outputImage);

		write_BMP_RGBA_to_RGB("verticalBlur.bmp", outputImage, imgWidth, imgHeight);
		std::cout << "Completed vertical blur on image for Task 3c(c)." << std::endl;

		// create a kernel
		kernel = cl::Kernel(program, "bloom");
		ImgBufferA = cl::Image2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)Image1a);
		ImgBufferC = cl::Image2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)Image1c);
		outputImgBuffer = cl::Image2D(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)outputImage);
		// set kernel arguments
		kernel.setArg(0, ImgBufferA);
		kernel.setArg(1, ImgBufferC);
		kernel.setArg(2, outputImgBuffer);
		queue.enqueueNDRangeKernel(kernel, offset, globalSize);

		queue.enqueueReadImage(outputImgBuffer, CL_TRUE, origin, region, 0, 0, outputImage);

		write_BMP_RGBA_to_RGB("bloom.bmp", outputImage, imgWidth, imgHeight);
		std::cout << "Completed bloom effect on image." << std::endl;

		// deallocate memory
		free(Image1a);
		free(outputImage);


}	// catch any OpenCL function errors
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
