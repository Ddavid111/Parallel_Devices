#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Összefésülő rendezés
void merge(int arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(int arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

int main() {
    int n;
    printf("Kerem a tomb meretet: ");
    scanf("%d", &n);

    // Tömb létrehozása és véletlen számokkal való feltöltése
    int arr[n];
    srand(time(NULL));
    printf("A veletlen szamokkal feltoltott tomb:\n");
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100; // Véletlen számok 0-99 között
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Összefésülő rendezés
    clock_t start = clock();
    mergeSort(arr, 0, n - 1);
    clock_t end = clock();

    // Rendezett tömb kiírása
    printf("A rendezett tomb:\n");
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");

    // Rendezési idő kiírása
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("A rendezes ideje: %f masodperc.\n", time_spent);

    return 0;
}
