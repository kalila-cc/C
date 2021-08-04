#include <stdio.h>
#include <stdlib.h>

void Hanoi(int n, char a, char b, char c) {
    if (n == 1) {
        printf("%c -> %c\n", a, c); // let n-1 plates from a to c
    } else {
        Hanoi(n-1, a, c, b); // let n-1 plates from a to b
        Hanoi(1, a, b, c);   // let  1  plate  from a to c
        Hanoi(n-1, b, a, c); // let n-1 plates from b to c
    }
}

int main() {
    int n;
    scanf("%d", &n);
    Hanoi(n, 'a', 'b', 'c'); // from a to c by b
    system("PAUSE");
}
