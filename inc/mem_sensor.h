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

MemData *info;
int writer(FILE *stream, MemData *info);

int read_mem_info(MemData *info);

#ifdef __cplusplus
}
#endif

#endif /* MEM_SENSOR_H */
