#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define M 3
#define true 1
#define false 0

typedef char bool;
typedef struct Student_tag {
    char name[24];
    char stuid[24];
    float score[M];
    float sumScore;
    float meanScore;
} Student;

const char filename[] = "./data.txt";
char course[M][12] = {"����", "��ѧ", "Ӣ��"};
int N = 0;
Student *stuList = NULL;

float _sum(float *score) {
    int i;
    float s = 0.0;
    for (i = 0; i < M; ++i)
        s += score[i];
    return s;
}

float _mean(float *score) {
    int i;
    float s = 0.0;
    for (i = 0; i < M; ++i)
        s += score[i];
    s /= M;
    return s;
}

void _init() {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        file = (FILE *)malloc(0);
        N = 0;
        printf("��ʼ����ɣ���ǰѧ������Ϊ��%d\n", N);
    } else {
        fseek(file, 0L, SEEK_END);
        N = ftell(file) / sizeof(Student);
        fseek(file, 0L, SEEK_SET);
        stuList = (Student *)malloc(sizeof(Student) * N);
        fread((char *)stuList, sizeof(Student), N, file);
        fclose(file);
        printf("��ʼ����ɣ���ǰѧ������Ϊ��%d\n", N);
    }
}

void _save() {
    FILE *file = fopen(filename, "rb");
    if (file)
        remove(filename);
    file = fopen(filename, "wb");
    fwrite((char *)stuList, sizeof(Student), N, file);
    fclose(file);
    printf("����ɹ���\n");
}

void _sort(char type, bool reverse) {
    int i, j, k;
    Student stu;
    for (int i = 0; i < N; ++i) {
        for (j = i + 1, k = i; j < N; ++j) {
            if (type == 'N') {
                if (strcmp(stuList[j].name, stuList[k].name) * (2 * reverse - 1) > 0) {
                    k = j;
                }
            } else if (type == 'I') {
                if (strcmp(stuList[j].stuid, stuList[k].stuid) * (2 * reverse - 1) > 0) {
                    k = j;
                }
            } else if (type == 'S') {
                if ((stuList[j].sumScore - stuList[k].sumScore) * (2 * reverse - 1) > 0) {
                    k = j;
                }
            }
        }
        if (k != i) {
            stu = stuList[k];
            stuList[k] = stuList[i];
            stuList[i] = stu;
        }
    }
}

void _warning(const char *warning_words) {
    printf("%s\n", warning_words);
}

void _done() {
    if (stuList && N)
        free(stuList);
    printf("\n��лʹ�ø�Ӧ�ó��򣬻�ӭ�´�������\n");
    exit(0);
}

void _add() {
    int i, j, n;
    Student stu;
    printf("��������Ҫ��ӵ�ѧ������(����0�˳�)��\n");
    scanf("%d", &n);
    while (n <= 0) {
        if (!n)
            return ;
        printf("���������0��ѧ������(����0�˳�)\n");
        scanf("%d", &n);
    }
    for (i = 0; i < n; ++i) {
        printf("��ֱ�����ѧ�������� ѧ�� ");
        for (j = 0; j < M; ++j)
            printf("%s�ɼ� ", course[j]);
        printf("��\n");
        scanf("%24s%24s", stu.name, stu.stuid);
        for (j = 0; j < M; ++j)
            scanf("%f", stu.score + j);
        stu.sumScore = _sum(stu.score);
        stu.meanScore = _mean(stu.score);
        ++N;
        stuList = realloc(stuList, sizeof(Student) * N);
        stuList[N - 1] = stu;
        printf("�Ѿ��ɹ���Ӹ�ѧ����\n");
    }
}

void _check() {
    bool found = false;
    char stuid[24];
    int i, j;
    printf("������ѧ����ѧ�ţ�\n");
    scanf("%24s", stuid);
    for (i = 0; i < N; ++i) {
        if (!strcmp(stuid, stuList[i].stuid)) {
            found = true;
            break;
        }
    }
    if (!found) {
        printf("��ѧ�������ڣ�\n");
    } else {
        printf("%24s%24s", "����", "ѧ��");
        for (j = 0; j < M; ++j)
            printf("%12s", course[j]);
        printf("%12s", "�ܷ�");
        printf("%12s\n", "����");
        printf("%24s%24s", stuList[i].name, stuList[i].stuid);
        for (j = 0; j < M; ++j)
            printf("%12.2f", stuList[i].score[j]);
        printf("%12.2f", stuList[i].sumScore);
        printf("%12.2f\n", stuList[i].meanScore);
    }
}

void _remove() {
    bool found = false;
    char stuid[24];
    int i, j;
    printf("������ѧ����ѧ�ţ�\n");
    scanf("%24s", stuid);
    for (i = 0; i < N; ++i) {
        if (!strcmp(stuid, stuList[i].stuid)) {
            found = true;
            break;
        }
    }
    if (!found) {
        printf("��ѧ�������ڣ�\n");
    } else {
        for (j = i + 1; j < N; ++j)
            stuList[j - 1] = stuList[j];
        --N;
        printf("�Ѿ��ɹ��Ƴ���ѧ����\n");
    }
}

void _display() {
    bool reverse;
    char op, type;
    int i, j;
    printf("��ѡ������ʽ��\n");
    printf("1. ������   2. ��ѧ��   3. ���ܷ�\n");
    fflush(stdin);
    op = getchar();
    while (op != '1' && op != '2' && op != '3') {
        printf("��������ȷ��ѡ�\n");
        fflush(stdin);
        op = getchar();
    }
    type = op == '1' ? 'N' : (N == '2' ? 'I' : 'S');
    printf("��ѡ������˳��\n");
    printf("1. ����   2. ����\n");
    fflush(stdin);
    op = getchar();
    while (op != '1' && op != '2') {
        printf("��������ȷ��ѡ�\n");
        fflush(stdin);
        op = getchar();
    }
    reverse = op != '1';
    _sort(type, reverse);
    printf("%24s%24s", "����", "ѧ��");
    for (j = 0; j < M; ++j)
        printf("%12s", course[j]);
    printf("%12s", "�ܷ�");
    printf("%12s\n", "����");
    for (i = 0; i < N; ++i) {
        printf("%24s%24s", stuList[i].name, stuList[i].stuid);
        for (j = 0; j < M; ++j)
            printf("%12.2f", stuList[i].score[j]);
        printf("%12.2f", stuList[i].sumScore);
        printf("%12.2f\n", stuList[i].meanScore);
    }
}

int main() {
    char op;
    _init();
    while (true) {
        printf("\n��ѡ�������\n");
        printf("a/A ���, c/C ��ѯ, d/D ��ʾ\n");
        printf("r/R �Ƴ�, s/S ����, e/E �˳�\n");
        fflush(stdin);
        op = getchar();
        switch (op) {
        case 'e':
        case 'E':
            _done();
            break;
        case 'a':
        case 'A':
            _add();
            break;
        case 's':
        case 'S':
            _save();
            break;
        case 'r':
        case 'R':
            _remove();
            break;
        case 'c':
        case 'C':
            _check();
            break;
        case 'd':
        case 'D':
            _display();
            break;
        default:
            _warning("��������ȷ��ѡ�");
            break;
        }
    }
    return 0;
}

/*
��������
xiaoming 2020112233 90 80 70
xiaohong 2020123321 80 90 85
John 2020666666 30 40 98
*/