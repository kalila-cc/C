/* coding: utf-0 */
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// processing function declaration
char transfunc(int a);
void historyfunc();
void deffunc(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
void displayfunc(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
void simplify(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
void randfunc(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int judgefunc(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
// operation function declaration
int func_F(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_f(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_U(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_u(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_R(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_r(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_L(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_l(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_B(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_b(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_D(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_d(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_M(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_m(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_E(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_e(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_W(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_w(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_S(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_X(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_Z(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
int func_Y(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]);
// variable declaration
int did;
int step = 0;
int count = 0;
char operate;
char track[2048];
const char operate_set[] = {
    'F', 'f', 'B', 'b',
    'U', 'u', 'D', 'd',
    'L', 'l', 'R', 'r',
    'M', 'm', 'E', 'e',
    'W', 'w', 'S', 'X',
    'Z', 'Y'
};
// main function definition
int main() {
    int i, j, k;
    int choose;
    int L[4][4], R[4][4], U[4][4], D[4][4], F[4][4], B[4][4];
    srand((unsigned)time(NULL));
    char judge = 'a';
    printf(
        "欢迎进入魔方游戏：\n"
        "版本：v2.0.20200921\n\n"
    );
    deffunc(U, F, R, D, B, L);
    displayfunc(U, F, R, D, B, L);
    do {
        printf("\n请选择操作：1.旋转魔方  2.打乱魔方  3.恢复初始  4.退出游戏\n");
        scanf("%d", &choose);
        if (choose == 1) {
            step = 0;
            do {
                if (!count) {
                    printf("请先打乱魔方再进行操作\n");
                } else if (judgefunc(U, F, R, D, B, L)) {
                    printf("恭喜成功还原魔方，共用了 %d 步\n", count);
                    break;
                }
                printf(
                    "请输入操作码：\n"
                    "F/f 正面顺/逆时针旋转90度  B/b 背面顺/逆时针旋转90度\n"
                    "U/u 顶面顺/逆时针旋转90度  D/d 底面顺/逆时针旋转90度\n"
                    "L/l 左面顺/逆时针旋转90度  R/r 右面顺/逆时针旋转90度\n"
                    "M/m 左面右面之间的面顺/逆时针旋转90度\n"
                    "E/e 顶面底面之间的面顺/逆时针旋转90度\n"
                    "W/w 正面背面之间的面顺/逆时针旋转90度\n"
                    "S/X 转动魔方使正面转为顶面/底面\n"
                    "Z/Y 转动魔方使正面转为左面/右面\n"
                    "(输入\'#\'退出操作)\n"
                );
                fflush(stdin);
                operate = getchar();
                simplify(U, F, R, D, B, L);
                system("cls");
                displayfunc(U, F, R, D, B, L);
                historyfunc();
            } while (operate != '#');
        } else if (choose == 2) {
            randfunc(U, F, R, D, B, L);
            system("cls");
            displayfunc(U, F, R, D, B, L);
            printf("魔方已成功打乱\n");
        } else if (choose == 3) {
            deffunc(U, F, R, D, B, L);
            system("cls");
            displayfunc(U, F, R, D, B, L);
            printf("魔方已成功还原为最初状态\n");
        } else if (choose == 4) {
            printf("欢迎下次使用，再见\n");
            break;
        } else {
            printf("你选择了错误的选项，默认不进行操作\n");
        }
    } while (1);
    fflush(stdin);
    getchar();
    return 0;
}
// processing function definition
char transfunc(int a) {
    return operate_set[a - 1];
}

void historyfunc() {
    int i;
    if (did) {
        track[step] = operate;
    }
    printf("以下为操作历史记录：\n");
    for (i = 1; i <= step; i++) {
        printf("%c ", track[i]);
        if (i % 20 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void deffunc(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, j;
    for (i = 1; i <= 3; i++) {
        for (j = 1; j <= 3; j++) {
            U[i][j] = 1;
            F[i][j] = 2;
            R[i][j] = 3;
            D[i][j] = 4;
            B[i][j] = 5;
            L[i][j] = 6;
        }
    }
}

void displayfunc(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    printf("魔方状态如下：\n");
    printf("\n\t              %d                           %d       %d", U[3][1], L[3][3], B[3][1]);                      // 1
    printf("\n\t          %d       %d", U[2][1], U[3][2]);  // 2
    printf("\n\t      %d       %d       %d               %d   %d       %d   %d", U[1][1], U[2][2], U[3][3], L[3][2], L[2][3], B[2][1], B[3][2]);  // 3
    printf("\n\t          %d       %d", U[1][2], U[2][3]);                  // 4
    printf(
        "\n\t  %d           %d           %d       %d   %d   %d       %d   %d   %d", F[3][1], U[1][3], R[3][3], L[3][1], L[2][2], L[1][3], B[1][1], B[2][2], B[3][3]);    // 5
    printf("\n\t");  // 6
    printf("\n\t  %d   %d               %d   %d       %d   %d               %d   %d", F[2][1], F[3][2], R[3][2], R[2][3], L[2][1], L[1][2], B[1][2], B[2][3]);    // 7
    printf("\n\t");  // 8
    printf("\n\t  %d   %d   %d       %d   %d   %d       %d           %d           %d", F[1][1], F[2][2], F[3][3], R[3][1], R[2][2], R[1][3], L[1][1], D[3][1], B[1][3]);  // 9
    printf("\n\t                                          %d       %d", D[2][1], D[3][2]);  // 10
    printf("\n\t      %d   %d       %d   %d               %d       %d       %d", F[1][2], F[2][3], R[2][1], R[1][2], D[1][1], D[2][2], D[3][3]);  // 11
    printf("\n\t                                          %d       %d", D[1][2], D[2][3]);  // 12
    printf("\n\t          %d       %d                           %d\n\n", F[1][3], R[1][1], D[1][3]);  // 13
}

void simplify(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    count++;
    step++;
    did = 1;
    switch (operate) {
        case 'F':
            func_F(U, F, R, D, B, L);
            break;
        case 'f':
            func_f(U, F, R, D, B, L);
            break;
        case 'B':
            func_B(U, F, R, D, B, L);
            break;
        case 'b':
            func_b(U, F, R, D, B, L);
            break;
        case 'U':
            func_U(U, F, R, D, B, L);
            break;
        case 'u':
            func_u(U, F, R, D, B, L);
            break;
        case 'D':
            func_D(U, F, R, D, B, L);
            break;
        case 'd':
            func_d(U, F, R, D, B, L);
            break;
        case 'L':
            func_L(U, F, R, D, B, L);
            break;
        case 'l':
            func_l(U, F, R, D, B, L);
            break;
        case 'R':
            func_R(U, F, R, D, B, L);
            break;
        case 'r':
            func_r(U, F, R, D, B, L);
            break;
        case 'M':
            func_M(U, F, R, D, B, L);
            break;
        case 'm':
            func_m(U, F, R, D, B, L);
            break;
        case 'E':
            func_E(U, F, R, D, B, L);
            break;
        case 'e':
            func_e(U, F, R, D, B, L);
            break;
        case 'W':
            func_W(U, F, R, D, B, L);
            break;
        case 'w':
            func_w(U, F, R, D, B, L);
            break;
        case 'S':
            func_S(U, F, R, D, B, L);
            break;
        case 'X':
            func_X(U, F, R, D, B, L);
            break;
        case 'Z':
            func_Z(U, F, R, D, B, L);
            break;
        case 'Y':
            func_Y(U, F, R, D, B, L);
            break;
        default:
            count--;
            step--;
            did = 0;
            break;
    }
}

void randfunc(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, a;
    srand((unsigned)time(NULL));
    for (i = 0; i < 20; i++) {
        a = rand() % 22 + 1;
        operate = transfunc(a);
        simplify(U, F, R, D, B, L);
    }
    displayfunc(U, F, R, D, B, L);
}

int judgefunc(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, j, temp, flag = 0;
    temp = 0;
    for (i = 1; i <= 3; i++) {
        for (j = 1; j <= 3; j++) {
            temp += U[i][j];
            if (temp != ((i - 1) * 3 + j) * U[1][1]) {
                flag = 1;
                break;
            }
        }
    }
    if (flag)
        return 0;
    temp = 0;
    for (i = 1; i <= 3; i++) {
        for (j = 1; j <= 3; j++) {
            temp += F[i][j];
            if (temp != ((i - 1) * 3 + j) * F[1][1]) {
                flag++;
                break;
            }
        }
    }
    if (flag)
        return 0;
    temp = 0;
    for (i = 1; i <= 3; i++) {
        for (j = 1; j <= 3; j++) {
            temp += R[i][j];
            if (temp != ((i - 1) * 3 + j) * R[1][1]) {
                flag++;
                break;
            }
        }
    }
    if (flag)
        return 0;
    temp = 0;
    for (i = 1; i <= 3; i++) {
        for (j = 1; j <= 3; j++) {
            temp += D[i][j];
            if (temp != ((i - 1) * 3 + j) * D[1][1]) {
                flag++;
                break;
            }
        }
    }
    if (flag)
        return 0;
    temp = 0;
    for (i = 1; i <= 3; i++) {
        for (j = 1; j <= 3; j++) {
            temp += B[i][j];
            if (temp != ((i - 1) * 3 + j) * B[1][1]) {
                flag++;
                break;
            }
        }
    }
    if (flag)
        return 0;
    return 1;
}
// operation function
int func_F(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 2; i++) {
        temp = F[3][i];
        F[3][i] = F[i][1];
        F[i][1] = F[1][4 - i];
        F[1][4 - i] = F[4 - i][3];
        F[4 - i][3] = temp;
    }
    for (i = 1; i <= 3; i++) {
        temp = U[1][i];
        U[1][i] = L[i][1];
        L[i][1] = D[1][4 - i];
        D[1][4 - i] = R[4 - i][1];
        R[4 - i][1] = temp;
    }
    return 0;
}

int func_f(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 2; i++) {
        temp = F[4 - i][3];
        F[4 - i][3] = F[1][4 - i];
        F[1][4 - i] = F[i][1];
        F[i][1] = F[3][i];
        F[3][i] = temp;
    }
    for (i = 1; i <= 3; i++) {
        temp = R[4 - i][1];
        R[4 - i][1] = D[1][4 - i];
        D[1][4 - i] = L[i][1];
        L[i][1] = U[1][i];
        U[1][i] = temp;
    }
    return 0;
}

int func_U(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 2; i++) {
        temp = U[1][i];
        U[1][i] = U[i][3];
        U[i][3] = U[3][4 - i];
        U[3][4 - i] = U[4 - i][1];
        U[4 - i][1] = temp;
    }
    for (i = 1; i <= 3; i++) {
        temp = F[3][i];
        F[3][i] = R[3][i];
        R[3][i] = B[3][4 - i];
        B[3][4 - i] = L[3][4 - i];
        L[3][4 - i] = temp;
    }
    return 0;
}

int func_u(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 2; i++) {
        temp = U[4 - i][1];
        U[4 - i][1] = U[3][4 - i];
        U[3][4 - i] = U[i][3];
        U[i][3] = U[1][i];
        U[1][i] = temp;
    }
    for (i = 1; i <= 3; i++) {
        temp = L[3][4 - i];
        L[3][4 - i] = B[3][4 - i];
        B[3][4 - i] = R[3][i];
        R[3][i] = F[3][i];
        F[3][i] = temp;
    }
    return 0;
}

int func_R(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 2; i++) {
        temp = R[3][i];
        R[3][i] = R[i][1];
        R[i][1] = R[1][4 - i];
        R[1][4 - i] = R[4 - i][3];
        R[4 - i][3] = temp;
    }
    for (i = 1; i <= 3; i++) {
        temp = F[i][3];
        F[i][3] = D[4 - i][3];
        D[4 - i][3] = B[4 - i][3];
        B[4 - i][3] = U[i][3];
        U[i][3] = temp;
    }
    return 0;
}

int func_r(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 2; i++) {
        temp = R[4 - i][3];
        R[4 - i][3] = R[1][4 - i];
        R[1][4 - i] = R[i][1];
        R[i][1] = R[3][i];
        R[3][i] = temp;
    }
    for (i = 1; i <= 3; i++) {
        temp = U[i][3];
        U[i][3] = B[4 - i][3];
        B[4 - i][3] = D[4 - i][3];
        D[4 - i][3] = F[i][3];
        F[i][3] = temp;
    }
    return 0;
}

int func_L(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 2; i++) {
        temp = L[3][i];
        L[3][i] = L[4 - i][3];
        L[4 - i][3] = L[1][4 - i];
        L[1][4 - i] = L[i][1];
        L[i][1] = temp;
    }
    for (i = 1; i <= 3; i++) {
        temp = F[4 - i][1];
        F[4 - i][1] = U[4 - i][1];
        U[4 - i][1] = B[i][1];
        B[i][1] = D[i][1];
        D[i][1] = temp;
    }
    return 0;
}

int func_l(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 2; i++) {
        temp = L[i][1];
        L[i][1] = L[1][4 - i];
        L[1][4 - i] = L[4 - i][3];
        L[4 - i][3] = L[3][i];
        L[3][i] = temp;
    }
    for (i = 1; i <= 3; i++) {
        temp = D[i][1];
        D[i][1] = B[i][1];
        B[i][1] = U[4 - i][1];
        U[4 - i][1] = F[4 - i][1];
        F[4 - i][1] = temp;
    }
    return 0;
}

int func_B(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 2; i++) {
        temp = B[3][i];
        B[3][i] = B[4 - i][3];
        B[4 - i][3] = B[1][4 - i];
        B[1][4 - i] = B[i][1];
        B[i][1] = temp;
    }
    for (i = 1; i <= 3; i++) {
        temp = R[4 - i][3];
        R[4 - i][3] = D[3][4 - i];
        D[3][4 - i] = L[i][3];
        L[i][3] = U[3][i];
        U[3][i] = temp;
    }
    return 0;
}

int func_b(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 2; i++) {
        temp = B[i][1];
        B[i][1] = B[1][4 - i];
        B[1][4 - i] = B[4 - i][3];
        B[4 - i][3] = B[3][i];
        B[3][i] = temp;
    }
    for (i = 1; i <= 3; i++) {
        temp = U[3][i];
        U[3][i] = L[i][3];
        L[i][3] = D[3][4 - i];
        D[3][4 - i] = R[4 - i][3];
        R[4 - i][3] = temp;
    }
    return 0;
}

int func_D(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 2; i++) {
        temp = D[1][i];
        D[1][i] = D[4 - i][1];
        D[4 - i][1] = D[3][4 - i];
        D[3][4 - i] = D[i][3];
        D[i][3] = temp;
    }
    for (i = 1; i <= 3; i++) {
        temp = F[1][i];
        F[1][i] = L[1][4 - i];
        L[1][4 - i] = B[1][4 - i];
        B[1][4 - i] = R[1][i];
        R[1][i] = temp;
    }
    return 0;
}

int func_d(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 2; i++) {
        temp = D[i][3];
        D[i][3] = D[3][4 - i];
        D[3][4 - i] = D[4 - i][1];
        D[4 - i][1] = D[1][i];
        D[1][i] = temp;
    }
    for (i = 1; i <= 3; i++) {
        temp = R[1][i];
        R[1][i] = B[1][4 - i];
        B[1][4 - i] = L[1][4 - i];
        L[1][4 - i] = F[1][i];
        F[1][i] = temp;
    }
    return 0;
}

int func_M(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 3; i++) {
        temp = F[4 - i][2];
        F[4 - i][2] = D[i][2];
        D[i][2] = B[i][2];
        B[i][2] = U[4 - i][2];
        U[4 - i][2] = temp;
    }
    return 0;
}

int func_m(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 3; i++) {
        temp = U[4 - i][2];
        U[4 - i][2] = B[i][2];
        B[i][2] = D[i][2];
        D[i][2] = F[4 - i][2];
        F[4 - i][2] = temp;
    }
    return 0;
}

int func_E(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 3; i++) {
        temp = F[2][i];
        F[2][i] = R[2][i];
        R[2][i] = B[2][4 - i];
        B[2][4 - i] = L[2][4 - i];
        L[2][4 - i] = temp;
    }
    return 0;
}

int func_e(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 3; i++) {
        temp = L[2][4 - i];
        L[2][4 - i] = B[2][4 - i];
        B[2][4 - i] = R[2][i];
        R[2][i] = F[2][i];
        F[2][i] = temp;
    }
    return 0;
}

int func_W(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 3; i++) {
        temp = R[4 - i][2];
        R[4 - i][2] = U[2][i];
        U[2][i] = L[i][2];
        L[i][2] = D[2][4 - i];
        D[2][4 - i] = temp;
    }
    return 0;
}

int func_w(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    int i, temp;
    for (i = 1; i <= 3; i++) {
        temp = D[2][4 - i];
        D[2][4 - i] = L[i][2];
        L[i][2] = U[2][i];
        U[2][i] = R[4 - i][2];
        R[4 - i][2] = temp;
    }
    return 0;
}

int func_S(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    func_l(U, F, R, D, B, L);
    func_M(U, F, R, D, B, L);
    func_R(U, F, R, D, B, L);
    return 0;
}

int func_X(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    func_L(U, F, R, D, B, L);
    func_m(U, F, R, D, B, L);
    func_r(U, F, R, D, B, L);
    return 0;
}

int func_Z(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    func_U(U, F, R, D, B, L);
    func_E(U, F, R, D, B, L);
    func_d(U, F, R, D, B, L);
    return 0;
}

int func_Y(int U[4][4], int F[4][4], int R[4][4], int D[4][4], int B[4][4], int L[4][4]) {
    func_u(U, F, R, D, B, L);
    func_e(U, F, R, D, B, L);
    func_D(U, F, R, D, B, L);
    return 0;
}
