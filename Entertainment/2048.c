#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <io.h>
#include <direct.h>
#include <windows.h>
#include <conio.h>

#define bool int
#define true 1
#define false 0
#define LEFT 'L'
#define RIGHT 'R'
#define UP 'U'
#define DOWN 'D'
#define QUIT 'Q'
#define SAVE 'S'
#define DISP 'D'
#define pass ;
#define N 4

int cmp(const void *pa, const void *pb);
void save();
void load();
void show();
void init();
void move(char ori);
void clear();
void start();
void reset();
void rankings(char mode);
void rand_choice();
char watch_orientation();
bool rest();
bool complete();

int score, board[N][N];
char dir_name[] = "./config";
char rankings_name[] = "./config/.rankings.cfg";
char board_name[] = "./config/.board.cfg";
bool moved = true;

typedef struct tagPerson {
    char _name[20];
    int _score;
} Person;

bool rest () {
    int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            if (board[i][j] == 0) return true;
    return false;
}

int cmp (const void *pa, const void *pb) {
    return (((Person *)pb)->_score - ((Person *)pa)->_score);
}

char watch_orientation () {
    Sleep(250);
    while (1) {
        if (GetAsyncKeyState(VK_UP) & 0x8000) return UP;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000) return DOWN;
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) return LEFT;
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) return RIGHT;
        if ((GetAsyncKeyState('q') & 0x8000) || (GetAsyncKeyState('Q') & 0x8000)) return QUIT;
        if ((GetAsyncKeyState('s') & 0x8000) || (GetAsyncKeyState('S') & 0x8000)) return SAVE;
    }
}

void rand_choice () {
    int i, j;
    if (!rest()) return;
    i = rand() % N;
    j = rand() % N;
    while (board[i][j]) {
        i = rand() % N;
        j = rand() % N;
    }
    board[i][j] = rand() % 4 ? 2 : 4;
}

