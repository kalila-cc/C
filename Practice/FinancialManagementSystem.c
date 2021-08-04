/* coding: utf-8 */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <memory.h>
#include <conio.h>
#include <string.h>
//  ȫ�ֱ���
double balance = 0;
int item_num = 0;
struct financial_info {
    char id[30];
    char item[50];
    double money;
    int date[3];
    char type[30];
} *info;

//  ��������
//  ���������Ƿ���ָ��ʱ���
int coincident(int *start_t, int *end_t, int *this_t) {
    long s = start_t[0] * 10000 + start_t[1] * 100 + start_t[2];
    long e = end_t[0] * 10000 + end_t[1] * 100 + end_t[2];
    long t = this_t[0] * 10000 + this_t[1] * 100 + this_t[2];
    if (s <= t && t <= e)
        return 1;
    else
        return 0;
}
// ���������������ڽ�������
void sort_by_date() {
    int i, j, k;
    long *date;
    struct financial_info *tmp;
    date = (long *)calloc(item_num, sizeof(long));
    tmp = (struct financial_info *)calloc(1, sizeof(struct financial_info));
    for (i = 0; i < item_num; i++)
        date[i] = info[i].date[0] * 10000 + info[i].date[1] * 100 + info[i].date[2];
    for (i = 0; i < item_num - 1; i++) {
        for (j = i + 1, k = i; j < item_num; j++) {
            if (date[k] > date[j]) {
                k = j;
            }
        }
        if (k != i) {
            *tmp = info[k];
            info[k] = info[i];
            info[i] = *tmp;
        }
    }
    free(date);
    free(tmp);
}
//  ��ʾ�ض���Ϣ
void show() {
    int selection, i, num, now_year, now_mon, *start_time, *end_time;
    char *state;
    time_t t;
    struct tm *tm_t;
    sort_by_date();
    printf("��ѡ����Ϣ��ʾ��ʽ��\n");
    printf("1.��ʾ���n����Ϣ\n");
    printf("2.��ʾ���һ���µ���Ϣ\n");
    printf("3.��ָ��ʱ�����ʾ��Ϣ\n");
    scanf("%d", &selection);
    switch (selection) {
    //  ��ʾ��� n ����Ϣ
    case 1:
        state = (char *)calloc(10, sizeof(char));
        printf("��������Ҫ��ʾ�������Ϣ������������س���Ĭ������Ϊ5����");
        fflush(stdin);
        gets(state);
        if (!strlen(state))
            num = 5;
        else
            num = atoi(state);
        for (i=item_num-num; i<item_num; i++) {
            if (i>=0) {
                printf("---------------------------------------------\n");
                printf("��ţ�%s\n", info[i].id);
                if (info[i].money > 0)
                    printf("������Ŀ���ƣ�%s\n", info[i].item);
                else
                    printf("֧����Ŀ���ƣ�%s\n", info[i].item);
                printf("��%.2lfԪ\n", info[i].money);
                printf("���ڣ�%4d-%02d-%02d\n", info[i].date[0], info[i].date[1], info[i].date[2]);
                if (info[i].money < 0)
                    printf("֧�����%s\n", info[i].type);
                printf("---------------------------------------------\n");
            }
        }
        printf("��%.2lf\n", balance);
        printf("---------------------------------------------\n");
        free(state);
        break;
    //  ��ʾ���һ���µ���Ϣ
    case 2:
        time(&t);
        tm_t = gmtime(&t);
        now_year = tm_t->tm_year + 1900;
        now_mon = tm_t->tm_mon + 1;
        for (i = 0; i < item_num; i++) {
            if (info[i].date[0]==now_year && info[i].date[1]==now_mon) {
                printf("---------------------------------------------\n");
                printf("��ţ�%s\n", info[i].id);
                if (info[i].money > 0)
                    printf("������Ŀ���ƣ�%s\n", info[i].item);
                else
                    printf("֧����Ŀ���ƣ�%s\n", info[i].item);
                printf("��%.2lf\n", info[i].money);
                printf("���ڣ�%4d-%02d-%02d\n", info[i].date[0], info[i].date[1], info[i].date[2]);
                if (info[i].money < 0)
                    printf("֧�����%s\n", info[i].type);
                printf("��%.2lf\n", balance);
                printf("---------------------------------------------\n");
            }
        }
        printf("��%.2lf\n", balance);
        printf("---------------------------------------------\n");
        free(tm_t);
        break;
    //  ��ָ��ʱ�����ʾ��Ϣ
    case 3:
        start_time = (int *)calloc(3, sizeof(int));
        end_time = (int *)calloc(3, sizeof(int));
        printf("���ԡ�yyyy-mm-dd���ĸ�ʽ����������ʾ��Ϣ����ʼʱ�䣺");
        fflush(stdin);
        scanf("%d-%d-%d", start_time, start_time + 1, start_time + 2);
        printf("���ԡ�yyyy-mm-dd���ĸ�ʽ����������ʾ��Ϣ�Ľ�ֹʱ�䣺");
        fflush(stdin);
        scanf("%d-%d-%d", end_time, end_time + 1, end_time + 2);
        for (i = 0; i < item_num; i++) {
            if (coincident(start_time, end_time, info[i].date)) {
                printf("---------------------------------------------\n");
                printf("��ţ�%s\n", info[i].id);
                if (info[i].money > 0)
                    printf("������Ŀ���ƣ�%s\n", info[i].item);
                else
                    printf("֧����Ŀ���ƣ�%s\n", info[i].item);
                printf("��%.2lf\n", info[i].money);
                printf("���ڣ�%4d-%02d-%02d\n", info[i].date[0], info[i].date[1], info[i].date[2]);
                if (info[i].money < 0)
                    printf("֧�����%s\n", info[i].type);
                printf("---------------------------------------------\n");
            }
        }
        printf("��%.2lf\n", balance);
        printf("---------------------------------------------\n");
        free(start_time), free(end_time);
        break;
    default:
        printf("��ѡ���˴������ʾ��Ϣ��ʽ��\n");
        break;
    }
}
// ��ʾ������Ϣ
void show_all() {
    int i;
    sort_by_date();
    for (i = 0; i < item_num; i++) {
        printf("---------------------------------------------\n");
        printf("��ţ�%s\n", info[i].id);
        if (info[i].money > 0)
            printf("������Ŀ���ƣ�%s\n", info[i].item);
        else
            printf("֧����Ŀ���ƣ�%s\n", info[i].item);
        printf("��%.2lfԪ\n", info[i].money);
        printf("���ڣ�%4d-%02d-%02d\n", info[i].date[0], info[i].date[1], info[i].date[2]);
        if (info[i].money < 0)
            printf("֧�����%s\n", info[i].type);
        printf("---------------------------------------------\n");
    }
    printf("��%.2lfԪ\n", balance);
    printf("---------------------------------------------\n");
}
// ��ʼ������
void init() {
    int i;
    char state = 's';
    while (state != 'q') {
        info = (struct financial_info *)realloc(info, (item_num + 1) * sizeof(struct financial_info));
        printf("=============================================\n");
        printf("������֧��/����ı�ţ�");
        fflush(stdin);
        gets(info[item_num].id);
        printf("������֧��/�������Ŀ���ƣ�");
        fflush(stdin);
        gets(info[item_num].item);
        printf("������֧��/����Ľ�");
        fflush(stdin);
        scanf("%lf", &info[item_num].money);
        balance += info[item_num].money;
        printf("���ԡ�yyyy-mm-dd����ʽ����֧��/��������ڣ�");
        fflush(stdin);
        scanf("%d-%d-%d", info[item_num].date, info[item_num].date + 1, info[item_num].date + 2);
        if (info[item_num].money < 0) {
            printf("������֧�����ͣ�");
            fflush(stdin);
            gets(info[item_num].type);
        }
        item_num++;
        printf("Ҫ���������밴�س������������밴��q����\n");
        fflush(stdin);
        state = getch();
        printf("=============================================\n");
    }
    printf("������ɣ�����Ϊ����ʾ������Ϣ��\n");
    sort_by_date();
    show_all();
}
//  ����һ����Ϣ����������Ӳ����ں������ڲ�
void add() {
    int i;
    info = (struct financial_info *)realloc(info, (item_num + 1) * sizeof(struct financial_info));
    printf("=============================================\n");
    printf("������֧��/����ı�ţ�");
    fflush(stdin);
    gets(info[item_num].id);
    printf("������֧��/�������Ŀ���ƣ�");
    fflush(stdin);
    gets(info[item_num].item);
    printf("������֧��/����Ľ�");
    fflush(stdin);
    scanf("%lf", &info[item_num].money);
    balance += info[item_num].money;
    printf("���ԡ�yyyy-mm-dd����ʽ����֧��/��������ڣ�");
    fflush(stdin);
    scanf("%d-%d-%d",info[item_num].date, info[item_num].date + 1, info[item_num].date + 2);
    if (info[item_num].money < 0) {
        printf("������֧�����ͣ�");
        fflush(stdin);
        gets(info[item_num].type);
    }
    item_num++;
    printf("\n�����Ǹ��µ���Ϣ�б�\n");
    sort_by_date();
    show_all();
    printf("=============================================\n");
}
//  ɾ��һ����Ϣ�������ѡ������ں������ڲ�
void delete() {
    int i, j;
    char *tmp_id;
    tmp_id = (char *)calloc(30, sizeof(char));
    printf("=============================================\n");
    printf("��������Ҫɾ������Ϣ��ţ�");
    fflush(stdin);
    gets(tmp_id);
    for (i = 0; i < item_num; i++) {
        if (!strcmp(tmp_id, info[i].id)) {
            balance -= info[i].money;
            for (j = i + 1; j < item_num; j++)
                info[j - 1] = info[j];
            item_num--;
            printf("�ɹ�ɾ�����Ϊ%s����Ϣ��\n", tmp_id);
            printf("=============================================\n");
            free(tmp_id);
            printf("\n�����Ǹ��µ���Ϣ�б�\n");
            show_all();
            return ;
        }
    }
    printf("���Ϊ%s����Ϣɾ��ʧ�ܣ�\n", tmp_id);
    printf("=============================================");
    free(tmp_id);
    printf("\n�����Ǹ��µ���Ϣ�б�\n");
    show_all();
}
// ��ѯ����
void check() {
    int selection, i, j, len1, len2, *date;
    char *item_name, *type_name;
    printf("��ѡ���ѯ��ʽ��\n");
    printf("1.����Ŀ���Ʋ�ѯ\n");
    printf("2.������ѯ\n");
    printf("3.�����ڲ�ѯ\n");
    scanf("%d", &selection);
    switch (selection) {
    case 1:     //  ����Ŀ���Ʋ�ѯ
        item_name = (char *)calloc(50, sizeof(char));
        printf("=============================================\n");
        printf("��������Ҫ��ѯ����Ŀ���ƣ�");
        fflush(stdin);
        gets(item_name);
        len1 = strlen(item_name);
        printf("��������Ҫ��ѯ����Ϣ���£�\n");
        for (i = 0; i < item_num; i++) {
            len2 = strlen(info[i].item);
            for (j=0; j<len2-len1; j++) {
                if (!strncmp(item_name, info[i].item+j, len1)) {
                    printf("---------------------------------------------\n");
                    printf("��ţ�%s\n", info[i].id);
                    if (info[i].money > 0)
                        printf("������Ŀ���ƣ�%s\n", info[i].item);
                    else
                        printf("֧����Ŀ���ƣ�%s\n", info[i].item);
                    printf("��%.2lf\n", info[i].money);
                    printf("���ڣ�%d-%d-%d\n", info[i].date[0], info[i].date[1], info[i].date[2]);
                    if (info[i].money < 0)
                        printf("֧�����%s\n", info[i].type);
                    printf("��%.2lf\n", balance);
                    printf("---------------------------------------------\n");
                }
                break;
            }
        }
        printf("=============================================\n");
        break;
    case 2:     //  ������ѯ
        type_name = (char *)calloc(50, sizeof(char));
        printf("=============================================\n");
        printf("��������Ҫ��ѯ�����");
        fflush(stdin);
        gets(type_name);
        len1 = strlen(type_name);
        printf("��������Ҫ��ѯ����Ϣ���£�\n");
        for (i = 0; i < item_num; i++) {
            len2 = strlen(info[i].item);
            for (j = 0; j < len2 - len1; j++) {
                if (!strncmp(type_name, info[i].type + j, len1)) {
                    printf("---------------------------------------------\n");
                    printf("��ţ�%s\n", info[i].id);
                    if (info[i].money > 0)
                        printf("������Ŀ���ƣ�%s\n", info[i].item);
                    else
                        printf("֧����Ŀ���ƣ�%s\n", info[i].item);
                    printf("��%.2lf\n", info[i].money);
                    printf("���ڣ�%d-%d-%d\n", info[i].date[0], info[i].date[1], info[i].date[2]);
                    if (info[i].money < 0)
                        printf("֧�����%s\n", info[i].type);
                    printf("��%.2lf\n", balance);
                    printf("---------------------------------------------\n");
                }
                break;
            }
        }
        printf("=============================================\n");
        break;
    case 3:     //  �����ڲ�ѯ
        date = (int *)calloc(3, sizeof(int));
        printf("=============================================\n");
        printf("���ԡ�yyyy-mm-dd���ĸ�ʽ������Ҫ��ѯ�����ڣ�");
        fflush(stdin);
        scanf("%d-%d-%d", date, date + 1, date + 2);
        printf("��������Ҫ��ѯ����Ϣ���£�\n");
        for (i = 0; i < item_num; i++) {
            if (date[1] == info[i].date[1] && date[1] == info[i].date[1] && date[2] == info[i].date[2]) {
                printf("---------------------------------------------\n");
                printf("��ţ�%s\n", info[i].id);
                if (info[i].money > 0)
                    printf("������Ŀ���ƣ�%s\n", info[i].item);
                else
                    printf("֧����Ŀ���ƣ�%s\n", info[i].item);
                printf("��%.2lf\n", info[i].money);
                printf("���ڣ�%d-%d-%d\n", info[i].date[0], info[i].date[1], info[i].date[2]);
                if (info[i].money < 0)
                    printf("֧�����%s\n", info[i].type);
                printf("��%.2lf\n", balance);
                printf("---------------------------------------------\n");
            }
        }
        printf("=============================================\n");
        break;
    default:
        printf("�������˴����ѡ��\n");
        break;
    }
    free(item_name), free(type_name);
}
// ͳ�ƺ���
void statistics() {
    int selection, i = 0, j, k, old_year, new_year, *count;
    double sum_in, sum_out, *quarter_in, *quarter_out, *mon_in, *mon_out;
    sort_by_date();
    printf("��ѡ������ͳ�Ƹ�ʽ��\n");
    printf("1.���ꡢ���ȡ���ͳ�������ܶ֧���ܶ\n");
    printf("2.ͳ��ÿ���µ�ƽ��֧��\n");
    scanf("%d", &selection);
    switch (selection) {
    case 1:     //  ��1�����ꡢ���ȡ���ͳ�������ܶ֧���ܶ�
        printf("=============================================\n");
        old_year = info[0].date[0];
        while (i < item_num) {
            sum_in = sum_out = 0;
            quarter_in = (double *)calloc(4, sizeof(double));
            quarter_out = (double *)calloc(4, sizeof(double));
            mon_in = (double *)calloc(12, sizeof(double));
            mon_out = (double *)calloc(12, sizeof(double));
            while (i < item_num && (new_year = info[i].date[0]) == old_year) {
                if (info[i].money > 0) {
                    mon_in[info[i].date[1] - 1] += info[i].money;
                    quarter_in[(info[i].date[1] - 1) / 3] += info[i].money;
                    sum_in += info[i].money;
                } else {
                    mon_out[info[i].date[1] - 1] += info[i].money;
                    quarter_out[(info[i].date[1] - 1) / 3] += info[i].money;
                    sum_out += info[i].money;
                }
                i++;
            }
            printf("%d�꣺\n", old_year);
            printf("�����ܶ%.2lfԪ\n", sum_in);
            printf("֧���ܶ%.2lfԪ\n", sum_out);
            for (j = 0; j < 4; j++) {
                printf("��%d���ȣ�\n", j + 1);
                for (k = 0; k < 3; k++) {
                    printf("%2d�����룺%.2lfԪ\n", j * 3 + k + 1, mon_in[j * 3 + k]);
                    printf("%2d��֧����%.2lfԪ\n", j * 3 +  k + 1, mon_out[j * 3 + k]);
                }
            }
            old_year = new_year;
        }
        printf("=============================================\n");
        free(quarter_in), free(quarter_out);
        free(mon_in), free(mon_out);
        break;
    case 2:     //  ��3��ͳ��ÿ���µ�ƽ��֧��
        mon_out = (double *)calloc(12, sizeof(double));
        count = (int *)calloc(12, sizeof(int));
        for (i = 0; i < item_num; i++) {
            if (info[i].money < 0) {
                mon_out[info[i].date[1] - 1] += info[i].money;
                count[info[i].date[1] - 1]++;
            }
        }
        for (i = 0; i < 12; i++) 
            if (count[i])
                mon_out[i] /= count[i];
        old_year = info[0].date[0], new_year = info[item_num - 1].date[0];
        printf("=============================================\n");
        if (old_year == new_year)
            printf("��%d�꣬���ĸ���ƽ��֧�����£�\n", old_year);
        else
            printf("��%d�굽%d�꣬���ĸ���ƽ��֧�����£�\n", old_year, new_year);
        for (i = 0; i < 12; i++)
            printf("%2d��ƽ��֧����%.2lf\n", i + 1, mon_out[i]);
        printf("=============================================\n");
        free(mon_out), free(count);
        break;
    default:
        printf("����ѡ�����");
        break;
    }
}
//  ������
int main() {
    int selection;
    printf("��ӭ����������ϵͳ���������뿪ʼ������Ϣ��\n");
    init();
    printf("��ѡ���ܣ�\n");
    while (1) {
        printf("1.��ʾ��Ϣ\t2.������Ϣ\t3.ɾ����Ϣ\t4.��ѯ��Ϣ\t5.����ͳ��\t6.�˳�ϵͳ\n");
        scanf("%d", &selection);
        switch (selection) {
        case 1:
            show();
            break;
        case 2:
            add();
            break;
        case 3:
            delete();
            break;
        case 4:
            check();
            break;
        case 5:
            statistics();
            break;
        case 6:
            printf("�����Ҫ�˳�ϵͳ�𣿣�����1��ȷ��,����0��ȡ����\n");
            scanf("%d", &selection);
            if (selection)
                exit(0);
            else
                printf("��ȡ���˳�ϵͳ��\n");
            break;
        default:
            break;
        }
    }
}
