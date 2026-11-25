#ifndef SYSMOND_H
#define SYSMOND_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Basic header template for a sysmond module.
 */

#define SYSMOND_NAME "sysmond"

#include <stdbool.h>
#include <sysmon_ipc.h>

typedef enum {
    LIFE_MEM = 0,
    LIFE_CPU,
    LIFE_PROC,
    LIFE_COUNT
} SensorLifecycleType;

typedef struct {
    const char *name;

    pid_t pid;
    int read_fd;

    unsigned int restart_count;
    bool active;
} SensorLifecycle;

void signal_handler();
int print_system_info(MemData *mem_info, CpuData *cpu_info, ProcData *proc_info);
pid_t spawn_sensor(const char *path, const char *fifo_path, int *fd);
void anonymous_pipe(int fds1);
static int read_full(int fd, void *buffer, size_t size);
void *reader_thread(void *arg);
void reap_child(pid_t pid);
void cleanup(pthread_t threads[], SharedData *shared);
static void sensor_started(SensorLifecycleType type, pid_t pid, int read_fd);
static void sensor_stopped(SensorLifecycleType type);
//static void sensor_restarted(SensorLifecycleType type);

#ifdef __cplusplus
}
#endif

#endif /* SYSMOND_H */