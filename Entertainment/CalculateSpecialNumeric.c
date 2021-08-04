#include <stdio.h>
#include <math.h>
#include <time.h>

#define N 50 

int len(long long a) {
	int i;
	if (a < 1) {
		printf("\n\t请输入正整数！");
		return 0;
	}
	for (i = 12; i >= 0; i--) {
        if (a == pow(10, i)) return i + 1;
        else if (a % (long long)(pow(10, i)) != a) return i + 1;
        else continue;
	}
    return 0;
}

int main() {
	int choose, time, alen, blen, sum;
	long long a, b, i, j, k, result, left, right, middle, temp, temp2;
	float total_time;
	long long tank[N];
	for (i = 0; i < N; i++) tank[i] = 0;
	clock_t start, finish;
	printf("\n\n\t提示：这是一个特殊自然数求解程序\n\t");
	printf("\n\t(请按提示进行操作，否则将自动退出相应模块！)\n\n\n\t");
	do {
		printf("\n\t请选择以下功能： ");
		printf("\n\t1.给定范围求素数\t2.给定范围求完全数\n\t3.给定范围求自幂数\n\t");
		scanf("%d", &choose);
		if (choose == 1) {
			printf("\n\n\t已选模块；1.给定范围求素数\n\n");
			time = 0;
			printf("\n\t请输入给定范围的左端点：");
			scanf("%lld", &a);
			printf("\n\t请输入给定范围的右端点：");
			scanf("%lld", &b);
			if (a < 1 || b < 1) break;
			start = clock();
			for (i = a; i <= b; i++) {
				if (i <= 2) {
					if (i == 2) {
						time++;
						printf("\n\n\t[%lld,%lld]内第 %d 个素数：2", a, b, time);
					}
					continue;
				}
				middle = (int)(sqrt(i)) + 1;
				for (j = 2; j <= middle; j++) {
					if (i % j == 0) break;
					if (j == middle) {
						time++;
						printf("\n\n\t[%lld,%lld]内第 %d 个素数：%lld", a, b, time, i);
					}
				}	
			}
			finish = clock();
			total_time = (float)((finish - start) / 1000.0);
			printf("\n\n\t此次用时 %f s", total_time);
			printf("\n\n\n\t[%lld,%lld]范围内的素数已全部找出！\n\n\t-------------------------------------------------\n\n\n\n", a, b);	
		} else if (choose == 2) {
			printf("\n\n\t已选模块；2.给定范围求完全数\n\n");
			time = 0;
			printf("\n\t请输入给定范围的左端点：");
			scanf("%lld", &a);
			printf("\n\t请输入给定范围的右端点：");
			scanf("%lld", &b);
			start = clock();
			if (a < 1 || b < 1 || a >= b) break;
			for (i = a; i <= b; i++) {
				result = 0;
				middle = (int)(sqrt(i));
				for (j = 1; j <= middle; j++) {
					if (result > i) break;
					if (i % j == 0) {
						if (j != 1) result += j + i / j;
						else result ++;
					}
				}
				if (pow(middle, 2) == i)
					result -= middle;
				if (result == i && i > 1) {
					time++;
					printf("\n\t[%lld,%lld]范围内的第 %d 个完全数：%lld\n\t", a, b, time, i);
				}
				if (i % 2000000 == 0)
					printf("\n\t(请勿退出程序，由于计算量较大，请耐心等待，此时已运行 %lld 次)", i);
			}
			finish = clock();
			total_time = (float)((finish - start) / 1000.0);
			printf("\n\n\t此次用时 %f s", total_time);
			printf("\n\n\n\t[%d,%llu]范围内的完全数已全部找出！\n\n\t-------------------------------------------------\n\n\n\n", a, b);
		} else if (choose == 3) {
			printf("\n\n\t已选模块；3.给定范围求自幂数\n\n");
			printf("\n\t请输入给定范围的左端点：");
			scanf("%lld", &a);
			printf("\n\t请输入给定范围的右端点：");
			scanf("%lld", &b);
			if (a < 1 || b < 1) break;
			start = clock();
			alen = len(a);
			blen = len(b);
			left = a;
			right = b;
			for (i = alen; i <= blen; i++) {
				for (j = 0; j < sum; j++) tank[i] = 0;
				sum = 0;
				if (i < blen) right = pow(10, i);
				else right = b + 1;
				for (j = left; j < right; j++) {
					result = 0;
					temp = j;
					temp2 = j;
					for (k = i; k > 0; k--) {
						temp = (temp2 - temp2 % (long long)(pow(10, k-1))) / pow(10, k-1);
						result += pow(temp, i);
						temp *= pow(10, k-1);
						temp2 = temp2 - temp;
					}
					if (result == j) tank[sum++] = result;
				}
				left = pow(10, i);
				if (sum != 0) {
					switch(i) {
						case 1:
							printf("\n\n\t[%lld,%lld]内共有以下 %d 个独身数（独身数即 n = 1 时的自幂数）\n\n\t", a, b, sum);
							break;
						case 3:
							printf("\n\n\t[%lld,%lld]内共有以下 %d 个水仙花数（水仙花数即 n = 3 时的自幂数）\n\n\t", a, b, sum);
							break;
						case 4:
							printf("\n\n\t[%lld,%lld]内共有以下 %d 个四叶玫瑰数（四叶玫瑰数即 n = 4 时的自幂数）\n\n\t", a, b, sum);
							break;
						case 5:
							printf("\n\n\t[%lld,%lld]内共有以下 %d 个五角星数（五角星数即 n = 5 时的自幂数）\n\n\t", a, b, sum);
							break;
						case 6:
							printf("\n\n\t[%lld,%lld]内共有以下 %d 个六合数（六合数即 n = 6 时的自幂数）\n\n\t", a, b, sum);
							break;
						case 7:
							printf("\n\n\t[%lld,%lld]内共有以下 %d 个北斗七星数（北斗七星数即 n = 7 时的自幂数）\n\n\t", a, b, sum);
							break;
						case 8:
							printf("\n\n\t[%lld,%lld]内共有以下 %d 个八仙数（八仙数即 n = 8 时的自幂数）\n\n\t", a, b, sum);
							break;
						case 9:
							printf("\n\n\t[%lld,%lld]内共有以下%d个九九重阳数（九九重阳数即 n = 9 时的自幂数）\n\n\t", a, b, sum);
							break;
						case 10:
							printf("\n\n\t[%lld,%lld]内共有以下%d个十全十美数（十全十美数即 n = 10 时的自幂数）\n\n\t", a, b, sum);
							break;
						default:
							break;
					}
					for (j = 0; j < sum; j++) printf("   %lld", tank[j]);
				}
			}
			finish = clock();
			total_time = (float)((finish - start) / 1000.0);
			printf("\n\n\t此次用时 %f s", total_time);
			printf("\n\n\n\t[%lld,%lld]范围内的自幂数已全部找出！\n\n\t-------------------------------------------------\n\n\n\n", a, b);
		}
		else
			break;
	} while(1);
}
