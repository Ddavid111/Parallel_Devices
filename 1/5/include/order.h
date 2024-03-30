#ifndef VECTORS_H
#define VECTORS_H
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
typedef struct __attribute__ ((packed)) order_s{
    int order[100];
    int n;
}order_t;

typedef struct __attribute__ ((aligned(16))) float_order_s{
    float order[100];
    int n;
}float_order_t;
#endif