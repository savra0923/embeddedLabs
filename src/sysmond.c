#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <stddef.h>
#include <sys/types.h>
#include "sysmond.h"
#include "mem_sensor.h"
#include "cpu_sensor.h"
#include "proc_sensor.h"

int make_output_dir(void)
{
    if (mkdir("/tmp/sysmon", 0755) == -1 && errno != EEXIST)
    {
        perror("mkdir");
        return -1;
    }
    return 0;
}

int read_mem_data(const char *file_path, mem_info_t *info) 
{
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) 
    {
        fprintf(stderr, "Failed to open %s: %s\n", file_path, strerror(errno));
        return -1;
    }


    char line[256];
    memset(info, 0, sizeof(mem_info_t));

    while (fgets(line, sizeof(line), fp) != NULL) 
    {
        if (sscanf(line, "MemTotal: %zu", &info->memTotal) == 1)
            continue;
        if (sscanf(line, "MemFree: %zu", &info->memFree) == 1)
            continue;
        if (sscanf(line, "MemAvailable: %zu", &info->memAvailable) == 1)
            continue;
        if (sscanf(line, "SwapTotal: %zu", &info->SwapTotal) == 1)
            continue;
        if (sscanf(line, "SwapFree: %zu", &info->SwapFree) == 1)
            continue;
    }

    fclose(fp);
    return 0;
}

int read_cpu_data(const char *file_path, cpu_info_t *info) 
{
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) 
    {
        fprintf(stderr, "Failed to open %s: %s\n", file_path, strerror(errno));
        return -1;
    }

    char line[256];
    memset(info, 0, sizeof(cpu_info_t));

    while (fgets(line, sizeof(line), fp) != NULL) 
    {
        if (sscanf(line, "1 min: %lf", &info->cpu1min) == 1)
            continue;
        if (sscanf(line, "5 min: %lf", &info->cpu5min) == 1)
            continue;
        if (sscanf(line, "15 min: %lf", &info->cpu15min) == 1)
            continue; 
    }

    fclose(fp);
    return 0;
}

int read_proc_data(const char *file_path, int *pid_count) 
{
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) 
    {
        fprintf(stderr, "Failed to open %s: %s\n", file_path, strerror(errno));
        return -1;
    }

    char line[256];
    *pid_count = 0;

    while (fgets(line, sizeof(line), fp) != NULL) 
    {
        if (sscanf(line, "Number of running processes: %d", pid_count) == 1)
            continue;
    }

    fclose(fp);
    return 0;
}

int print_system_info(mem_info_t *mem_info, cpu_info_t *cpu_info, int *pid_count)
{
    fprintf(stdout, "\nSystem Information:\n");
    fprintf(stdout, "MemTotal:     %zu kB\n", mem_info->memTotal);
    fprintf(stdout, "MemFree:      %zu kB\n", mem_info->memFree);
    fprintf(stdout, "MemAvailable: %zu kB\n", mem_info->memAvailable);
    fprintf(stdout, "SwapTotal:    %zu kB\n", mem_info->SwapTotal);
    fprintf(stdout, "SwapFree:     %zu kB\n", mem_info->SwapFree);
    fprintf(stdout, "1 min:  %.2f\n", cpu_info->cpu1min);
    fprintf(stdout, "5 min:  %.2f\n", cpu_info->cpu5min);
    fprintf(stdout, "15 min: %.2f\n", cpu_info->cpu15min); 
    fprintf(stdout, "Number of running processes: %d\n", *pid_count);

    return 0;
}

pid_t start_process(const char *program)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return -1;
    }

    if (pid == 0)
    {
        char *const args[] = {
            (char *)program,
            NULL
        };

        char *const env[] = { NULL };

        execve(program, args, env);

        // Reached only if execve fails
        perror("execve");
    }

    // Parent returns the child's PID
    return pid;
}

int main(void)
{
    make_output_dir();

    mem_info_t mem_info;
    cpu_info_t cpu_info;
    int pid_count = 0;

    pid_t cpu_pid;
    pid_t mem_pid;
    pid_t proc_pid;

    cpu_pid = start_process("./bin/cpu_sensor");
    if (cpu_pid < 0)
        printf("Failed to start CPU sensor\n");

    mem_pid = start_process("./bin/mem_sensor");
    if (mem_pid < 0)
        printf("Failed to start Memory sensor\n");

    proc_pid = start_process("./bin/proc_sensor");
    if (proc_pid < 0)
        printf("Failed to start Process sensor\n");

    printf("Started CPU sensor: PID %d\n", cpu_pid);
    printf("Started Memory sensor: PID %d\n", mem_pid);
    printf("Started Process sensor: PID %d\n", proc_pid);

    while (1) {
        read_mem_data(MEM_FILE, &mem_info);
        read_cpu_data(CPU_FILE, &cpu_info);
        read_proc_data(PROC_FILE, &pid_count);
        sleep(READ_INTERVAL);

        print_system_info(&mem_info, &cpu_info, &pid_count);
        sleep(PRINT_INTERVAL);
    }

	return 0;
}
