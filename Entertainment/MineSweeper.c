#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N 12
#define BOOM_NUM 15

unsigned char map[N][N];
int x, y, marker, mark[N][N], hint[N][N];

int getBoom() {
    int i, j, res = 0;
    for (i = x - 1; i <= x + 1; i++)
        for (j = y - 1; j <= y + 1; j++)
            if (i >= 0 && i < N && j >= 0 && j < N && map[i][j] == '*')
                res++;
    hint[x][y] = res;
    return res;
}

void extend() {
    int i, j, ti, tj;
    for (i = x - 1; i <= x + 1; i++) {
        for (j = y - 1; j <= y + 1; j++) {
            if (i >= 0 && i < N && j >= 0 && j < N && !(i == x && j == y) && hint[i][j] == -1) {
                ti = x, tj = y;
                x = i, y = j;
                map[x][y] = ' ';
                if (!getBoom()) extend();
                x = ti, y = tj;
            }
        }
    }
}

int win() {
    int i, j, num = 0;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            if (hint[i][j] < 0)
                num++;
    return num == BOOM_NUM;
}

void init() {
    int i, j;
    srand((unsigned)time(NULL));
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            map[i][j] = '\1';
            mark[i][j] = 0;
            hint[i][j] = -1;
        }
    }
    for (i = 0; i < BOOM_NUM;) {
        x = rand() % N;
        y = rand() % N;
        if (map[x][y] == '\1') {
            map[x][y] = '*';
            i++;
        }
    }
    do {
        x = rand() % N, y = rand() % N;
    } while (map[x][y] == '*');
    map[x][y] = ' ';
    marker = 0;
    if (!getBoom()) extend();
}

void operate(unsigned char c) {
    if (c == '\x20') { // mark
        mark[x][y] = 1 - mark[x][y];
        marker += mark[x][y] ? 1 : -1;
    } else if (c == '\x0D' && map[x][y] == '*') { // check
        marker = -1;
    } else {
        map[x][y] = ' ';
        if (!getBoom()) extend();
    }
}

void update() {
    int i, j;
    unsigned char c;
    system("cls");
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (i == x && j == y) c = '#';
            else if (hint[i][j] > 0) c = '0' + hint[i][j];
            else if (map[i][j] != ' ' && mark[i][j]) c = '+';
            else if ((c = map[i][j]) == '*') c = '\1';
            else c = map[i][j];
            printf(" %c ", c);
        }
        printf("\n");
    }
    printf(
        "\n\n"
        "Press space to add/remove marker\n"
        "Press direction keys to move\n"
        "Press enter to check\n"
        "\n"
        "Matrix : %d X %d\n"
        "Boom number : %d\n"
        "Marker number : %d\n",
        N, N, BOOM_NUM, marker
    );
}

void move(unsigned char c) {
    if (c == '\x48' && x > 0) // up 72
        x--;
    else if (c == '\x50' && x < N - 1) // down 80
        x++;
    else if (c == '\x4B' && y > 0) // left 75
        y--;
    else if (c == '\x4D' && y < N - 1) // right 77
        y++;
}

int main() {
    int i, j;
    unsigned char c;
reload:
    init();
    while (marker >= 0 && !win()) {
        update();
        if ((c = getch()) == 0xE0) move(c = getch());
        else operate(c);
    }
    system("cls");
    if (marker < 0) {
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) printf((i == x && j == y) ? " X " : " %c ", map[i][j]);
            printf("\n");
        }
        printf("\n\nYou've been dead!\n");
    } else {
        printf("\n\nCongratulations! You win!\n");
    }
    printf(
        "\n\n"
        "Would you like to play it again?\n"
        "[y|Y or n|N]\n"
    );
    while (1) {
        fflush(stdin);
        c = getch();
        if (c != 'y' && c != 'Y' && c != 'n' && c != 'N') {
            printf("\nPlease input [y|Y or n|N]!\n");
            continue;
        }
        break;
    }
    if (c == 'y' || c == 'Y') goto reload;
    else printf("\n\nGood bye!\n\n");
    system("pause");
}
