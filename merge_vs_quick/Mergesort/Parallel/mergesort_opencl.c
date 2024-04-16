#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "kernel_loader.h" // Betöltési mechanizmus hozzáadása

#define MAX_SOURCE_SIZE (0x100000)

int main() {
    // Tömb méretének bekérése a felhasználótól
    int size;
    printf("Enter the number of elements in the array: ");
    scanf("%d", &size);

    // Tömb létrehozása és feltöltése véletlen számokkal
    int *data = (int *)malloc(size * sizeof(int));
    srand(time(NULL));
    printf("The elements of the array: ");
    for (int i = 0; i < size; i++) {
        data[i] = rand() % 100; // Véletlen számok 0 és 99 között
        printf("%d ", data[i]);
    }
    printf("\n");

    // OpenCL kernel betöltése a fájlból
    cl_int ret;
    char *kernel_source = load_kernel_source("kernels/mergesort_opencl.cl", &ret); // Kernel betöltése
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to load kernel source file.\n");
        exit(1);
    }

    // OpenCL platform, eszköz, illetve program inicializálása
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint num_platforms, num_devices;
    ret = clGetPlatformIDs(1, &platform_id, &num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_devices);

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &ret);

    // OpenCL bufferek inicializálása és feltöltése adatokkal
    cl_mem data_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, size * sizeof(int), NULL, &ret);
    ret = clEnqueueWriteBuffer(command_queue, data_buffer, CL_TRUE, 0, size * sizeof(int), data, 0, NULL, NULL);

    // OpenCL program inicializálása és kernel létrehozása
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernel_source, NULL, &ret);
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "merge_sort", &ret);

    // Kernel argumentumainak beállítása
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&data_buffer);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_int), (void *)&size);

    // Kernel futtatása
    size_t global_item_size = 1;
    size_t local_item_size = 1;
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    ret = clFinish(command_queue);

    // Rendezett adatok visszaolvasása
    ret = clEnqueueReadBuffer(command_queue, data_buffer, CL_TRUE, 0, size * sizeof(int), data, 0, NULL, NULL);

    // Eredmény kiírása
    printf("Sorted data: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");

    // OpenCL objektumok felszabadítása
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(data_buffer);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    free(kernel_source);
    free(data);

    return 0;
}
