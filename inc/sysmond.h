#ifndef SYSMOND_H
#define SYSMOND_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Basic header template for a sysmond module.
 */

#define SYSMOND_NAME "sysmond"

#include <sysmon_ipc.h>

void signal_handler(int signum);
int print_system_info(MemData *mem_info, CpuData *cpu_info, ProcData *proc_info);
pid_t spawn_sensor(const char *path, const char *fifo_path, int *fd);
void anonymous_pipe(int fds1);
static int read_full(int fd, void *buffer, size_t size);
void *reader_thread(void *arg);
void reap_child(pid_t pid);
void cleanup(pid_t mem_pid, pid_t cpu_pid, pid_t proc_pid, pthread_t threads[], SharedData *shared);

#ifdef __cplusplus
}
#endif

#endif /* SYSMOND_H */