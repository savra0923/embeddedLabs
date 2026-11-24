#ifndef SYSMON_IPC_H
#define SYSMON_IPC_H

#define CPU_FIFO_PATH  "/tmp/sysmon_cpu.fifo"
#define PROC_FIFO_PATH "/tmp/sysmon_proc.fifo"
#define INTERVAL 5
#define SENSOR_MEM  0
#define SENSOR_CPU  1
#define SENSOR_PROC 2

#include <pthread.h>

typedef struct {
    long mem_total;
    long mem_free;
    long mem_avail;
    long swap_total;
    long swap_free;
} MemData;

typedef struct {
    double load1;
    double load5;
    double load15;
} CpuData;

typedef struct {
    int total_procs;
} ProcData;

typedef struct
{
    MemData mem;
    CpuData cpu;
    ProcData proc;

    int mem_valid;
    int cpu_valid;
    int proc_valid;

    pthread_mutex_t mutex;
} SharedData;

typedef struct
{
    int fd;
    int sensor_type;
    SharedData *shared;
} ThreadArgs;

#define SENSOR_MEM  0
#define SENSOR_CPU  1
#define SENSOR_PROC 2

#endif /* SYSMON_IPC_H */