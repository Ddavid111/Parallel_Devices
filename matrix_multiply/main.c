#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#define MAX_SOURCE_SIZE (0x100000)

int main() {
    // Méret paraméterek beállítása
    const int rowsA = 4;
    const int colsA = 3;
    const int rowsB = 3;
    const int colsB = 4;
    size_t datasizeA = sizeof(float) * rowsA * colsA;
    size_t datasizeB = sizeof(float) * rowsB * colsB;

    // Bemeneti mátrixok létrehozása és inicializálása
    float A[rowsA][colsA];
    float B[rowsB][colsB];
    float C[rowsA][colsB];

    for(int i = 0; i < rowsA; i++) {
        for(int j = 0; j < colsA; j++) {
            A[i][j] = i * colsA + j; // Példa inicializálás, lehet módosítani
        }
    }

    for(int i = 0; i < rowsB; i++) {
        for(int j = 0; j < colsB; j++) {
            B[i][j] = i * colsB + j; // Példa inicializálás, lehet módosítani
        }
    }

    // OpenCL változók deklarálása
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint num_devices;
    cl_uint num_platforms;
    cl_int ret;

    // Platform és eszköz információk lekérdezése
    ret = clGetPlatformIDs(1, &platform_id, &num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_devices);

    // OpenCL kontextus létrehozása
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    // Parancssor létrehozása a kontextushoz
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // Bemeneti és kimeneti bufferek létrehozása a host oldalon
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY, datasizeA, NULL, &ret);
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY, datasizeB, NULL, &ret);
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * rowsA * colsB, NULL, &ret);

    // Bemeneti mátrixok másolása a bemeneti bufferekbe
    ret = clEnqueueWriteBuffer(command_queue, bufferA, CL_TRUE, 0, datasizeA, A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, bufferB, CL_TRUE, 0, datasizeB, B, 0, NULL, NULL);

    // OpenCL kernel betöltése
    FILE *kernel_file = fopen("sample.cl", "r");
    if (!kernel_file) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    char *kernel_source = (char *)malloc(MAX_SOURCE_SIZE);
    size_t kernel_size = fread(kernel_source, 1, MAX_SOURCE_SIZE, kernel_file);
    fclose(kernel_file);

    // Kernel létrehozása
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernel_source, (const size_t *)&kernel_size, &ret);
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    cl_kernel kernel = clCreateKernel(program, "matrix_multiply", &ret);

    // Kernel argumentumainak beállítása
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&bufferA);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bufferB);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&bufferC);
    ret = clSetKernelArg(kernel, 3, sizeof(int), (void *)&rowsA);
    ret = clSetKernelArg(kernel, 4, sizeof(int), (void *)&colsA);
    ret = clSetKernelArg(kernel, 5, sizeof(int), (void *)&colsB);

    // Kernel futtatása
    size_t global_item_size[2] = {rowsA, colsB}; // Output mérete (C mátrix mérete)
    size_t local_item_size[2] = {1, 1}; // Munkacsoport mérete
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);

    // Kimeneti mátrix másolása a host oldali memóriába
    ret = clEnqueueReadBuffer(command_queue, bufferC, CL_TRUE, 0, sizeof(float) * rowsA * colsB, C, 0, NULL, NULL);

    // Eredmény kiírása
    printf("Matrix A:\n");
    for(int i = 0; i < rowsA; i++) {
        for(int j = 0; j < colsA; j++) {
            printf("%f ", A[i][j]);
        }
        printf("\n");
    }

    printf("\nMatrix B:\n");
    for(int i = 0; i < rowsB; i++) {
        for(int j = 0; j < colsB; j++) {
            printf("%f ", B[i][j]);
        }
        printf("\n");
    }

    printf("\nResult Matrix C:\n");
    for(int i = 0; i < rowsA; i++) {
        for(int j = 0; j < colsB; j++) {
            printf("%f ", C[i][j]);
        }
        printf("\n");
    }

    // Resszurzusok felszabadítása
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(bufferA);
    ret = clReleaseMemObject(bufferB);
    ret = clReleaseMemObject(bufferC);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    free(kernel_source);

    return 0;
}
