#include "../include/kernel_loader.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_SOURCE_SIZE (0x100000)

cl_program load_kernel(cl_context context, cl_device_id device_id, const char *filename) {
    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char *)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    cl_int ret;
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to create program from source.\n");
        exit(1);
    }

    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Failed to build program.\n");
        exit(1);
    }

    free(source_str);
    return program;
}
