#include <stdio.h>
#define dx 0.04f
#define dy 0.08f

static const float L = -2.0f;
static const float R = 2.0f;
static const float D = -2.0f;
static const float U = 2.0f;

int is_dot (float x, float y) {
    float x_sq = x * x, y_sq = y *y;
    float lp = x_sq + y_sq - 1;
    lp *= lp * lp;
    float rp = x_sq * y_sq * y;
    return lp <= rp;
}

int main () {
    float x, y;
    for (y = U; y >= D; y -= dy, putchar('\n'))
        for (x = L; x <= R; x += dx)
            printf("\e[0;31m%c", is_dot(x, y) ? '*' : ' ');
    printf("\e[0m");
    return 0;
}
