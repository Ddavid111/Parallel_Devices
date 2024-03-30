
#define MAX_SIZE 1000 // Vagy bármilyen más érték, ami elég a céljainkhoz

__kernel void merge_sort(__global int* data, const int n) {
    int temp[MAX_SIZE]; 

    for (int size = 1; size < n; size *= 2) {
        for (int left_start = 0; left_start < n - 1; left_start += 2 * size) {
            int mid = left_start + size - 1;
            int right_end = left_start + 2 * size - 1;
            if (right_end >= n) {
                right_end = n - 1;
            }
            int left = left_start;
            int right = mid + 1;
            int k = left_start;

            while (left <= mid && right <= right_end) {
                if (data[left] <= data[right]) {
                    temp[k++] = data[left++];
                } else {
                    temp[k++] = data[right++];
                }
            }
            while (left <= mid) {
                temp[k++] = data[left++];
            }
            while (right <= right_end) {
                temp[k++] = data[right++];
            }
            for (int i = left_start; i <= right_end; i++) {
                data[i] = temp[i];
            }
        }
    }
}
