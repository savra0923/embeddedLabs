#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <stddef.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sysmon_ipc.h>
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

pid_t spawn_sensor(const char *path)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
    }

    if (pid == 0)
    {
        char *args[] = {(char *)path, NULL};
        char *env[] = {NULL};

        execve(path, args, env);

        perror("execve");
    }

    return pid;
}

int main(void)
{
    int fds[2];
    pid_t pid;

    pipe(fds);
    pid = fork();

    if (pid == 0)  /* Child */
    {
        close(fds[0]);  // Child only writes

        char fd_arg[20];
        snprintf(fd_arg, sizeof(fd_arg), "%d", fds[1]);
        
        char *args[] = {
            "mem_sensor",
            fd_arg,
            NULL};

        execve("./bin/mem_sensor", args, NULL);

        perror("execve");
    }
    else /* Parent */
    {
        close(fds[1]);  // Parent only reads

        if (mkfifo(CPU_FIFO_PATH, 0666) == -1)
        {
            if (errno != EEXIST)
                perror("mkfifo");
        }

        pid_t cpu_pid = spawn_sensor("./bin/cpu_sensor");

        int cpu_fd = open(CPU_FIFO_PATH, O_RDONLY);
        char buffer_cpu[1024];

        if (cpu_fd == -1)
        {
            perror("open CPU FIFO");
        }

        FILE *stream = fdopen(fds[0], "r");

        if (stream == NULL)
        {
            perror("fdopen");
            return 1;
        }

        char buffer[1024];

        while (fgets(buffer, sizeof(buffer), stream) != NULL) 
        {
            MemData info;

            sscanf(buffer, "%ld %ld %ld %ld %ld", &info.mem_total, &info.mem_free,
                &info.mem_avail, &info.swap_total, &info.swap_free);

            printf("----------MEMDATA----------\n");
            printf("Total memory: %ld\n", info.mem_total);
            printf("Free memory: %ld\n", info.mem_free);
            printf("Available memory: %ld\n", info.mem_avail);
            printf("Total swap: %ld\n", info.swap_total);
            printf("Free swap: %ld\n", info.swap_free);

            CpuData data;

            ssize_t bytes = read(cpu_fd, &data, sizeof(data));

            if (bytes == sizeof(data))
            {
                printf("CPU load: %.2f %.2f %.2f\n",
                    data.load1, data.load5, data.load15);
            }
        }
        
        close(cpu_fd);
        fclose(stream);
        waitpid(pid, NULL, 0);
        
    }
    

	return 0;
}
