/*----------------------------------------------------------------------------
*
* Author:   Liang Ma (liang-ma@polito.it)
*
* Host code defining all the parameters and launching the kernel.
*
* ML_cl.h is OpenCL library file <CL/cl.hpp>.
*
* Exception option is enabled to make host code simpler.
*
* The global and local size are set to 1 since the kernel is written in C/C++ instead of OpenCL.
*
*----------------------------------------------------------------------------
*/

#define __CL_ENABLE_EXCEPTIONS
#include "../headers/ML_cl.h"
#include "testBench.h"

#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char** argv)
{
	ifstream ifstr(argv[1]);
	
	const string programString(istreambuf_iterator<char>(ifstr),
		(istreambuf_iterator<char>()));
	vector<float> h_call(1),h_put(1);

	try
	{
		vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);

		cl_context_properties properties[] =
		{
				CL_CONTEXT_PLATFORM,
				(cl_context_properties)(platforms[0])(),
				0
		};
		cl::Context context(CL_DEVICE_TYPE_ACCELERATOR, properties);
		vector<cl::Device> devices=context.getInfo<CL_CONTEXT_DEVICES>();

		cl::Program::Binaries binaries(1, make_pair(programString.c_str(), programString.length()));
		cl::Program program(context,devices,binaries);
		try
		{
			program.build(devices);
		}
		catch (cl::Error err)
		{
			if (err.err() == CL_BUILD_PROGRAM_FAILURE)
			{
				string info;
				program.getBuildInfo(devices[0],CL_PROGRAM_BUILD_LOG, &info);
				cout << info << endl;
				return EXIT_FAILURE;
			}
			else throw err;
		}

		cl::CommandQueue commandQueue(context, devices[0]);
		
		typedef cl::make_kernel<cl::Buffer,cl::Buffer,float,float,float,float,float> kernelType;
		kernelType kernelFunctor = kernelType(program, TB::kernel_name);

		cl::Buffer d_call = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float)*h_call.size());
		cl::Buffer d_put  = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float)*h_put.size());

		cl::EnqueueArgs enqueueArgs(commandQueue,cl::NDRange(1),cl::NDRange(1));
		cl::Event event = kernelFunctor(enqueueArgs,
						d_call,d_put,
						TB::data.timeT,
						TB::data.freeRate,
				 		TB::data.volatility,
						TB::data.initPrice,
						TB::data.strikePrice
						);

		commandQueue.finish();
		event.wait();

		cl::copy(commandQueue, d_call, h_call.begin(), h_call.end());
		cl::copy(commandQueue, d_put, h_put.begin(), h_put.end());
		for (vector<float>::iterator it = h_call.begin(); it != h_call.end(); it++)
		{
			cout<<"the call price is:"<<*it<<'\t';
		}
		cout<<endl;
		for (vector<float>::iterator it = h_put.begin(); it != h_put.end(); it++)
		{
			cout<<"the put price is:"<<*it<<'\t';
		}
		cout<<endl;
	}
	catch (cl::Error err)
	{
		cerr
			<< "Error:\t"
			<< err.what()
			<< endl;

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
