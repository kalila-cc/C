#include <stdio.h>
#define N 3

void disp(int *p,int length);
void swap(int *p1, int *p2);
void Permutation(int *arr,int len,int index);
static unsigned long long solution = 0;

int main() {
    int i, arr[N];
    for (i = 0; i < N; i++) arr[i] = i + 1;
    Permutation(arr, N, 0);
    getchar();
}

void Permutation(int *arr, int len, int index) {
    int i;
    if (index == len) {
        disp(arr, len);
    } else {
        for (i = index; i < len; i++) {
            swap(arr + index, arr + i);
            Permutation(arr, len, index + 1);
            swap(arr + index, arr + i);
        }
    }
}

void disp(int *p, int length) {
	int i;
    printf("%10llu: ", ++solution);
    for (i = 0; i < length; i++) printf("%d ",p[i]);
    printf("\n");
}

void swap(int *p1, int *p2) {
    int temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}
