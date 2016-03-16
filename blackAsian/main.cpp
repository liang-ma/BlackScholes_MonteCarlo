/*----------------------------------------------------------------------------
*
* Author:   Liang Ma (liang-ma@polito.it)
*
* Host code defining all the parameters and launching the kernel.
* The only command line argument is the name of the kernel.
*
* ML_cl.h is the OpenCL library file <CL/cl.hpp>. Currently the version shipped with SDAccel is buggy.
* 
* Several input parameters for the simulation are defined in namespace Paras.
*
* S0:		stock price at time 0
* K:		strike price
* rate:		interest rate
* volatility:		volatility of stock
* T:		time period of the option
* kernel_name:	the kernel name
*
* Exception handling is enabled (__CL_ENABLE_EXCEPTIONS) to make host code simpler.
*
* The global and local size are set to 1 since the kernel is written in C/C++ instead of OpenCL.
*
*----------------------------------------------------------------------------
*/

#define __CL_ENABLE_EXCEPTIONS

// This should be used when cl.hpp from SDAccel works.
#ifdef CL_HEADER_BUG_FIXED
#include <CL/cl.hpp>
#else
#include "../common/ML_cl.h"
#endif

#include "../common/stockData.h"
#include <iostream>
#include <fstream>
using namespace std;


namespace Paras 
{
	const double S0 = 100;
	const double K = 105;
	const double rate = 0.1;
	const double volatility = 0.15;
	const double T = 10.0;
	stockData data(T,rate,volatility,S0,K);
}

int main(int argc, char** argv)
{
	// Get the name of the kernel to be launched.
	ifstream ifstr(argv[1]);
	
	const string programString(istreambuf_iterator<char>(ifstr),
		(istreambuf_iterator<char>()));
	vector<float> h_call(1),h_put(1);

	try
	{
		vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);

		cl::Context context(CL_DEVICE_TYPE_ACCELERATOR);
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
				program.geParasuildInfo(devices[0],CL_PROGRAM_BUILD_LOG, &info);
				cout << info << endl;
				return EXIT_FAILURE;
			}
			else throw err;
		}

		cl::CommandQueue commandQueue(context, devices[0]);
		
		typedef cl::make_kernel<cl::Buffer,cl::Buffer,float,float,float,float,float> kernelType;
		kernelType kernelFunctor = kernelType(program, "blackAsian");

		cl::Buffer d_call = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float));
		cl::Buffer d_put  = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float));

		cl::EnqueueArgs enqueueArgs(commandQueue,cl::NDRange(1),cl::NDRange(1));
		cl::Event event = kernelFunctor(enqueueArgs,
						d_call,d_put,
						Paras::data.timeT,
						Paras::data.freeRate,
				 		Paras::data.volatility,
						Paras::data.initPrice,
						Paras::data.strikePrice
						);

		commandQueue.finish();
		event.wait();

		cl::copy(commandQueue, d_call, h_call.begin(), h_call.end());
		cl::copy(commandQueue, d_put, h_put.begin(), h_put.end());
		cout<<"the call price is:"<<h_call[0]<<endl;
		cout<<"the put price is:"<<h_put[0]<<endl;
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
