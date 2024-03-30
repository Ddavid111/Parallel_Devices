#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#include "kernel_loader.h"

#define MAX_SOURCE_SIZE (0x100000)

int main() {

    int left, right;
    int size;
    printf("Enter the number of elements in the array: ");
    scanf("%d", &size);

    int *data = (int *)malloc(size * sizeof(int));
    printf("The elements of the array: ");
    srand(time(NULL));
    for (int i = 0; i < size; i++) {
        data[i] = rand() % 100;
        printf("%d ", data[i]);
    }
    printf("\n");

    // OpenCL kernel betöltése a fájlból
    cl_int ret;
    char *kernel_source = load_kernel_source("kernels/quicksort_opencl.cl", &ret); // Kernel betöltése
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to load kernel source file.\n");
        exit(1);
    }

    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint num_platforms, num_devices;
    clGetPlatformIDs(1, &platform_id, &num_platforms);
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_devices);

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, NULL);
    cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, NULL);

    cl_mem data_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, size * sizeof(int), NULL, NULL);
    clEnqueueWriteBuffer(command_queue, data_buffer, CL_TRUE, 0, size * sizeof(int), data, 0, NULL, NULL);

    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernel_source, NULL, &ret);
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "quicksort", &ret);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&data_buffer);

    left = 0;
    right = size - 1;

    clSetKernelArg(kernel, 1, sizeof(cl_int), (void *)&left);
    clSetKernelArg(kernel, 2, sizeof(cl_int), (void *)&right);

    size_t global_item_size = 1;
    size_t local_item_size = 1;
    clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    clFinish(command_queue);

    clEnqueueReadBuffer(command_queue, data_buffer, CL_TRUE, 0, size * sizeof(int), data, 0, NULL, NULL);

    printf("Sorted data: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");

    clFlush(command_queue);
    clFinish(command_queue);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(data_buffer);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);

    free(data);
    free(kernel_source);

    return 0;
}