void move (char ori) {
    int i, j, k, ctr;
    switch (ori) {
    case UP:
        for (j = 0; j < N; j++) {
            for (ctr = 0, i = 0; i < N; i++) {
                while (i < N && board[i][j] == 0) i++;
                k = i + 1;
                while (k < N && board[k][j] == 0) k++;
                if (i < N && k < N) {
                    if (board[i][j] == board[k][j]) {
                        board[ctr][j] = board[i][j] << 1;
                        if (i != ctr) board[i][j] = 0;
                        board[k][j] = 0;
                        i = k;
                        score += board[ctr][j];
                        moved = true;
                    } else {
                        board[ctr][j] = board[i][j];
                        board[ctr + 1][j] = board[k][j];
                        if (i != ctr && i != ctr + 1) {
                            moved = true;
                            board[i][j] = 0;
                        }
                        if (k != ctr && k != ctr + 1) {
                            moved = true;
                            board[k][j] = 0;
                        }
                        i = ctr;
                    }
                    ctr++;
                } else if (i < N) {
                    board[ctr][j] = board[i][j];
                    if (i != ctr) {
                        moved = true;
                        board[i][j] = 0;
                    }
                    ctr++;
                    i = k;
                }
            }
        }
        break;
    case DOWN:
        for (j = N - 1; j >= 0; j--) {
            for (ctr = N - 1, i = N - 1; i >= 0; i--) {
                while (i >= 0 && board[i][j] == 0) i--;
                k = i - 1;
                while (k >= 0 && board[k][j] == 0) k--;
                if (i >= 0 && k >= 0) {
                    if (board[i][j] == board[k][j]) {
                        board[ctr][j] = board[i][j] << 1;
                        if (i != ctr) board[i][j] = 0;
                        board[k][j] = 0;
                        i = k;
                        score += board[ctr][j];
                        moved = true;
                    } else {
                        board[ctr][j] = board[i][j];
                        board[ctr - 1][j] = board[k][j];
                        if (i != ctr && i != ctr - 1) {
                            moved = true;
                            board[i][j] = 0;
                        }
                        if (k != ctr && k != ctr - 1) {
                            moved = true;
                            board[k][j] = 0;
                        }
                        i = ctr;
                    }
                    ctr--;
                } else if (i >= 0) {
                    board[ctr][j] = board[i][j];
                    if (i != ctr) {
                        moved = true;
                        board[i][j] = 0;
                    }
                    ctr--;
                    i = k;
                }
            }
        }
        break;
    case LEFT:
        for (i = 0; i < N; i++) {
            for (ctr = 0, j = 0; j < N; j++) {
                while (j < N && board[i][j] == 0) j++;
                k = j + 1;
                while (k < N && board[i][k] == 0) k++;
                if (j < N && k < N) {
                    if (board[i][j] == board[i][k]) {
                        board[i][ctr] = board[i][j] << 1;
                        if (j != ctr) board[i][j] = 0;
                        board[i][k] = 0;
                        j = k;
                        score += board[i][ctr];
                        moved = true;
                    } else {
                        board[i][ctr] = board[i][j];
                        board[i][ctr + 1] = board[i][k];
                        if (j != ctr && j != ctr + 1) {
                            moved = true;
                            board[i][j] = 0;
                        }
                        if (k != ctr && k != ctr + 1) {
                            moved = true;
                            board[i][k] = 0;
                        }
                        j = ctr;
                    }
                    ctr++;
                } else if (j < N) {
                    board[i][ctr] = board[i][j];
                    if (j != ctr) {
                        moved = true;
                        board[i][j] = 0;
                    }
                    ctr++;
                    j = k;
                }
            }
        }
        break;
    case RIGHT:
        for (i = N - 1; i >= 0; i--) {
            for (ctr = N - 1, j = N - 1; j >= 0; j--) {
                while (j >= 0 && board[i][j] == 0) j--;
                k = j - 1;
                while (k >= 0 && board[i][k] == 0) k--;
                if (j >= 0 && k >= 0) {
                    if (board[i][j] == board[i][k]) {
                        board[i][ctr] = board[i][j] << 1;
                        if (j != ctr) board[i][j] = 0;
                        board[i][k] = 0;
                        j = k;
                        score += board[i][ctr];
                        moved = true;
                    } else {
                        board[i][ctr] = board[i][j];
                        board[i][ctr - 1] = board[i][k];
                        if (j != ctr && j != ctr - 1) {
                            moved = true;
                            board[i][j] = 0;
                        }
                        if (k != ctr && k != ctr - 1) {
                            moved = true;
                            board[i][k] = 0;
                        }
                            
                        j = ctr;
                    }
                    ctr--;
                } else if (j >= 0) {
                    board[i][ctr] = board[i][j];
                    if (j != ctr) {
                        moved = true;
                        board[i][j] = 0;
                    }
                    ctr--;
                    j = k;
                }
            }
        }
        break;
    }
}

bool complete () {
    int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            if (board[i][j] == 0) return false;
    for (i = 0; i < N; i++)
        for (j = 1; j < N; j++)
            if (board[i][j - 1] == board[i][j]) return false;
    for (j = 0; j < N; j++)
        for (i = 1; i < N; i++)
            if (board[i - 1][j] == board[i][j]) return false;
    return true;
}

void show () {
    int i, j, k;
    system("cls");
    printf("(press q/Q to quit, press s/S to save)\n");
    for (printf("-"), i = 0; i < N; i++) printf("-------");
    for (printf("\n"), i = 0; i < N; i++, printf("\n")) {
        for (printf(board[i][0] > 0 ? "| %4d |" : "|      |", board[i][0]), j = 1; j < N; j++)
            printf(board[i][j] > 0 ? " %4d |" : "      |", board[i][j]);
        if (i >= N - 1) continue;
        for (printf("\n-"), k = 0; k < N; k++)
            printf("-------");
    }
    for (printf("-"), i = 0; i < N; i++) printf("-------");
    printf("\a\nscore : %d\n", score);
}

void reset () {
    int i, j;
    for (i = 0; i < N; i++)
        for(j = 0; j < N; j++)
            board[i][j] = 0;
    score = 0;
}

void init () {
    int i, j;
    FILE *fp;
    srand((unsigned)time(NULL));
    reset();
    if (access(dir_name, 0)) mkdir(dir_name);
    if (access(rankings_name, 0)) {
        fp = fopen(rankings_name, "w");
        fprintf(fp, "RANKINGS: 0");
        fclose(fp);
    }
    if (access(board_name, 0)) {
        fp = fopen(board_name, "w");
        fprintf(fp, "SAVE: 0\n");
        fclose(fp);
    }
}

