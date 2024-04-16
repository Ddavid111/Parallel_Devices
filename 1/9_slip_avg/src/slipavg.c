#include "utils.h"
#include "PlatformAndDevices.h"
#include "sliporder.h"
#include "SizeSpec.h"
#include "ClWrapper.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
void slipavg(){
    srand(1);
    ClWrapper_t cw = mlInit("kernels/slipavg.cl","slipavg");
    order_t o;
    for (int i=0; i<100; i++){
        o.order[i] = rand() % 101;
        o.avgs[i] = 0;
    }
    o.n = 100;
    o.sugar = 2;
    printf("listing order:\n");
    for (int i = 0; i < 100; i++)
    {
        printf("[%d]: %d, ", i, o.order[i]);
    }
    printf("\n");
    const int buffer_count = 1;
    const int input_param_count = 1;
    const int output_param_count = 1;
    cl_mem buffers[buffer_count];
    buffers[0] = clCreateBuffer(cw.context, CL_MEM_READ_WRITE, sizeof(order_t), NULL, NULL);

    mlInitKernel(cw.kernel, cw.context, buffers, buffer_count);

    cl_command_queue command_queue = clCreateCommandQueueWithProperties(cw.context, cw.pad.device_id, NULL, NULL);

    void * i_ptrs[input_param_count];
    i_ptrs[0] = &o;

    mlInputToDevice(command_queue, buffers, buffer_count, sizeof(order_t), i_ptrs);

    SizeSpec_t s = mlSizeSpecification1D(100,10);

    mlExecComandQueue(command_queue, cw, s);

    void * o_ptrs[output_param_count];
    o_ptrs[0] = &o;

    mlOutputFromDevice(command_queue, buffers, buffer_count,sizeof(order_t), o_ptrs);
    printf("Avarages in sugar: %d\n", o.sugar);
    for (int i = 0; i < 100; i++)
    {
        printf("[%d]: %.2f, ", i, o.avgs[i]);
    }
    mlReleaseResources(cw);
}