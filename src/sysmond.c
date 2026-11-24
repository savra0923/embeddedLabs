#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <stddef.h>
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



int main(void)
{
    int fds[2];
    pid_t pid;

    pipe(fds);
    pid = fork();

    if (pid == 0)  /* Child */
    {
        close(fds[0]);  // Child only writes

        anonymous_pipe(fds[1]);
    }
    else /* Parent */
    {
        close(fds[1]);  // Parent only reads

        if (mkfifo(CPU_FIFO_PATH, 0666) == -1)
        {
            if (errno != EEXIST)
                perror("mkfifo");
        }

        int cpu_fd;
        pid_t cpu_pid = spawn_sensor("./bin/cpu_sensor", CPU_FIFO_PATH, &cpu_fd);

        int proc_fd;
        pid_t proc_pid = spawn_sensor("./bin/proc_sensor", PROC_FIFO_PATH, &proc_fd);

        FILE *stream = fdopen(fds[0], "r");

        if (stream == NULL)
        {
            perror("fdopen");
            return 1;
        }

        char buffer[1024];

        while (fgets(buffer, sizeof(buffer), stream) != NULL) 
        {
            MemData mem_data;
            CpuData cpu_data;
            ProcData proc_data;

            sscanf(buffer, "%ld %ld %ld %ld %ld", &mem_data.mem_total, &mem_data.mem_free,
                &mem_data.mem_avail, &mem_data.swap_total, &mem_data.swap_free);

            read(cpu_fd, &cpu_data, sizeof(cpu_data));
            read(proc_fd, &proc_data, sizeof(proc_data));

            print_system_info(&mem_data, &cpu_data, &proc_data);
        }

        close(proc_fd);
        close(cpu_fd);
        fclose(stream);
        waitpid(pid, NULL, 0);
        waitpid(cpu_pid, NULL, 0);
        waitpid(proc_pid, NULL, 0);
    }
    

	return 0;
}
