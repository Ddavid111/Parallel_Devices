#ifndef KERNEL_LOADER_H
#define KERNEL_LOADER_H
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>

cl_program load_kernel(cl_context context, cl_device_id device_id, const char *filename);

#endif /* KERNEL_LOADER_H */
