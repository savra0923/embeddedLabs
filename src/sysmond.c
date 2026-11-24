#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <stddef.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sysmon_ipc.h>
//#include "mem_sensor.h"
#include "cpu_sensor.h"
#include "proc_sensor.h"

int print_system_info(MemData *mem_info, CpuData *cpu_info, ProcData *proc_info)
{
    fprintf(stdout, "\nSystem Information:\n");
    fprintf(stdout, "MemTotal:     %zu kB\n", mem_info->mem_total);
    fprintf(stdout, "MemFree:      %zu kB\n", mem_info->mem_free);
    fprintf(stdout, "MemAvailable: %zu kB\n", mem_info->mem_avail);
    fprintf(stdout, "SwapTotal:    %zu kB\n", mem_info->swap_total);
    fprintf(stdout, "SwapFree:     %zu kB\n", mem_info->swap_free);
    fprintf(stdout, "1 min:  %.2f\n", cpu_info->load1);
    fprintf(stdout, "5 min:  %.2f\n", cpu_info->load5);
    fprintf(stdout, "15 min: %.2f\n", cpu_info->load15); 
    fprintf(stdout, "Number of running processes: %d\n", proc_info->total_procs);

    return 0;
}

pid_t spawn_sensor(const char *path, const char *fifo_path, int *fd)
{
    if (mkfifo(fifo_path, 0666) == -1 && errno != EEXIST)
    {
        perror("mkfifo");
        return -1;
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return -1;
    }

    if (pid == 0)
    {
        char *args[] = {(char *)path, NULL};

        execve(path, args, NULL);

        perror("execve");
        _exit(EXIT_FAILURE);
    }

    *fd = open(fifo_path, O_RDONLY);

    if (*fd == -1)
    {
        perror("open FIFO");
        return -1;
    }

    return pid;
}

void anonymous_pipe(int fds1)
{
    char fd_arg[20];
    snprintf(fd_arg, sizeof(fd_arg), "%d", fds1);
        
    char *args[] = {
        "mem_sensor",
        fd_arg,
        NULL};

    execve("./bin/mem_sensor", args, NULL);

    perror("execve");
}

static int read_full(int fd, void *buffer, size_t size)
{
    size_t total = 0;
    char *ptr = buffer;

    while (total < size)
    {
        ssize_t bytes = read(fd, ptr + total, size - total);

        if (bytes == -1)
        {
            if (errno == EINTR)
                continue;

            return -1;
        }

        if (bytes == 0)
            return total == 0 ? 0 : -1;

        total += bytes;
    }

    return 1;
}

void *reader_thread(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    SharedData *shared = args->shared;

    if (args->sensor_type == SENSOR_MEM)
    {
        FILE *stream = fdopen(args->fd, "r");

        if (stream == NULL)
        {
            perror("fdopen");
            return NULL;
        }

        char buffer[1024];

        while (fgets(buffer, sizeof(buffer), stream) != NULL)
        {
            MemData data;

            if (sscanf(buffer, "%zu %zu %zu %zu %zu",
                       &data.mem_total,
                       &data.mem_free,
                       &data.mem_avail,
                       &data.swap_total,
                       &data.swap_free) != 5)
            {
                fprintf(stderr, "Invalid memory sensor data\n");
                continue;
            }

            pthread_mutex_lock(&shared->mutex);

            shared->mem = data;
            shared->mem_valid = 1;

            pthread_mutex_unlock(&shared->mutex);
        }

        fclose(stream);
    }
    else if (args->sensor_type == SENSOR_CPU)
    {
        while (1)
        {
            CpuData data;

            int result = read_full(args->fd, &data, sizeof(data));

            if (result <= 0)
                break;

            pthread_mutex_lock(&shared->mutex);

            shared->cpu = data;
            shared->cpu_valid = 1;

            pthread_mutex_unlock(&shared->mutex);
        }
    }
    else if (args->sensor_type == SENSOR_PROC)
    {
        while (1)
        {
            ProcData data;

            int result = read_full(args->fd, &data, sizeof(data));

            if (result <= 0)
                break;

            pthread_mutex_lock(&shared->mutex);

            shared->proc = data;
            shared->proc_valid = 1;

            pthread_mutex_unlock(&shared->mutex);
        }
    }

    return NULL;
}

int main(void)
{
    int fds[2];

    if (pipe(fds) == -1)
    {
        perror("pipe");
        return 1;
    }

    pid_t mem_pid = fork();

    if (mem_pid == -1)
    {
        perror("fork");
        return 1;
    }

    if (mem_pid == 0)
    {
        close(fds[0]);

        anonymous_pipe(fds[1]);
        _exit(EXIT_FAILURE);
    }

    /* Parent process */

    close(fds[1]);

    int cpu_fd;
    pid_t cpu_pid = spawn_sensor(
        "./bin/cpu_sensor", CPU_FIFO_PATH, &cpu_fd);

    if (cpu_pid == -1)
        return 1;

    int proc_fd;
    pid_t proc_pid = spawn_sensor(
        "./bin/proc_sensor", PROC_FIFO_PATH, &proc_fd);

    if (proc_pid == -1)
        return 1;

    /* Initialize shared data */

    SharedData shared = {0};

    pthread_mutex_init(&shared.mutex, NULL);

    /* Prepare thread arguments */

    ThreadArgs args[3] =
    {
        {fds[0], SENSOR_MEM,  &shared},
        {cpu_fd, SENSOR_CPU,  &shared},
        {proc_fd, SENSOR_PROC, &shared}
    };

    pthread_t threads[3];

    /* Create reader threads */

    for (int i = 0; i < 3; i++)
    {
        int ret = pthread_create(
            &threads[i],
            NULL,
            reader_thread,
            &args[i]);

        if (ret != 0)
        {
            fprintf(stderr, "pthread_create: %s\n", strerror(ret));
            return 1;
        }
    }

    /* Main thread prints the latest sensor data */

    while (1)
    {
        MemData mem;
        CpuData cpu;
        ProcData proc;

        int ready = 0;

        pthread_mutex_lock(&shared.mutex);

        if (shared.mem_valid &&
            shared.cpu_valid &&
            shared.proc_valid)
        {
            mem = shared.mem;
            cpu = shared.cpu;
            proc = shared.proc;

            ready = 1;
        }

        pthread_mutex_unlock(&shared.mutex);

        if (ready)
        {
            print_system_info(&mem, &cpu, &proc);
        }

        sleep(1);
    }
    

	return 0;
}
