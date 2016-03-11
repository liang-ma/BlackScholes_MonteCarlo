# SDAccel creates a compile run or solution of the application per invocation of the tool
# Set the name of the solution to be used throughut the current script
set solution "solution"

# This script will copy the results of the experiment to a user defined Location
set solution_results "results"
# Create a solution
create_solution -name $solution -force

# Define the target devices for the current solution
add_device -vbnv xilinx:adm-pcie-7v3:1ddr:2.0

# Host Source Files
add_files "main.cpp"
add_files "../headers/stockData.cpp"
# Host Compiler Flags
set_property -name host_cflags -value "-I ../headers -g -Wall -D __CLIANG__ " -objects [current_solution]


# Define a kernel to be compiled by SDAccel
# The kernel definition name must match the kernel name in the source code
# The kernel type is given by the source code type for the kernel
create_kernel -type c blackEuro

# Adding files for a kernel works in the same way as adding files for host code.
# User must associate source files to specific kernels using the -kernel option to the add_files command
add_files -kernel [get_kernels blackEuro] "blackEuro.cpp"
add_files -kernel [get_kernels blackEuro] "blackScholes.cpp"
add_files -kernel [get_kernels blackEuro] "../headers/RNG.cpp"
add_files -kernel [get_kernels blackEuro] "../headers/stockData.cpp"

# Create a binary container. Every SDAccel application has at least 1 binary container to hold the FPGA binary.
create_opencl_binary blackEuro1
# Depending on configuration, a target device may have 1 or more areas reserved for kernels compiled by SDAccel
# The user must tell SDAccel which area to target on the device. This sets the compilation parameters for the kernel.
set_property region OCL_REGION_0 [get_opencl_binary blackEuro1]
# Kernels are compiled into compute units. There is at least 1 compute unit per kernel in an FPGA binary.
create_compute_unit -opencl_binary [get_opencl_binary blackEuro1] -kernel [get_kernels blackEuro] -name K1



# Compile the design for CPU based emulation
#compile_emulation -flow cpu -opencl_binary [get_opencl_binary blackEuro]

# Run the design in CPU emulation mode
#run_emulation -flow cpu -args "blackEuro.xclbin"


# Compile the median filter for CPU emulation
compile_emulation -flow hardware -opencl_binary [get_opencl_binary blackEuro1]

# Run the application
run_emulation -flow hardware -args "blackEuro1.xclbin"

#Compile the application to run on an FPGA
#build_system

#Package_system
#package_system

# Run the application in hardware
#run_system -args "blackEuro1.xclbin"


# Compute the resource estimate for the application
report_estimate


