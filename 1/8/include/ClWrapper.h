#ifndef CL_WRAPPER_H
#define CL_WRAPPER_H
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#include "PlatformAndDevices.h"
typedef struct ClWrapper_s
{
  PlatformAndDevices_t pad;
  cl_context context;
  cl_program program;
  cl_kernel kernel;
} ClWrapper_t;
#endif