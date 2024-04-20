#include "utils.h"
#include "PlatformAndDevices.h"
#include "order.h"
#include "SizeSpec.h"
#include "ClWrapper.h"

#include <stdio.h>
#include <stdlib.h>
void replace(){
    ClWrapper_t cw = mlInit("kernels/replace.cl","replace");

    float_order_t o;
    for (int i=0; i<99; i++){
      if(i % 2 == 0){
        o.order[i] = (float)i;
      } else {
        o.order[i] = (float)0;
      }
    }
    o.n = 100;
    printf("listing order:\n");
    for (int i = 0; i < 99; i++)
    {
        printf("[%d]: %.2f, ", i, o.order[i]);
    }
    const int buffer_count = 1;
    const int input_param_count = 1;
    const int output_param_count = 1;
    cl_mem buffers[buffer_count];
    buffers[0] = clCreateBuffer(cw.context, CL_MEM_READ_WRITE, sizeof(float_order_t), NULL, NULL);

    mlInitKernel(cw.kernel, cw.context, buffers, buffer_count);

    cl_command_queue command_queue = clCreateCommandQueueWithProperties(cw.context, cw.pad.device_id, NULL, NULL);

    void * i_ptrs[input_param_count];
    i_ptrs[0] = &o;

    mlInputToDevice(command_queue, buffers, buffer_count, sizeof(float_order_t), i_ptrs);

    SizeSpec_t s = mlSizeSpecification1D(100,10);

    mlExecComandQueue(command_queue, cw, s);

    void * o_ptrs[output_param_count];
    o_ptrs[0] = &o;

    mlOutputFromDevice(command_queue, buffers, buffer_count,sizeof(float_order_t), o_ptrs);
    printf("\nFixed array with generated elements:\n");
    for (int i = 0; i < 99; i++)
    {
        printf("[%d]: %.2f, ", i, o.order[i]);
    }
    mlReleaseResources(cw);
}