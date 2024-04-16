#ifndef SIZE_SPEC_H
#define SIZE_SPEC_H
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
typedef struct SizeSpec_s
{
  size_t* global_size;
  size_t* local_size;
  cl_uint dim;
} SizeSpec_t;
#endif