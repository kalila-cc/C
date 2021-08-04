/* coding: utf-8 */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <memory.h>
#include <conio.h>
#include <string.h>
//  全局变量
double balance = 0;
int item_num = 0;
struct financial_info {
    char id[30];
    char item[50];
    double money;
    int date[3];
    char type[30];
} *info;

//  辅助函数
//  检验日期是否在指定时间段
int coincident(int *start_t, int *end_t, int *this_t) {
    long s = start_t[0] * 10000 + start_t[1] * 100 + start_t[2];
    long e = end_t[0] * 10000 + end_t[1] * 100 + end_t[2];
    long t = this_t[0] * 10000 + this_t[1] * 100 + this_t[2];
    if (s <= t && t <= e)
        return 1;
    else
        return 0;
}
// 排序函数，根据日期进行排序
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
//  显示特定信息
void show() {
    int selection, i, num, now_year, now_mon, *start_time, *end_time;
    char *state;
    time_t t;
    struct tm *tm_t;
    sort_by_date();
    printf("请选择信息显示方式：\n");
    printf("1.显示最近n笔信息\n");
    printf("2.显示最近一个月的信息\n");
    printf("3.按指定时间段显示信息\n");
    scanf("%d", &selection);
    switch (selection) {
    //  显示最近 n 笔信息
    case 1:
        state = (char *)calloc(10, sizeof(char));
        printf("请输入需要显示的最近信息的数量（输入回车则默认数量为5）：");
        fflush(stdin);
        gets(state);
        if (!strlen(state))
            num = 5;
        else
            num = atoi(state);
        for (i=item_num-num; i<item_num; i++) {
            if (i>=0) {
                printf("---------------------------------------------\n");
                printf("编号：%s\n", info[i].id);
                if (info[i].money > 0)
                    printf("收入项目名称：%s\n", info[i].item);
                else
                    printf("支出项目名称：%s\n", info[i].item);
                printf("金额：%.2lf元\n", info[i].money);
                printf("日期：%4d-%02d-%02d\n", info[i].date[0], info[i].date[1], info[i].date[2]);
                if (info[i].money < 0)
                    printf("支出类别：%s\n", info[i].type);
                printf("---------------------------------------------\n");
            }
        }
        printf("余额：%.2lf\n", balance);
        printf("---------------------------------------------\n");
        free(state);
        break;
    //  显示最近一个月的信息
    case 2:
        time(&t);
        tm_t = gmtime(&t);
        now_year = tm_t->tm_year + 1900;
        now_mon = tm_t->tm_mon + 1;
        for (i = 0; i < item_num; i++) {
            if (info[i].date[0]==now_year && info[i].date[1]==now_mon) {
                printf("---------------------------------------------\n");
                printf("编号：%s\n", info[i].id);
                if (info[i].money > 0)
                    printf("收入项目名称：%s\n", info[i].item);
                else
                    printf("支出项目名称：%s\n", info[i].item);
                printf("金额：%.2lf\n", info[i].money);
                printf("日期：%4d-%02d-%02d\n", info[i].date[0], info[i].date[1], info[i].date[2]);
                if (info[i].money < 0)
                    printf("支出类别：%s\n", info[i].type);
                printf("余额：%.2lf\n", balance);
                printf("---------------------------------------------\n");
            }
        }
        printf("余额：%.2lf\n", balance);
        printf("---------------------------------------------\n");
        free(tm_t);
        break;
    //  按指定时间段显示信息
    case 3:
        start_time = (int *)calloc(3, sizeof(int));
        end_time = (int *)calloc(3, sizeof(int));
        printf("请以“yyyy-mm-dd”的格式输入所需显示信息的起始时间：");
        fflush(stdin);
        scanf("%d-%d-%d", start_time, start_time + 1, start_time + 2);
        printf("请以“yyyy-mm-dd”的格式输入所需显示信息的截止时间：");
        fflush(stdin);
        scanf("%d-%d-%d", end_time, end_time + 1, end_time + 2);
        for (i = 0; i < item_num; i++) {
            if (coincident(start_time, end_time, info[i].date)) {
                printf("---------------------------------------------\n");
                printf("编号：%s\n", info[i].id);
                if (info[i].money > 0)
                    printf("收入项目名称：%s\n", info[i].item);
                else
                    printf("支出项目名称：%s\n", info[i].item);
                printf("金额：%.2lf\n", info[i].money);
                printf("日期：%4d-%02d-%02d\n", info[i].date[0], info[i].date[1], info[i].date[2]);
                if (info[i].money < 0)
                    printf("支出类别：%s\n", info[i].type);
                printf("---------------------------------------------\n");
            }
        }
        printf("余额：%.2lf\n", balance);
        printf("---------------------------------------------\n");
        free(start_time), free(end_time);
        break;
    default:
        printf("您选择了错误的显示信息方式！\n");
        break;
    }
}
// 显示所有信息
void show_all() {
    int i;
    sort_by_date();
    for (i = 0; i < item_num; i++) {
        printf("---------------------------------------------\n");
        printf("编号：%s\n", info[i].id);
        if (info[i].money > 0)
            printf("收入项目名称：%s\n", info[i].item);
        else
            printf("支出项目名称：%s\n", info[i].item);
        printf("金额：%.2lf元\n", info[i].money);
        printf("日期：%4d-%02d-%02d\n", info[i].date[0], info[i].date[1], info[i].date[2]);
        if (info[i].money < 0)
            printf("支出类别：%s\n", info[i].type);
        printf("---------------------------------------------\n");
    }
    printf("余额：%.2lf元\n", balance);
    printf("---------------------------------------------\n");
}
// 初始化函数
void init() {
    int i;
    char state = 's';
    while (state != 'q') {
        info = (struct financial_info *)realloc(info, (item_num + 1) * sizeof(struct financial_info));
        printf("=============================================\n");
        printf("请输入支出/收入的编号：");
        fflush(stdin);
        gets(info[item_num].id);
        printf("请输入支出/收入的项目名称：");
        fflush(stdin);
        gets(info[item_num].item);
        printf("请输入支出/收入的金额：");
        fflush(stdin);
        scanf("%lf", &info[item_num].money);
        balance += info[item_num].money;
        printf("请以“yyyy-mm-dd”格式输入支出/收入的日期：");
        fflush(stdin);
        scanf("%d-%d-%d", info[item_num].date, info[item_num].date + 1, info[item_num].date + 2);
        if (info[item_num].money < 0) {
            printf("请输入支出类型：");
            fflush(stdin);
            gets(info[item_num].type);
        }
        item_num++;
        printf("要继续输入请按回车，结束输入请按“q”：\n");
        fflush(stdin);
        state = getch();
        printf("=============================================\n");
    }
    printf("输入完成，以下为您显示所有信息：\n");
    sort_by_date();
    show_all();
}
//  增加一条信息，具体的增加操作在函数体内部
void add() {
    int i;
    info = (struct financial_info *)realloc(info, (item_num + 1) * sizeof(struct financial_info));
    printf("=============================================\n");
    printf("请输入支出/收入的编号：");
    fflush(stdin);
    gets(info[item_num].id);
    printf("请输入支出/收入的项目名称：");
    fflush(stdin);
    gets(info[item_num].item);
    printf("请输入支出/收入的金额：");
    fflush(stdin);
    scanf("%lf", &info[item_num].money);
    balance += info[item_num].money;
    printf("请以“yyyy-mm-dd”格式输入支出/收入的日期：");
    fflush(stdin);
    scanf("%d-%d-%d",info[item_num].date, info[item_num].date + 1, info[item_num].date + 2);
    if (info[item_num].money < 0) {
        printf("请输入支出类型：");
        fflush(stdin);
        gets(info[item_num].type);
    }
    item_num++;
    printf("\n以下是更新的信息列表：\n");
    sort_by_date();
    show_all();
    printf("=============================================\n");
}
//  删除一条信息，具体的选择操作在函数体内部
void delete() {
    int i, j;
    char *tmp_id;
    tmp_id = (char *)calloc(30, sizeof(char));
    printf("=============================================\n");
    printf("请输入需要删除的信息编号：");
    fflush(stdin);
    gets(tmp_id);
    for (i = 0; i < item_num; i++) {
        if (!strcmp(tmp_id, info[i].id)) {
            balance -= info[i].money;
            for (j = i + 1; j < item_num; j++)
                info[j - 1] = info[j];
            item_num--;
            printf("成功删除编号为%s的信息！\n", tmp_id);
            printf("=============================================\n");
            free(tmp_id);
            printf("\n以下是更新的信息列表：\n");
            show_all();
            return ;
        }
    }
    printf("编号为%s的信息删除失败！\n", tmp_id);
    printf("=============================================");
    free(tmp_id);
    printf("\n以下是更新的信息列表：\n");
    show_all();
}
// 查询函数
void check() {
    int selection, i, j, len1, len2, *date;
    char *item_name, *type_name;
    printf("请选择查询方式：\n");
    printf("1.按项目名称查询\n");
    printf("2.按类别查询\n");
    printf("3.按日期查询\n");
    scanf("%d", &selection);
    switch (selection) {
    case 1:     //  按项目名称查询
        item_name = (char *)calloc(50, sizeof(char));
        printf("=============================================\n");
        printf("请输入需要查询的项目名称：");
        fflush(stdin);
        gets(item_name);
        len1 = strlen(item_name);
        printf("您可能想要查询的信息如下：\n");
        for (i = 0; i < item_num; i++) {
            len2 = strlen(info[i].item);
            for (j=0; j<len2-len1; j++) {
                if (!strncmp(item_name, info[i].item+j, len1)) {
                    printf("---------------------------------------------\n");
                    printf("编号：%s\n", info[i].id);
                    if (info[i].money > 0)
                        printf("收入项目名称：%s\n", info[i].item);
                    else
                        printf("支出项目名称：%s\n", info[i].item);
                    printf("金额：%.2lf\n", info[i].money);
                    printf("日期：%d-%d-%d\n", info[i].date[0], info[i].date[1], info[i].date[2]);
                    if (info[i].money < 0)
                        printf("支出类别：%s\n", info[i].type);
                    printf("余额：%.2lf\n", balance);
                    printf("---------------------------------------------\n");
                }
                break;
            }
        }
        printf("=============================================\n");
        break;
    case 2:     //  按类别查询
        type_name = (char *)calloc(50, sizeof(char));
        printf("=============================================\n");
        printf("请输入需要查询的类别：");
        fflush(stdin);
        gets(type_name);
        len1 = strlen(type_name);
        printf("您可能想要查询的信息如下：\n");
        for (i = 0; i < item_num; i++) {
            len2 = strlen(info[i].item);
            for (j = 0; j < len2 - len1; j++) {
                if (!strncmp(type_name, info[i].type + j, len1)) {
                    printf("---------------------------------------------\n");
                    printf("编号：%s\n", info[i].id);
                    if (info[i].money > 0)
                        printf("收入项目名称：%s\n", info[i].item);
                    else
                        printf("支出项目名称：%s\n", info[i].item);
                    printf("金额：%.2lf\n", info[i].money);
                    printf("日期：%d-%d-%d\n", info[i].date[0], info[i].date[1], info[i].date[2]);
                    if (info[i].money < 0)
                        printf("支出类别：%s\n", info[i].type);
                    printf("余额：%.2lf\n", balance);
                    printf("---------------------------------------------\n");
                }
                break;
            }
        }
        printf("=============================================\n");
        break;
    case 3:     //  按日期查询
        date = (int *)calloc(3, sizeof(int));
        printf("=============================================\n");
        printf("请以“yyyy-mm-dd”的格式输入需要查询的日期：");
        fflush(stdin);
        scanf("%d-%d-%d", date, date + 1, date + 2);
        printf("您可能想要查询的信息如下：\n");
        for (i = 0; i < item_num; i++) {
            if (date[1] == info[i].date[1] && date[1] == info[i].date[1] && date[2] == info[i].date[2]) {
                printf("---------------------------------------------\n");
                printf("编号：%s\n", info[i].id);
                if (info[i].money > 0)
                    printf("收入项目名称：%s\n", info[i].item);
                else
                    printf("支出项目名称：%s\n", info[i].item);
                printf("金额：%.2lf\n", info[i].money);
                printf("日期：%d-%d-%d\n", info[i].date[0], info[i].date[1], info[i].date[2]);
                if (info[i].money < 0)
                    printf("支出类别：%s\n", info[i].type);
                printf("余额：%.2lf\n", balance);
                printf("---------------------------------------------\n");
            }
        }
        printf("=============================================\n");
        break;
    default:
        printf("您输入了错误的选择！\n");
        break;
    }
    free(item_name), free(type_name);
}
// 统计函数
void statistics() {
    int selection, i = 0, j, k, old_year, new_year, *count;
    double sum_in, sum_out, *quarter_in, *quarter_out, *mon_in, *mon_out;
    sort_by_date();
    printf("请选择数据统计格式：\n");
    printf("1.按年、季度、月统计收入总额、支出总额；\n");
    printf("2.统计每个月的平均支出\n");
    scanf("%d", &selection);
    switch (selection) {
    case 1:     //  （1）按年、季度、月统计收入总额、支出总额
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
            printf("%d年：\n", old_year);
            printf("收入总额：%.2lf元\n", sum_in);
            printf("支出总额：%.2lf元\n", sum_out);
            for (j = 0; j < 4; j++) {
                printf("第%d季度：\n", j + 1);
                for (k = 0; k < 3; k++) {
                    printf("%2d月收入：%.2lf元\n", j * 3 + k + 1, mon_in[j * 3 + k]);
                    printf("%2d月支出：%.2lf元\n", j * 3 +  k + 1, mon_out[j * 3 + k]);
                }
            }
            old_year = new_year;
        }
        printf("=============================================\n");
        free(quarter_in), free(quarter_out);
        free(mon_in), free(mon_out);
        break;
    case 2:     //  （3）统计每个月的平均支出
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
            printf("在%d年，您的各月平均支出如下：\n", old_year);
        else
            printf("在%d年到%d年，您的各月平均支出如下：\n", old_year, new_year);
        for (i = 0; i < 12; i++)
            printf("%2d月平均支出：%.2lf\n", i + 1, mon_out[i]);
        printf("=============================================\n");
        free(mon_out), free(count);
        break;
    default:
        printf("您的选择出错！");
        break;
    }
}
//  主函数
int main() {
    int selection;
    printf("欢迎进入财务管理系统！接下来请开始输入信息：\n");
    init();
    printf("请选择功能：\n");
    while (1) {
        printf("1.显示信息\t2.增加信息\t3.删除信息\t4.查询信息\t5.数据统计\t6.退出系统\n");
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
            printf("您真的要退出系统吗？（输入1以确认,输入0以取消）\n");
            scanf("%d", &selection);
            if (selection)
                exit(0);
            else
                printf("已取消退出系统！\n");
            break;
        default:
            break;
        }
    }
}
