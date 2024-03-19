__kernel void quicksort(__global int *arr, const int left, const int right, const int max_depth) {
    int stack[32];
    int top = -1;

    // Push initial subarray onto the stack
    stack[++top] = left;
    stack[++top] = right;

    // Loop until stack is empty or max depth is reached
    while (top >= 0) {
        // Pop subarray from stack
        int high = stack[top--];
        int low = stack[top--];

        // Partition the subarray
        int pivot = arr[high];
        int i = low - 1;

        for (int j = low; j < high; j++) {
            if (arr[j] <= pivot) {
                i++;
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }

        int temp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = temp;

        // Push left subarray onto stack
        if (i > low && (top / 2) < max_depth) {
            stack[++top] = low;
            stack[++top] = i;
        }

        // Push right subarray onto stack
        if (i + 2 < high && (top / 2) < max_depth) {
            stack[++top] = i + 2;
            stack[++top] = high;
        }
    }
}
