#include "kernel_loader.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_SOURCE_SIZE (0x100000)

cl_program load_program(cl_context context, cl_device_id device, const char *filename) {
    FILE *fp;
    char *source_str;
    size_t source_size;
    cl_int ret;
    cl_program program;

    fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char *)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
    ret = clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    free(source_str);

    return program;
}
