__kernel void partition(__global int* arr, const int low, const int high, __global int* pi) {
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    *pi = (i + 1);
}

__kernel void quick_sort(__global int* arr, const int low, int high, __global int* pi_buffer) {
    int stack[64];
    int top = -1;

    stack[++top] = low;
    stack[++top] = high;

    while (top >= 0) {
        high = stack[top--];
        int local_low = stack[top--];

        int pi;
        partition(arr, local_low, high, pi_buffer); 

        if (pi - 1 > local_low) {
            stack[++top] = local_low;
            stack[++top] = pi - 1;
        }

        if (pi + 1 < high) {
            stack[++top] = pi + 1;
            stack[++top] = high;
        }
    }
}

__kernel void merge(__global int* arr, const int low1, const int high1, const int low2, const int high2) {
    int temp[128]; 
    int i = low1, j = low2, k = 0;

    while (i <= high1 && j <= high2) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }

    while (i <= high1) {
        temp[k++] = arr[i++];
    }

    while (j <= high2) {
        temp[k++] = arr[j++];
    }

    for (i = low1, k = 0; i <= high2; i++, k++) {
        arr[i] = temp[k];
    }
}
