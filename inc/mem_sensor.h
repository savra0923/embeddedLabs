#ifndef MEM_SENSOR_H
#define MEM_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Basic header template for a memory sensor module.
 */

#define MEM_SENSOR_NAME "mem_sensor"

#define PROC_MEMINFO "/proc/meminfo"
#define OUTPUT_DIR   "/tmp/sysmon"
#define INTERVAL     2

void *mem_sensor_thread(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* MEM_SENSOR_H */
