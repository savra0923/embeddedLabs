#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <stddef.h>
#include <sys/wait.h>
#include <poll.h>
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
    }

    *fd = open(fifo_path, O_RDONLY);

    if (*fd == -1)
    {
        perror("open FIFO");
        return -1;
    }

    return pid;
}

int anonymous_pipe(int fds1)
{
    char fd_arg[20];
    snprintf(fd_arg, sizeof(fd_arg), "%d", fds1);
        
    char *args[] = {
        "mem_sensor",
        fd_arg,
        NULL};

    execve("./bin/mem_sensor", args, NULL);

    perror("execve");
    return 0;
}

int read_memory_line(int fd, char *buffer, size_t size)
{
    size_t total = 0;

    while (total < size - 1)
    {
        char c;
        ssize_t bytes = read(fd, &c, 1);

        if (bytes == -1)
        {
            if (errno == EINTR)
                continue;

            return -1;
        }

        if (bytes == 0)
            return total == 0 ? 0 : -1;

        if (c == '\n')
            break;

        buffer[total++] = c;
    }

    buffer[total] = '\0';

    return 1;
}

int read_full(int fd, void *buffer, size_t size)
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

    /* Initialize poll descriptors */

    struct pollfd pfds[3];

    pfds[0].fd = fds[0];
    pfds[0].events = POLLIN;

    pfds[1].fd = cpu_fd;
    pfds[1].events = POLLIN;

    pfds[2].fd = proc_fd;
    pfds[2].events = POLLIN;

    /* Store the latest sensor readings */

    MemData mem_data = {0};
    CpuData cpu_data = {0};
    ProcData proc_data = {0};

    int mem_valid = 0;
    int cpu_valid = 0;
    int proc_valid = 0;

    char buffer[1024];

    while (1)
    {
        int ret = poll(pfds, 3, -1);

        if (ret == -1)
        {
            if (errno == EINTR)
                continue;

            perror("poll");
            break;
        }

        int updated = 0;

        /* Memory sensor */

        if (pfds[0].revents & POLLIN)
        {
            int result = read_memory_line(
                pfds[0].fd, buffer, sizeof(buffer));

            if (result == 1)
            {
                if (sscanf(buffer, "%ld %ld %ld %ld %ld",
                           &mem_data.mem_total,
                           &mem_data.mem_free,
                           &mem_data.mem_avail,
                           &mem_data.swap_total,
                           &mem_data.swap_free) == 5)
                {
                    mem_valid = 1;
                    updated = 1;
                }
                else
                {
                    fprintf(stderr, "Invalid memory data\n");
                }
            }
            else
            {
                printf("Memory sensor disconnected\n");
                close(pfds[0].fd);
                pfds[0].fd = -1;
            }
        }

        /* CPU sensor */

        if (pfds[1].revents & POLLIN)
        {
            int result = read_full(
                pfds[1].fd, &cpu_data, sizeof(cpu_data));

            if (result == 1)
            {
                cpu_valid = 1;
                updated = 1;
            }
            else
            {
                printf("CPU sensor disconnected\n");
                close(pfds[1].fd);
                pfds[1].fd = -1;
            }
        }

        /* Process sensor */

        if (pfds[2].revents & POLLIN)
        {
            int result = read_full(
                pfds[2].fd, &proc_data, sizeof(proc_data));

            if (result == 1)
            {
                proc_valid = 1;
                updated = 1;
            }
            else
            {
                printf("Process sensor disconnected\n");
                close(pfds[2].fd);
                pfds[2].fd = -1;
            }
        }

        /* Handle disconnected writers */

        for (int i = 0; i < 3; i++)
        {
            if (pfds[i].fd != -1 &&
                (pfds[i].revents & (POLLERR | POLLNVAL)))
            {
                fprintf(stderr, "Sensor FD %d error\n", i);
                close(pfds[i].fd);
                pfds[i].fd = -1;
            }
        }

        /* Print when we have at least one update */

        if (updated && mem_valid && cpu_valid && proc_valid)
        {
            print_system_info(
                &mem_data,
                &cpu_data,
                &proc_data);
        }

        /* Stop if all sensors have disconnected */

        if (pfds[0].fd == -1 &&
            pfds[1].fd == -1 &&
            pfds[2].fd == -1)
        {
            break;
        }
    }

    close(fds[0]);

    waitpid(mem_pid, NULL, 0);
    waitpid(cpu_pid, NULL, 0);
    waitpid(proc_pid, NULL, 0);

    return 0;
}
