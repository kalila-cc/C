#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <windows.h>
#define MAX_ELEMENT 50

// function declaration
void disp(int *, int);
void swap(int *, int *);
void heapAdjust(int *, int, int);
void bubble_sort(int *, int);
void bucket_sort(int *, int);
void select_sort(int *, int);
void insert_sort(int *, int);
void heap_sort(int *, int);
void shell_sort(int *, int);
void merge_sort(int *, int);
void quick_sort(int *, int);
void counting_sort(int *, int);
void radix_sort(int *, int);

// function pointer list
void (*func_pointer_list[])(int *, int) = {
    bubble_sort, select_sort, insert_sort, merge_sort, quick_sort, 
    heap_sort, shell_sort, bucket_sort, radix_sort, counting_sort
};
// function name list
char func_name_list[][10] = {
	"bubble", "select", "insert", "merge", "quick", 
    "heap", "shell", "bucket", "radix", "counting"
};
// main function
int main() {
	// variable declaration
	const char *save_path = "./sort_data.csv";
    int i, export_data, is_test;
	int cur_times, loop_times;
	int size, size_min, size_max, size_step;
	int func_index, func_num;
    int *src, *cpy;
    double *use_time_list;
    LARGE_INTEGER nFreq, t1, t2;
	FILE *fp;
	// variable assignment
	export_data = 1;
	is_test = 0;
	// test config
	size = 20;
	func_index = 0;
	// comparison config
	loop_times = 100;
	size_min = 8000;
	size_max = 8000;
	size_step = 500;
	// other config
	func_num = sizeof(func_name_list) / 10;
	use_time_list = (double *)malloc(func_num * sizeof(double));
	QueryPerformanceFrequency(&nFreq);
    srand((unsigned)time(NULL));
	// execute
	if (is_test) {
		// for test
		printf("test:\n");
		src = (int *)malloc(size * sizeof(int));
		for (i = 0; i < size; i++) src[i] = rand() % MAX_ELEMENT + 1;
		disp(src, size);
		QueryPerformanceCounter(&t1);
		func_pointer_list[func_index](src, size);
		QueryPerformanceCounter(&t2);
		use_time_list[func_index] = (double)1e6 * (double)(t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart;
		disp(src, size);
		printf("%10s sort takes %.3f us\n", func_name_list[func_index], use_time_list[func_index]);
		free(use_time_list);
		free(src);
		system("PAUSE");
	} else {
		// for comparison
		if (export_data) {
			remove(save_path);
			fp = fopen(save_path, "w");
			fprintf(fp, "%s, ", "size");
			for (func_index = 0; func_index < func_num - 1; func_index++) fprintf(fp, "%s, ", func_name_list[func_index]);
			fprintf(fp, "%s\n", func_name_list[func_index]);
		}
		printf("comparison:\n");
		printf("\nrepeat times: %d\n", loop_times);
		for (size = size_min; size <= size_max; size += size_step) {
			// allocate array
			src = (int *)malloc(size * sizeof(int));
			cpy = (int *)malloc(size * sizeof(int));
			// clear time record
			for (i = 0; i < func_num; i++) use_time_list[i] = 0.0;
			// each function will sort the array loop_times times
			for (cur_times = 0; cur_times < loop_times; cur_times++) {
				// assign random values to the array
				for (i = 0; i < size; i++) src[i] = rand() % MAX_ELEMENT + 1;
				// call each function to sort
				for (func_index = 0; func_index < func_num; func_index++) {
					memcpy(cpy, src, size * sizeof(int));
					printf("\rin %10s", func_name_list[func_index]);
					QueryPerformanceCounter(&t1);
					func_pointer_list[func_index](cpy, size);
					QueryPerformanceCounter(&t2);
					use_time_list[func_index] += (double)1e6 * (double)(t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart;
				}
			}
			printf("\r             ");
			// output information
			printf("\narray size: %d\n", size);
			for (i = 0; i < func_num; i++) printf("%10s sort takes %.3f us\n", func_name_list[i], use_time_list[i]);
			// release allocated memory
			free(src);
			free(cpy);
			if (export_data) {
				fprintf(fp, "%d, ", size);
				for (func_index = 0; func_index < func_num - 1; func_index++) {
					fprintf(fp, "%.3f, ", use_time_list[func_index]);
				}
				fprintf(fp, "%.3f\n", use_time_list[func_index]);
			}
		}
		free(use_time_list);
		if (export_data) {
			fclose(fp);
            printf("file '%s' has been generated.\n", save_path);
		}
		system("PAUSE");
	}
}

void disp(int *p, int length) {
    int i, j;
    for (i = 0; i < length; i++) {
        printf("%2d : ", p[i]);
        for (j = 0; j < p[i]; j++)
            printf("��");
        printf("\n");
    }
	printf("\n");
}

void swap(int *p1, int *p2) {
    int temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

void heapAdjust(int *p, int length, int i) {
	// �Ե����ϵ����ѵ�˳��
    int k;
    // �õ������ӽڵ��±�
    int L = (i << 1) + 1;
    int R = (i << 1) + 2;
    while (L < length) {
        // ��ȡ����ӽڵ�
        k = R >= length ? L : (p[L] > p[R] ? L : R);
        if (p[i] < p[k]) {
            // �ӽڵ���ڽڵ������򽻻�����
            swap(p + i, p + k);
            i = k;
            // �ѱ����ң����»ص��ӽڵ㴦��ʼ������
            L = (i << 1) + 1;
            R = (i << 1) + 2;
        } else {
            // �ӽڵ�С�ڽڵ����ݣ��ѵ������
            break;
        }
    }
}

void bucket_sort(int *p, int length) {
    int i, j, k, min, max, gap, bucket_num;
    int *count, *start, *tmp, **bucket, *index;
    // �ҵ���Сֵ���ֵ�±�
    for (i = 1, min = max = 0; i < length; i++) {
        if (p[min] > p[i]) {
            min = i;
        } else if (p[max] < p[i]) {
            max = i;
        }
    }
    // �õ���Сֵ���ֵ
    min = p[min];
    max = p[max];
    // ����ÿ��Ͱ�����ݷ�Χ
    gap = (max - min) / length + 1;
    // �������ݷ�Χ����Ͱ������
    bucket_num = (max - min) / gap + 1;
    index = (int *)malloc(length * sizeof(int));
    // count ����ÿ��Ͱ�����������ȳ�ʼ��
    count = (int *)calloc(bucket_num, sizeof(int));
    // start �ں�����Ͱ��ֵ��Ϊ����λ�ü�¼
    start = (int *)calloc(bucket_num, sizeof(int));
    bucket = (int **)malloc(bucket_num * sizeof(int *));
    // ���ÿ������Ӧ�ô���Ͱ��λ��
    for (i = 0; i < length; i++)
        index[i] = (p[i] - min) / gap;
    // ��һ��ÿ��Ͱ�ж��ٸ�����
    for (i = 0; i < length; i++)
        count[index[i]]++;
    // ��ÿ��Ͱ�����Ӧ�Ĵ�С
    for (i = 0; i < bucket_num; i++)
        bucket[i] = (int *)malloc(count[i] * sizeof(int));
    // ��ÿ��Ͱװ������
    for (i = 0; i < length; i++) {
        j = index[i];
        bucket[j][start[j]++] = p[i];
    }
    // �ü��������ÿ��Ͱ�����ݽ�������
    for (i = 0; i < bucket_num; i++)
        counting_sort(bucket[i], count[i]);
    for (i = k = 0; i < bucket_num; i++) {
        tmp = bucket[i];
        // ��ÿ��Ͱ�����ݵ���ԭ����
        for (j = 0; j < count[i]; j++)
            p[k++] = tmp[j];
    }
    // �ͷ�ָ���ڴ�
    for (i = 0; i < bucket_num; i++)
        free(bucket[i]);
    free(count);
    free(start);
    free(index);
    free(bucket);
}

void radix_sort(int *p, int length) {
    int i, max, base, *tmp, start[10], count[10];
    tmp = (int *)malloc(length * sizeof(int));
    // ��ȡ���ֵ
    max = 0;
    for (i = 1; i < length; i++)
        if (p[max] < p[i])
            max = i;
    // �õ����ֵ
    max = p[max];
    // base �Ӹ�λ��ʼ��ÿ�β��� * 10
    for (base = 1; max > 0; base *= 10, max /= 10) {
        memset(count, 0, 10 * sizeof(int));
        // ��ȡ��Ӧλ������
        for (i = 0; i < length; i++)
            count[p[i] / base % 10]++;
        // ����ֱ������0-9�����ַ�Χ�����ݵ�������
        for (i = 1, start[0] = 0; i < 10; i++)
            start[i] = start[i - 1] + count[i - 1];
        // ���������Ӧ�����Ӵ�װ�����ݣ�����������һλ
        for (i = 0; i < length; i++)
            tmp[start[p[i] / base % 10]++] = p[i];
        // ��������
        memcpy(p, tmp, length * sizeof(int));  
    }
    free(tmp);
}

void quick_sort(int *p, int length) {
    int trap = 0, i = 1, j = length - 1, x = p[0];
    if (length > 1) {
        //�� i��j ���� x ���ҷ�������
        while (i <= j) {
            // j ��Ҫ�ҵ��� x С��
            while (j >= i && p[j] > x)
                j--;
             //�ҵ����򽻻�
            if (j >= i) {
                p[trap] = p[j];
                trap = j--;
            }
            // i ��Ҫ�ҵ��� x ���
            while (j >= i && p[i] <= x)
                i++;
            // �ҵ����򽻻�
            if (j >= i) {
                p[trap] = p[i];
                trap = i++;
            }
        }
        // ��ӣ���Ϊ x ��λ���Ѿ��������ˣ�trap ��λ�ÿ���
        p[trap] = x;
        // �ݹ���Ź�������ߵ�����
        quick_sort(p, trap);
        // �ݹ���Ź�������ߵ�����
        quick_sort(p + trap + 1, length - trap - 1);
    }
}

void counting_sort(int *p, int length) {
    int i, j, k, *t, range, min = 0, max = 0;
    for (i = 1; i < length; i++)  //������ֵ��Сֵ
        if (p[i] > p[max])
            max = i;
        else if (p[i] < p[min])
            min = i;
    // �������ݷ�Χ
    range = p[max] - p[min] + 1;
    min = p[min];
    t = (int *)calloc(range, sizeof(int));
    // ��ʼ����
    for (i = 0; i < length; i++)
        t[p[i] - min]++;
    // ����������������
    for (i = k = 0; i < range; i++)
        for (j = 0; j < t[i]; j++)
            p[k++] = i + min;
    free(t);
}

void heap_sort(int *p, int length) {
    int i;
    // ����ԭʼ��
    for (i = (length >> 1) - 1; i >= 0; i--)
        heapAdjust(p, length, i);
    for (i = 1; i < length; i++) {
        // �����Ѷ��Ͷѵ�Ԫ��
        swap(p, p + length - i);
        // ϸ����
        heapAdjust(p, length - i, 0);
    }
}

void merge_sort(int *p, int length) {
    int i, j, k, mid, last, *arr, *m;
    if (length > 2) {
        i = j = k = 0;
        arr = (int *)malloc(length * sizeof(int));
        m = p + mid;
        mid = length >> 1;
        last = mid + (length & 1);
        // �����ݷ�Ϊ��������ݹ���η�����
        merge_sort(p, mid);
        merge_sort(m, last);
        // �ۺ�����������������һһ�ԱȰ����ݰ���С������ʱ����
        while (i < mid && j < last) {
            if (p[i] < m[j]) {
                arr[k++] = p[i++];
            } else {
                arr[k++] = m[j++];
            }
        }
        // �� i �ж�������������ʱ����
        while (i < mid)
            arr[k++] = p[i++];
        // �� j �ж�������������ʱ����
        while (j < last)
            arr[k++] = m[j++];
        // װ������
        memcpy(p, arr, length * sizeof(int));
        free(arr);
    } else if (length > 1) {
        if (p[0] > p[1]) {
            swap(p, p + 1);
        }
    }
}

void shell_sort(int *p, int length) {
	int i, j, k, t, gap;
    // �� gap ����������в�������
	for (gap = length / 2; gap > 0; gap /= 2) {
        // ���� gap ����Ҫ��������i Ϊÿ�����Ԫ��
		for (i = 0; i < gap; i++) {
            // ÿѭ��һ�Σ�δ���򲿷���Ԫ���±����� gap ����������Ϊ���������ԭ��
			for (j = i + gap; j < length; j += gap) {
				t = p[j];
				for (k = j - gap; k >= 0 && p[k] > t; k -= gap)
					p[k + gap] = p[k];
				p[k + gap] = t;
			}
		}
	}
}

void insert_sort(int *p, int length) {
    int i, j, temp;
    for (i = 1; i < length; i++) {
        // ��¼�� i + 1 ��������Ҫ�Աȵ����ұߵ����ݣ�ֻҪ��ߵ�С�� temp ������
        temp = p[i];
        for (j = i - 1; j >= 0 && p[j] > temp; j--)
            p[j + 1] = p[j];
        p[j + 1] = temp;
    }
}

void select_sort(int *p, int length) {
    int i, j, k;
    for (i = 0; i < length - 1; i++) {
        k = i;
        for (j = i + 1; j < length; j++) {
            if (p[k] > p[j]) {
                k = j;
            }
        }
        if (k != i) {
            swap(p + k, p + i);
        }
    }
}

void bubble_sort(int *p, int length) {
    int i, j, k, flag;
    for (i = 0; i < length - 1; i++) {
        flag = 1;
        k = length - 1 - i;
        for (j = 0; j < k; j++)
            if (p[j] > p[j + 1]) {
                swap(p + j, p + j + 1);
                flag = 0;
            }
        if (flag)
            break;
    }
}
