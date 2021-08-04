#include <stdio.h>
#include <math.h>
#include <time.h>

#define N 50 

int len(long long a) {
	int i;
	if (a < 1) {
		printf("\n\t��������������");
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
	printf("\n\n\t��ʾ������һ��������Ȼ��������\n\t");
	printf("\n\t(�밴��ʾ���в����������Զ��˳���Ӧģ�飡)\n\n\n\t");
	do {
		printf("\n\t��ѡ�����¹��ܣ� ");
		printf("\n\t1.������Χ������\t2.������Χ����ȫ��\n\t3.������Χ��������\n\t");
		scanf("%d", &choose);
		if (choose == 1) {
			printf("\n\n\t��ѡģ�飻1.������Χ������\n\n");
			time = 0;
			printf("\n\t�����������Χ����˵㣺");
			scanf("%lld", &a);
			printf("\n\t�����������Χ���Ҷ˵㣺");
			scanf("%lld", &b);
			if (a < 1 || b < 1) break;
			start = clock();
			for (i = a; i <= b; i++) {
				if (i <= 2) {
					if (i == 2) {
						time++;
						printf("\n\n\t[%lld,%lld]�ڵ� %d ��������2", a, b, time);
					}
					continue;
				}
				middle = (int)(sqrt(i)) + 1;
				for (j = 2; j <= middle; j++) {
					if (i % j == 0) break;
					if (j == middle) {
						time++;
						printf("\n\n\t[%lld,%lld]�ڵ� %d ��������%lld", a, b, time, i);
					}
				}	
			}
			finish = clock();
			total_time = (float)((finish - start) / 1000.0);
			printf("\n\n\t�˴���ʱ %f s", total_time);
			printf("\n\n\n\t[%lld,%lld]��Χ�ڵ�������ȫ���ҳ���\n\n\t-------------------------------------------------\n\n\n\n", a, b);	
		} else if (choose == 2) {
			printf("\n\n\t��ѡģ�飻2.������Χ����ȫ��\n\n");
			time = 0;
			printf("\n\t�����������Χ����˵㣺");
			scanf("%lld", &a);
			printf("\n\t�����������Χ���Ҷ˵㣺");
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
					printf("\n\t[%lld,%lld]��Χ�ڵĵ� %d ����ȫ����%lld\n\t", a, b, time, i);
				}
				if (i % 2000000 == 0)
					printf("\n\t(�����˳��������ڼ������ϴ������ĵȴ�����ʱ������ %lld ��)", i);
			}
			finish = clock();
			total_time = (float)((finish - start) / 1000.0);
			printf("\n\n\t�˴���ʱ %f s", total_time);
			printf("\n\n\n\t[%d,%llu]��Χ�ڵ���ȫ����ȫ���ҳ���\n\n\t-------------------------------------------------\n\n\n\n", a, b);
		} else if (choose == 3) {
			printf("\n\n\t��ѡģ�飻3.������Χ��������\n\n");
			printf("\n\t�����������Χ����˵㣺");
			scanf("%lld", &a);
			printf("\n\t�����������Χ���Ҷ˵㣺");
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
							printf("\n\n\t[%lld,%lld]�ڹ������� %d ������������������ n = 1 ʱ����������\n\n\t", a, b, sum);
							break;
						case 3:
							printf("\n\n\t[%lld,%lld]�ڹ������� %d ��ˮ�ɻ�����ˮ�ɻ����� n = 3 ʱ����������\n\n\t", a, b, sum);
							break;
						case 4:
							printf("\n\n\t[%lld,%lld]�ڹ������� %d ����Ҷõ��������Ҷõ������ n = 4 ʱ����������\n\n\t", a, b, sum);
							break;
						case 5:
							printf("\n\n\t[%lld,%lld]�ڹ������� %d �������������������� n = 5 ʱ����������\n\n\t", a, b, sum);
							break;
						case 6:
							printf("\n\n\t[%lld,%lld]�ڹ������� %d ������������������ n = 6 ʱ����������\n\n\t", a, b, sum);
							break;
						case 7:
							printf("\n\n\t[%lld,%lld]�ڹ������� %d �������������������������� n = 7 ʱ����������\n\n\t", a, b, sum);
							break;
						case 8:
							printf("\n\n\t[%lld,%lld]�ڹ������� %d ������������������ n = 8 ʱ����������\n\n\t", a, b, sum);
							break;
						case 9:
							printf("\n\n\t[%lld,%lld]�ڹ�������%d���ž����������ž��������� n = 9 ʱ����������\n\n\t", a, b, sum);
							break;
						case 10:
							printf("\n\n\t[%lld,%lld]�ڹ�������%d��ʮȫʮ������ʮȫʮ������ n = 10 ʱ����������\n\n\t", a, b, sum);
							break;
						default:
							break;
					}
					for (j = 0; j < sum; j++) printf("   %lld", tank[j]);
				}
			}
			finish = clock();
			total_time = (float)((finish - start) / 1000.0);
			printf("\n\n\t�˴���ʱ %f s", total_time);
			printf("\n\n\n\t[%lld,%lld]��Χ�ڵ���������ȫ���ҳ���\n\n\t-------------------------------------------------\n\n\n\n", a, b);
		}
		else
			break;
	} while(1);
}
