#ifndef PLATFORM_DEVICES_H
#define PLATFORM_DEVICES_H
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
typedef struct PlatformAndDevices_s
{
    cl_uint n_devices;
    cl_device_id device_id;
} PlatformAndDevices_t;
#endif