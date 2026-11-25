#include <stdio.h>

int main(void) {
    int arr[100];

    for (int j = 0; j < 100; j++) {
        arr[j] = j + 21;
    }

    int *p = arr;

    for (int j = 0; j < 100; j++) {
        printf("Address: %p | Value: %d\n",
               (void *)(p + j), *(p + j));
    }

    return 0;
}
