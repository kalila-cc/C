#include <stdio.h>
#include <math.h>

#define PI 3.141593
#define APART 1024

char multiPrintCharWithSpaceSuffix(char c, int repeat_num, int space_num);
int nCr(int n, int r); 

int main() {
	int i, j, k, m, n, r, choose, delta, line, tempx, tempy;
	int L1 = (int)(0.25 * APART);
	int L2 = (int)(0.75 * APART);
	char IN, tank = 'A';
	printf("\n\n\t这是一个图表绘制程序，可以用于绘制某些图形或列表，退出各层模块请输入-1：");
	do {
		printf("\n\n\t请选择需要绘制的图案：");
		printf("\n\t1.圆形\t\t2.沙漏\n\t3.杨辉三角\t4.字母金字塔\n\n\t");
		scanf("%d", &choose);
		if (choose == 1) {
			printf("\n\n\t已选模块：1.圆形\n\t");
			printf("\n\t请输入圆的半径：\n\n\t"); 
			scanf("%d", &r);
			int YZ[2 * r + 2];
			int delta[2 * r + 2];
			for (n = 0; n < 2 * r + 2; n++) YZ[n] = 0;
			for (k = L1; k < L2 + 1; k++) {
				tempx = (int)(r + r * cos((2 * k * PI) / APART));
				tempy = (int)(r + r * sin((2 * k * PI) / APART));
				YZ[tempy] = (int)(1.8 * tempx);
				delta[tempy] = (int)(1.8 * (-2 * r * cos((2 * k * PI) / APART)));
			}
			printf("\n\n");
			for (i = 0; i < 2 * r; i++) {
				printf("\n\t\t\t");
				for (j = 0; j < YZ[i]; j++) putchar(' ');
				putchar('0');
				if (i != 0 && i != 2 * r + 1) {
					for (m = 0; m < delta[i]; m++) putchar(' ');
					putchar('0');
				}
			}
		} else if(choose == 2) {
			printf("\n\n\t已选模块：2.沙漏\n\t");
			printf("\n\n\t请输入沙漏的半高度：\n\n\t");
			scanf("%d", &n);
			printf("\n\n");
			for (i = 1; i <= n; i++) {
				putchar('\t');
				multiPrintCharWithSpaceSuffix(' ', 2 * i, 0);
				multiPrintCharWithSpaceSuffix('*', 2 * (n - i) + 1, 1);
				printf("\n\n");
			}
			for (i = 2; i <= n; i++) {
				putchar('\t');
				multiPrintCharWithSpaceSuffix(' ', 2 * (n - i + 1), 0);
				multiPrintCharWithSpaceSuffix('*', 2 * i - 1, 1);
				printf("\n\n");
			}
		} else if(choose == 3) {
			printf("\n\n\t已选模块：3.杨辉三角\n\t");
			printf("\n\n\t请输入杨辉三角所需行数\n\n\t");
			scanf("%d", &r);
			for (i = 0; i < r; i++) {
				for(j = 0; j < r - i; j++) printf("     ");
				for(k = 0; k < i + 1; k++) {
					printf("%5d     ", nCr(i, k));
					if (k == i) putchar('\n');
				}
			}
		} else if(choose == 4) {
			printf("\n\n\t已选模块：4.字母金字塔\n\t");
			printf("\n\t请输入一个大写字母 : \t");
			fflush(stdin); 
			scanf("%c", &IN);
			delta = IN - 'A';
			line = delta;
			if (IN == 'A') {
                printf("\n\n\tA");
            } else {
				putchar('\n');
				for (i = 0; i <= line; i++) {
					putchar('\t');
					for (j = 0; j < delta; j++) putchar(' ');
					for (j = 0; j <= i; j++, tank++) printf("%c", tank);
					for(j = 0; j < i; j++) printf("%c", tank - j - 2);
					delta--;
					putchar('\n');
					tank = 'A'; 
				}
		   }
		} else {
            break;
        }
	} while(1);
	return 0;
}

char multiPrintCharWithSpaceSuffix(char c, int repeat_num, int space_num) { 
	int i, j;
	for (i = 0; i < repeat_num; i++) {
        putchar(c);
		for(j = 0; j < space_num; j++) putchar(' ');
	}
}

int nCr(int n, int r) {
    int i, res = 1;
    if ((r << 1) > n) r = n - r;
    for (i = 1; i <= r; i++, n--) {
			res *= n;
			res /= i;
		}
    return res;
}
