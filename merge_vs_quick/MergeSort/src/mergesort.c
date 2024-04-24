#include <stdio.h>
#include <stdlib.h>
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#include <time.h>
#include "../include/kernel_loader.h"

#define MAX_SOURCE_SIZE (0x100000)

void merge_sort_seq(int* arr, const int low, const int high) {
    if (low < high) {
        int mid = low + (high - low) / 2;
        merge_sort_seq(arr, low, mid);
        merge_sort_seq(arr, mid + 1, high);

        // Merge
        int n1 = mid - low + 1;
        int n2 = high - mid;

        // Temporary arrays
        int *left = (int *)malloc(sizeof(int) * n1);
        int *right = (int *)malloc(sizeof(int) * n2);

        for (int i = 0; i < n1; i++)
            left[i] = arr[low + i];
        for (int j = 0; j < n2; j++)
            right[j] = arr[mid + 1 + j];

        // Merge the temporary arrays
        int i = 0, j = 0, k = low;
        while (i < n1 && j < n2) {
            if (left[i] <= right[j]) {
                arr[k] = left[i];
                i++;
            } else {
                arr[k] = right[j];
                j++;
            }
            k++;
        }

        // Copy remaining elements
        while (i < n1) {
            arr[k] = left[i];
            i++;
            k++;
        }
        while (j < n2) {
            arr[k] = right[j];
            j++;
            k++;
        }

        free(left);
        free(right);
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
    cl_kernel mergeToTemp_kernel;
    cl_kernel copyFromTemp_kernel;

    ret = clGetPlatformIDs(1, &platform_id, &num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_devices);
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);

    program = load_kernel(context, device_id, "kernels/mergesort.cl");

    mergeToTemp_kernel = clCreateKernel(program, "mergeToTemp", &ret);
    copyFromTemp_kernel = clCreateKernel(program, "copyFromTemp", &ret);

    clock_t parallel_time;
    double parallel_duration, seq_duration;

    for (int s = 0; s < num_sizes; ++s) {
        const int n = sizes[s];
        int *arr_copy = (int *)malloc(sizeof(int) * n);

        for (int i = 0; i < n; ++i) {
            arr_copy[i] = rand() % 1000;
        }

        clock_t seq_time = clock();
        merge_sort_seq(arr_copy, 0, n - 1);

        seq_duration = (double)(clock() - seq_time) / CLOCKS_PER_SEC;

        int temp_size = (n < 128) ? 128 : n;
        int *temp = (int *)malloc(sizeof(int) * temp_size);
        for (int i = 0; i < temp_size; ++i) {
            temp[i] = 0;
        }

        cl_mem temp_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * temp_size, temp, &ret);
        ret = clSetKernelArg(mergeToTemp_kernel, 5, sizeof(cl_mem), (void *)&temp_buffer);
        ret = clSetKernelArg(copyFromTemp_kernel, 5, sizeof(cl_mem), (void *)&temp_buffer);

        cl_mem arr_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * n, arr_copy, &ret);

        size_t global_item_size_mergeToTemp = n;
        size_t global_item_size_copyFromTemp = n;

        parallel_time = clock();
        ret = clEnqueueNDRangeKernel(command_queue, mergeToTemp_kernel, 1, NULL, &global_item_size_mergeToTemp, NULL, 0, NULL, NULL);
        ret = clEnqueueNDRangeKernel(command_queue, copyFromTemp_kernel, 1, NULL, &global_item_size_copyFromTemp, NULL, 0, NULL, NULL);

        ret = clFinish(command_queue);
        parallel_duration = (double)(clock() - parallel_time) / CLOCKS_PER_SEC;

        double acceleration = seq_duration / parallel_duration;

        printf("Array size: %d\n", n);
        printf("Sequential mergesort duration: %.2e seconds\n", seq_duration);
        printf("Parallel mergesort duration: %.2e seconds\n", parallel_duration);
        printf("Acceleration: %.2f\n", acceleration);

        ret = clReleaseMemObject(arr_buffer);
        free(arr_copy);
        free(temp);
        ret = clReleaseMemObject(temp_buffer);
    }

    ret = clReleaseKernel(mergeToTemp_kernel);
    ret = clReleaseKernel(copyFromTemp_kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    return 0;
}