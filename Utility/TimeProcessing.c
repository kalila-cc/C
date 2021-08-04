#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <sys/time.h>

int main() {
    int sleep_time = 200;
    double dtime;
    time_t t = time(NULL);
    struct tm *pt = localtime(&t);
    // task <clock>, needs <time.h>
    clock_t clock_start, clock_end;
    // task <QueryPerformanceCounter>, needs <windows.h>
    LARGE_INTEGER nFreq, QueryPerformanceCounter_start, QueryPerformanceCounter_end;
    QueryPerformanceFrequency(&nFreq);
    // task <gettimeofday>, needs <sys/time.h>
    struct timeval gettimeofday_start, gettimeofday_end;
    // task <GetTickCount>, needs <windows.h>
    double GetTickCount_start, GetTickCount_end;

    // 记录开始时间
    
    
    // 时间格式化函数
    printf("Timestamp : %ld\n", t);
    printf("Timestamp: %ld\n", mktime(pt));
    printf("Local time : %s\n", asctime(pt));
    printf("Local time : %s\n", ctime(&t));
    printf("Local time : %d-%02d-%02d %02d:%02d:%02d\n", pt->tm_year + 1900, pt->tm_mon + 1, pt->tm_mday, pt->tm_hour, pt->tm_min, pt->tm_sec);
    
    // 记录结束时间
    clock_start = clock();
    Sleep(sleep_time);
    clock_end = clock();
    dtime = 1e3 * (double)(clock_end - clock_start) / CLOCKS_PER_SEC;
    printf("task <clock> takes %.0lf ms\n", dtime);

    QueryPerformanceCounter(&QueryPerformanceCounter_start);
    Sleep(sleep_time);
    QueryPerformanceCounter(&QueryPerformanceCounter_end);
    dtime = (double)1e6 * (double)(QueryPerformanceCounter_end.QuadPart - QueryPerformanceCounter_start.QuadPart) / (double)nFreq.QuadPart;
    printf("task <QueryPerformanceCounter> takes %.0f us\n", dtime);

    gettimeofday(&gettimeofday_start, NULL);
    Sleep(sleep_time);
    gettimeofday(&gettimeofday_end, NULL);
    dtime = (gettimeofday_end.tv_sec - gettimeofday_start.tv_sec) + (gettimeofday_end.tv_usec - gettimeofday_start.tv_usec) / 1e6;
    printf("task <gettimeofday> takes %.6lf s\n", dtime);

    GetTickCount_start = GetTickCount();
    Sleep(sleep_time);
    GetTickCount_end = GetTickCount();
    dtime = GetTickCount_end - GetTickCount_start;
    printf("task <GetTickCount> takes %.0lf ms\n", dtime);

    getchar();
}
