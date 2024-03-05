__kernel void matrix_multiply(__global float* A, __global float* B, __global float* C, const int rowsA, const int colsA, const int colsB) {
    int i = get_global_id(0);
    int j = get_global_id(1);

    float sum = 0.0f;
    for (int k = 0; k < colsA; ++k) {
        sum += A[i * colsA + k] * B[k * colsB + j];
    }

    C[i * colsB + j] = sum;
}
