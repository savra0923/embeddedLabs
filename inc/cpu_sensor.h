#ifndef CPU_SENSOR_H
#define CPU_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Basic header template for a CPU sensor module.
 */

#define CPU_SENSOR_NAME "cpu_sensor"

#define PROC_CPUINFO "/proc/loadavg"
#define OUTPUT_DIR   "/tmp/sysmon"
#define OUTPUT_FILE  "/tmp/sysmon/cpu.dat"
#define INTERVAL     5

typedef struct {
    double cpu1min;
    double cpu5min;
    double cpu15min;
} cpu_info_t;

int read_cpu_info(cpu_info_t *info);
int write_cpu_info(const cpu_info_t *info);
#ifdef __cplusplus
}
#endif

#endif /* CPU_SENSOR_H */