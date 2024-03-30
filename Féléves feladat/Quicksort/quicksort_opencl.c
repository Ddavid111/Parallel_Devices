#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>

#define MAX_SOURCE_SIZE (0x100000)

int main() {

    int left, right;
    // Felhasználótól kérjük be a tömb méretét
    int size;
    printf("Adja meg a tömb méretét: ");
    scanf("%d", &size);

    // Tömb létrehozása és véletlenszerű feltöltése
    int *data = (int *)malloc(size * sizeof(int));
    printf("A tömb elemei: ");
    srand(time(NULL)); // srand hívása az aktuális idő alapján
    for (int i = 0; i < size; i++) {
        data[i] = rand() % 100; // Véletlenszerű számok 0 és 99 között
        printf("%d ", data[i]);
    }
    printf("\n");

    // OpenCL kernel betöltése a fájlból
    FILE *kernel_file;
    char *kernel_source;
    size_t kernel_source_size;

    kernel_file = fopen("quicksort_kernel.cl", "r");
    if (!kernel_file) {
        fprintf(stderr, "Nem sikerült betölteni a kernel fájlt.\n");
        exit(1);
    }

    kernel_source = (char *)malloc(MAX_SOURCE_SIZE);
    kernel_source_size = fread(kernel_source, 1, MAX_SOURCE_SIZE, kernel_file);
    fclose(kernel_file);

    // OpenCL platform, eszköz, illetve program inicializálása
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint num_platforms, num_devices;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &num_platforms);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült lekérni a platformokat. Hiba: %d\n", ret);
        exit(1);
    }
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_devices);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült lekérni a GPU eszközöket. Hiba: %d\n", ret);
        exit(1);
    }

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült létrehozni a kontextust. Hiba: %d\n", ret);
        exit(1);
    }
    cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült létrehozni a parancssort. Hiba: %d\n", ret);
        exit(1);
    }

    // OpenCL bufferek inicializálása és feltöltése adatokkal
    cl_mem data_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, size * sizeof(int), NULL, &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült létrehozni a buffert. Hiba: %d\n", ret);
        exit(1);
    }
    ret = clEnqueueWriteBuffer(command_queue, data_buffer, CL_TRUE, 0, size * sizeof(int), data, 0, NULL, NULL);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült feltölteni a buffert. Hiba: %d\n", ret);
        exit(1);
    }

    // OpenCL program inicializálása és kernel létrehozása
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernel_source, (const size_t *)&kernel_source_size, &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült létrehozni a programot a forráskódból. Hiba: %d\n", ret);
        exit(1);
    }
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült összeállítani a programot. Hiba: %d\n", ret);
        exit(1);
    }
    cl_kernel kernel = clCreateKernel(program, "quicksort", &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült létrehozni a kernelt. Hiba: %d\n", ret);
        exit(1);
    }

    // A felhasználótól beolvasott tömb méretét és a quicksort kernel argumentumait átadod a kernelnek
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&data_buffer);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült beállítani a 0. kernel argumentumot. Hiba: %d\n", ret);
        exit(1);
    }

    left = 0;  // Az első elem indexe
    right = size - 1;  // Az utolsó elem indexe

    ret = clSetKernelArg(kernel, 1, sizeof(cl_int), (void *)&left); // Helyezd el a quicksort kernel argumentumainak beállítását a left változó deklarációjának után
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült beállítani a 1. kernel argumentumot. Hiba: %d\n", ret);
        exit(1);
    }
    ret = clSetKernelArg(kernel, 2, sizeof(cl_int), (void *)&right); // Helyezd el a quicksort kernel argumentumainak beállítását a right változó deklarációjának után
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült beállítani a 2. kernel argumentumot. Hiba: %d\n", ret);
        exit(1);
    }

    // Kernel futtatása
    size_t global_item_size = 1;
    size_t local_item_size = 1;
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült futtatni a kernelt. Hiba: %d\n", ret);
        exit(1);
    }
    ret = clFinish(command_queue);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült befejezni a parancssort. Hiba: %d\n", ret);
        exit(1);
    }

    // Rendezett adatok visszaolvasása
    ret = clEnqueueReadBuffer(command_queue, data_buffer, CL_TRUE, 0, size * sizeof(int), data, 0, NULL, NULL);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült visszaolvasni a buffert. Hiba: %d\n", ret);
        exit(1);
    }

    // Eredmény kiírása
    printf("Rendezett adatok: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");

    // OpenCL objektumok felszabadítása
    ret = clFlush(command_queue);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült kiüríteni a parancssort. Hiba: %d\n", ret);
        exit(1);
    }
    ret = clFinish(command_queue);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült befejezni a parancssort. Hiba: %d\n", ret);
        exit(1);
    }
    ret = clReleaseKernel(kernel);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült felszabadítani a kernelt. Hiba: %d\n", ret);
        exit(1);
    }
    ret = clReleaseProgram(program);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült felszabadítani a programot. Hiba: %d\n", ret);
        exit(1);
    }
    ret = clReleaseMemObject(data_buffer);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült felszabadítani a buffert. Hiba: %d\n", ret);
        exit(1);
    }
    ret = clReleaseCommandQueue(command_queue);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült felszabadítani a parancssort. Hiba: %d\n", ret);
        exit(1);
    }
    ret = clReleaseContext(context);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Nem sikerült felszabadítani a kontextust. Hiba: %d\n", ret);
        exit(1);
    }

    free(data);
    free(kernel_source);

    return 0;
}
