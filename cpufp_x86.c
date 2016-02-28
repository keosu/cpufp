#define _GNU_SOURCE
#include "cpufp_x86.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

static double get_time(struct timespec *start,
    struct timespec *end)
{
    return end->tv_sec - start->tv_sec +
        (end->tv_nsec - start->tv_nsec) * 1e-9;
}

#if defined(_USE_X86_SSE)

#define FP32_COMP (0x60000000L * 64)
#define FP64_COMP (0x60000000L * 32)

static void* thread_func_fp32(void *params)
{
    cpufp_x86_sse_fp32();
    return NULL;
}

static void* thread_func_fp64(void *params)
{
    cpufp_x86_sse_fp64();
    return NULL;
}

#elif defined(_USE_X86_AVX)

#define FP32_COMP (0x200000000L * 16)
#define FP64_COMP (0x200000000L * 8)

static void* thread_func_fp32(void *params)
{
    cpufp_x86_avx_fp32();
    return NULL;
}

static void* thread_func_fp64(void *params)
{
    cpufp_x86_avx_fp64();
    return NULL;
}

#elif defined(_USE_X86_FMA)

#define FP32_COMP (0x80000000L * 160)
#define FP64_COMP (0x80000000L * 80)

static void* thread_func_fp32(void *params)
{
    cpufp_x86_fma_fp32();
    return NULL;
}

static void* thread_func_fp64(void *params)
{
    cpufp_x86_fma_fp64();
    return NULL;
}

#endif

static void cpufp_x86(int num_cores)
{
    int i;
    struct timespec start, end;
    double time_used, perf;

    pthread_t *tids = (pthread_t*)calloc(num_cores, sizeof(pthread_t));
   
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (i = 0; i < num_cores; i++)
    {
        pthread_create(&tids[i], NULL, thread_func_fp32, (void*)&i);
    }
    for (i = 0; i < num_cores; i++)
    {
        pthread_join(tids[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    time_used = get_time(&start, &end);
    perf = FP32_COMP * num_cores / time_used * 1e-9;
    printf("FP32 perf: %.4lf GFLOPS.\n", perf);

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (i = 0; i < num_cores; i++)
    {
        pthread_create(&tids[i], NULL, thread_func_fp64, (void*)&i);
    }
    for (i = 0; i < num_cores; i++)
    {
        pthread_join(tids[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    time_used = get_time(&start, &end);
    perf = FP64_COMP * num_cores / time_used * 1e-9;
    printf("FP64 perf: %.4lf GFLOPS.\n", perf);

    free(tids);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s num_cores.\n", argv[0]);
        exit(0);
    }

    int num_cores = atoi(argv[1]);
    printf("Core used: %d\n", num_cores);

    cpufp_x86(num_cores);

    return 0;
}

