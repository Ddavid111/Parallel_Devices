#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>


#define MAX_SOURCE_SIZE (0x100000)

int main() {
    // Méret paraméterek beállítása
    const int rows = 4;
    const int cols = 4;
    size_t datasize = sizeof(float) * rows * cols;

    // Bemeneti mátrix létrehozása és inicializálása
    float input_matrix[rows][cols];
    float transposed_matrix[cols][rows];
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            input_matrix[i][j] = i * cols + j; // Példa inicializálás, lehet módosítani
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
    cl_mem input_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &ret);
    cl_mem output_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, datasize, NULL, &ret);

    // Bemeneti mátrix másolása a bemeneti bufferbe
    ret = clEnqueueWriteBuffer(command_queue, input_buffer, CL_TRUE, 0, datasize, input_matrix, 0, NULL, NULL);

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

    cl_kernel kernel = clCreateKernel(program, "transpose", &ret);

    // Kernel argumentumainak beállítása
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&input_buffer);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&output_buffer);
    ret = clSetKernelArg(kernel, 2, sizeof(int), (void *)&rows);
    ret = clSetKernelArg(kernel, 3, sizeof(int), (void *)&cols);

    // Kernel futtatása
    size_t global_item_size[2] = {cols, rows}; // Output mérete
    size_t local_item_size[2] = {1, 1}; // Munkacsoport mérete
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);

    // Kimeneti mátrix másolása a host oldali memóriába
    ret = clEnqueueReadBuffer(command_queue, output_buffer, CL_TRUE, 0, datasize, transposed_matrix, 0, NULL, NULL);

    // Eredmény kiírása
    printf("Original Matrix:\n");
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            printf("%f ", input_matrix[i][j]);
        }
        printf("\n");
    }

    printf("Transposed Matrix:\n");
    for(int i = 0; i < cols; i++) {
        for(int j = 0; j < rows; j++) {
            printf("%f ", transposed_matrix[i][j]);
        }
        printf("\n");
    }

    // Resszurzusok felszabadítása
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(input_buffer);
    ret = clReleaseMemObject(output_buffer);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    free(kernel_source);

    return 0;
}