void rankings (char mode) {
    int i, num, _score;
    char _name[20];
    Person *people;
    FILE *fp;
    if (mode == DISP) {
        fp = fopen(rankings_name, "r");
        fscanf(fp, "RANKINGS: %d", &num);
        system("cls");
        printf("----- Rankings -----\n");
        if (num == 0) {
            printf("\nThere haven't recorded any score\n");
        } else {
            for (i = 0; i < num; i++) {
                fscanf(fp, "%s %d", _name, &_score);
                printf("%s\t\t%d\n", _name, _score);
            }
        }
        fclose(fp);
        system("PAUSE");
    } else if (mode == SAVE) {
        fp = fopen(rankings_name, "r");
        fscanf(fp, "RANKINGS: %d", &num);
        people = (Person *)calloc(num + 1, sizeof(Person));
        for (i = 0; i < num; i++) fscanf(fp, "%s %d", people[i]._name, &people[i]._score);
        printf("Please input your name: \n");
        scanf("%s", people[num]._name);
        people[num]._score = score;
        qsort(people, num + 1, sizeof(Person), cmp);
        fclose(fp);
        fp = fopen(rankings_name, "w");
        fprintf(fp, "RANKINGS: %d\n", num + 1);
        for (i = 0; i < num + 1; i++) fprintf(fp, "%s %d\n", people[i]._name, people[i]._score);
        fclose(fp);
    }
}

void load () {
    int i, j;
    bool saved;
    FILE *fp = fopen(board_name, "r");
    fscanf(fp, "SAVED: %d", &saved);
    if (!saved) {
        printf("\n\nYou don't have any game saved!\n");
        fclose(fp);
        system("PAUSE");
    } else {
        for (i = 0; i < N; i++)
            for(j = 0; j < N; j++)
                fscanf(fp, "%d", &board[i][j]);
        fscanf(fp, "%d", &score);
        fclose(fp);
        start();
    }
}

void save () {
    int i, j;
    FILE *fp = fopen(board_name, "w");
    fprintf(fp, "SAVED: 1\n");
    for (i = 0; i < N; i++, fprintf(fp, "\n"))
        for (j = 0; j < N; j++)
            fprintf(fp, "%d ", board[i][j]);
    fprintf(fp, "%d\n", score);
    fclose(fp);
}

void start () {
    char ori;
    while (!complete()) {
        if (moved) {
            rand_choice();
            moved = false;
        }
        show();
        ori = watch_orientation();
        if (ori == QUIT) {
            printf("\n\nYou have terminated the game!\n");
            return ;
        } else if (ori == SAVE) {
            save();
            printf("\n\nGame progress saved successfully!\n");
            return ;
        } else {
            move(ori);
        }
    }
    rankings(SAVE);
}

void clear () {
    char choice;
    FILE *fp;
    printf("What do you want to clear?\n1. rankings\t2. backups\t3.back to main menu\n");
    choice = getch();
    while (choice != '1' && choice != '2' && choice != '3') {
        printf("\n\nIllegal input!\n");
        choice = getch();
    }
    if (choice == '1') {
        fp = fopen(rankings_name, "w");
        fprintf(fp, "RANKINGS: 0\n");
        fclose(fp);
    } else if (choice == '2') {
        fp = fopen(board_name, "w");
        fprintf(fp, "SAVED: 0\n");
        fclose(fp);
    } else if (choice == '3') {
        return;
    }
    printf("\n\nClear over!\n");
}

int main () {
    char ori, choice;
    init();
    printf("Welcome to 2048 Game!\n");
    system("PAUSE");
    while (1) {
        system("cls");
        printf("Please choose an item beneath:\n\n");
        printf("1. start a new game\n");
        printf("2. see the rankings\n");
        printf("3. load the previous game\n");
        printf("4. clear cache\n");
        printf("5. exit game\n");
        choice = getch();
        while (choice < '1' || choice > '5') {
            printf("Please enter a number between 1 and 5\n");
            choice = getch();
        }
        switch (choice) {
        case '1':
            reset();
            start();
            break;
        case '2':
            rankings(DISP);
            break;
        case '3':
            load();
            break;
        case '4':
            clear();
            break;
        case '5':
            printf("\n\nGood bye!\n");
            return 0;
        }
    }
}
