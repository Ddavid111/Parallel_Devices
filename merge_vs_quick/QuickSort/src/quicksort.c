#include <stdio.h>
#include <stdlib.h>
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#include <time.h>
#include "kernel_loader.h"

#define MAX_SOURCE_SIZE (0x100000)

void partition(int arr[], const int low, const int high, int *pi) {
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    *pi = (i + 1);
}

void quick_sort_seq(int arr[], const int low, const int high) {
    if (low < high) {
        int pi;
        partition(arr, low, high, &pi);
        quick_sort_seq(arr, low, pi - 1);
        quick_sort_seq(arr, pi + 1, high);
    }
}

int main() {
    const int sizes[] = {1000000, 2000000, 3000000, 4500000, 6000000, 7000000};
    const int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint num_devices;
    cl_uint num_platforms;
    cl_int ret;
    cl_context context;
    cl_command_queue command_queue;
    cl_program program;
    cl_kernel partition_kernel, quick_sort_kernel, merge_kernel;
    cl_mem arr_buffer, pi_buffer;

    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("kernels/quicksort.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char *)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    ret = clGetPlatformIDs(1, &platform_id, &num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_devices);
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);

    program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    partition_kernel = clCreateKernel(program, "partition", &ret);
    quick_sort_kernel = clCreateKernel(program, "quick_sort", &ret);
    merge_kernel = clCreateKernel(program, "merge", &ret);

    clock_t seq_time, parallel_time;
    double seq_duration, parallel_duration;

    for (int s = 0; s < num_sizes; ++s) {
        const int n = sizes[s];
        int *arr = (int *)malloc(sizeof(int) * n);
        int *arr_copy = (int *)malloc(sizeof(int) * n);

        for (int i = 0; i < n; ++i) {
            arr[i] = rand() % 1000;
            arr_copy[i] = arr[i];
        }

        seq_time = clock();
        quick_sort_seq(arr_copy, 0, n - 1);
        seq_duration = (double)(clock() - seq_time) / CLOCKS_PER_SEC;

        arr_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * n, arr, &ret);
        pi_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);

        ret = clSetKernelArg(partition_kernel, 0, sizeof(cl_mem), (void *)&arr_buffer);
        ret = clSetKernelArg(partition_kernel, 3, sizeof(cl_mem), (void *)&pi_buffer);

        ret = clSetKernelArg(quick_sort_kernel, 0, sizeof(cl_mem), (void *)&arr_buffer);
        ret = clSetKernelArg(quick_sort_kernel, 3, sizeof(cl_mem), (void *)&pi_buffer);

        size_t global_item_size_partition = 1;
        size_t local_item_size = 1;

        parallel_time = clock();
        ret = clEnqueueNDRangeKernel(command_queue, partition_kernel, 1, NULL, &global_item_size_partition, &local_item_size, 0, NULL, NULL);
        ret = clEnqueueNDRangeKernel(command_queue, quick_sort_kernel, 1, NULL, &global_item_size_partition, &local_item_size, 0, NULL, NULL);
        ret = clFinish(command_queue);
        parallel_duration = (double)(clock() - parallel_time) / CLOCKS_PER_SEC;

        double acceleration = seq_duration / parallel_duration;

        printf("Array size: %d\n", n);
        printf("Sequential quicksort duration: %.2e seconds\n", seq_duration);
        printf("Parallel quicksort duration: %.2e seconds\n", parallel_duration);
        printf("Acceleration: %.2f\n", acceleration);

        ret = clReleaseMemObject(arr_buffer);
        ret = clReleaseMemObject(pi_buffer);
        free(arr);
        free(arr_copy);
    }

    ret = clReleaseKernel(partition_kernel);
    ret = clReleaseKernel(quick_sort_kernel);
    ret = clReleaseKernel(merge_kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    free(source_str);

    return 0;
}
