// Kernel függvény az összefésülésre
__kernel void mergeToTemp(__global int* arr, const int low1, const int high1, const int low2, const int high2, __global int* temp) {
    int input_size = high2 - low1 + 1; // A bemeneti adatok teljes mérete

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
}

// Kernel függvény a temp tömbből való másolásra az arr tömbbe
__kernel void copyFromTemp(__global int* arr, const int low1, const int high2, __global int* temp) {
    int i = low1, k = 0;

    for (i = low1, k = 0; i <= high2; i++, k++) {
        arr[i] = temp[k];
    }
}
