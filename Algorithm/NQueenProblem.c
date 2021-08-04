#include <stdio.h>

static int n, res = 0, arr[32] = {0};

void NQueen(int layer) {
    if (layer < n) {
        int tmp, num = 0;
        for (int row = 0; row < layer; row++) {
            num |= arr[row];
            if ((tmp = arr[row] << (layer - row)) < (1 << n))
                num |= tmp;
            if ((tmp = arr[row] >> (layer - row)) > 0)
                num |= tmp;
        }
        for (int col = 0; col < n; col++) {
            if (!((num >> col) & 1)) {
                arr[layer] = 1 << col;
                NQueen(layer + 1);
            }
        }
    } else {
        res++;
    }
}

int main() {
    scanf("%d", &n);
    NQueen(0);
    printf("%d", res);
}