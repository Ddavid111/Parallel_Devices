__kernel void transpose(__global float* input, __global float* output, const int rows, const int cols) {
    int i = get_global_id(0);
    int j = get_global_id(1);

    output[i * rows + j] = input[j * cols + i];
}
