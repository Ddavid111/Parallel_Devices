#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include "kernel_loader.h"

#define CL_TARGET_OPENCL_VERSION 220
#define MAX_SOURCE_SIZE (0x100000)

int main() {
    int n;
    printf("Enter the number of elements in the array: ");
    scanf("%d", &n);

    srand(time(NULL));

    int *arr = (int *)malloc(sizeof(int) * n);
    printf("Generated array: ");
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100;
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Load the kernel source code
    int error_code;
    char *source_str = load_kernel_source("quicksort_opencl.cl", &error_code);
    if (error_code != 0) {
        fprintf(stderr, "Failed to load kernel source.\n");
        free(arr);
        return 1;
    }

    // Initialize the OpenCL environment
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint num_devices;
    cl_uint num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_devices);

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &ret);

    // Create memory buffers on the device for input and output
    cl_mem arr_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(int), NULL, &ret);

    // Copy the input array to the memory buffer
    ret = clEnqueueWriteBuffer(command_queue, arr_mem_obj, CL_TRUE, 0, n * sizeof(int), arr, 0, NULL, NULL);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, NULL, &ret);

    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "quicksort", &ret);

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&arr_mem_obj);

    // Execute the OpenCL kernel
    size_t global_item_size = n; // Process the entire array
    size_t local_item_size = 1;  // Process in a single work item
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

    // Read the memory buffer on the device to the local variable
    ret = clEnqueueReadBuffer(command_queue, arr_mem_obj, CL_TRUE, 0, n * sizeof(int), arr, 0, NULL, NULL);

    // Print the sorted array
    printf("Sorted array: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(arr_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(arr);
    free(source_str);

    return 0;
}
