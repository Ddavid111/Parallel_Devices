#include "utils.h" //library of ml programs
#include "vectors.h" //for example only
#include "SizeSpec.h" //library for size specification
#include "ClWrapper.h" //library for context data

#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#define MAX_KERNEL_SIZE 1
#include <stdio.h>
#include <stdlib.h>

void toobigkernel()
{
    //Initialize context with the path to your OpenCL kernel program and it's name
    ClWrapper_t cw = mlInit("kernels/sample.cl","vector_sum");
    //Initialize structure(s) which you want to work with
    Vector vs;
    vs.n = 5;
    for(int i=0; i<5; i++){
        vs.a[i] = i;
        vs.b[i] = i + 2;
        vs.c[i] = 0;
    }
    // Create the device buffer for input and output data
    // buffer_count: The overall amount of structers being used
    // input_param_count: Number of input structures
    // output_param_count = Number of output structures 
    const int buffer_count = 1;
    const int input_param_count = 1;
    const int output_param_count = 1;
    //
    
    // Create buffer for input and output. Input and output can use the same memory 
    // if data is being written and read from the same memory space (i.e. vs.a and vs.b are inputs and vs.c is the output)
    cl_mem buffers[buffer_count];
    buffers[0] = clCreateBuffer(cw.context, CL_MEM_READ_WRITE, sizeof(Vector), NULL, NULL);
    //

    // Prepare the kernel for operation
    mlInitKernel(cw.kernel, cw.context, buffers, buffer_count);
    //

    // Create the command queue
    cl_command_queue command_queue = clCreateCommandQueueWithProperties(cw.context, cw.pad.device_id, NULL, NULL);
    //

    // Transform the input structure to a void * so it can be used as input
    void * i_ptrs[input_param_count];
    i_ptrs[0] = &vs;
    //

    // Host buffer (CPU) -> Device buffer (GPU)
    mlInputToDevice(command_queue, buffers, buffer_count, sizeof(Vector), i_ptrs);
    //

    // Size specification (Overall number of data, number of groups the data should be split)
    SizeSpec_t s = mlSizeSpecification1D(10,2);
    //

    // Apply the kernel on the range
    mlExecComandQueue(command_queue, cw, s);
    //

    // Transform the output structure to a void * so it can be used as input. In this case the input 
    // pointer can be used, since its the same field of data, but if there is an input 
    // structure and a different output structure, this step is required. Right now
    // it's present for example.
    void * o_ptrs[output_param_count];
    o_ptrs[0] = &vs;
    //

    // Host buffer (CPU) <- Device buffer (GPU)
    mlOutputFromDevice(command_queue, buffers, buffer_count,sizeof(Vector), o_ptrs);
    //

    // Print results
    printf("For %d number of work:\n",vs.n);
    for (int i = 0; i < 5; i++)
    {
        printf("[%d]: %d,", i, vs.a[i]);
        printf("[%d]: %d,", i, vs.b[i]);
        printf("[%d]: %d,\n", i, vs.c[i]);
    }
    // Release the resources
    mlReleaseResources(cw);
    //
}