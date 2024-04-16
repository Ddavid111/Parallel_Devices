#ifndef UTILS_H
#define UTILS_H
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#include "PlatformAndDevices.h"
#include "SizeSpec.h"
#include "ClWrapper.h"

cl_program mlProgramBuilder(const char *const path, cl_device_id device_id, cl_context context);
PlatformAndDevices_t mlGetPlatformAndDevices();
SizeSpec_t mlSizeSpecification1D(size_t x, size_t sub_x);
SizeSpec_t mlSizeSpecification2D(size_t x, size_t y, size_t sub_x, size_t sub_y);
ClWrapper_t mlInit(char * path, char * prg_name);
char* mlLoadKernelFromSource(const char* const path, cl_int* error_code);
void mlInitKernel(cl_kernel kernel, cl_context context, cl_mem buffers[], int buffer_size);
void mlInputToDevice(cl_command_queue command_queue, cl_mem input_buffers[], int buffer_count, size_t var_size, const void * ptr);
void mlOutputFromDevice(cl_command_queue command_queue, cl_mem output_buffers[], int buffer_count, size_t var_size, const void * ptr);
void mlExecComandQueue(cl_command_queue command_queue, ClWrapper_t cw, SizeSpec_t s);
void mlReleaseResources(ClWrapper_t cw);

void occurance();
#endif
