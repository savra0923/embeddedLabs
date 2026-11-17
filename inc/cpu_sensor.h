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
#define INTERVAL     2

void *cpu_sensor_thread(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* CPU_SENSOR_H */