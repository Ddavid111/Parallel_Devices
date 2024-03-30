__kernel void quicksort(__global int* data, int left, int right) {
    int stack[32];
    int top = -1;

    stack[++top] = left;
    stack[++top] = right;

    while (top >= 0) {
        right = stack[top--];
        left = stack[top--];

        int i = left, j = right;
        int pivot = data[(left + right) / 2];

        // Partition
while (i <= j) {
    while (data[i] < pivot)
        i++;
    while (data[j] > pivot)
        j--;
    if (i <= j) {
        int temp = data[i];
        data[i] = data[j];
        data[j] = temp;
        i++;
        j--;
    }
}

// Push left side to stack
if (left < j) {
    stack[++top] = left;
    stack[++top] = j;
}

// Push right side to stack
if (i < right) {
    stack[++top] = i;
    stack[++top] = right;
}
    }
}
