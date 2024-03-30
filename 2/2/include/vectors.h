#ifndef VECTORS_H
#define VECTORS_H
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
typedef struct __attribute__ ((packed)) vector{
    int a[5];
    int b[5];
    int c[5];
    int n;
}Vector;
#endif