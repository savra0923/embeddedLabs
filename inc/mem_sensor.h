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
#define OUTPUT_FILE  "/tmp/sysmon/mem.dat"
#define INTERVAL     5

typedef struct {
    size_t memTotal;
    size_t memFree;
    size_t memAvailable;
    size_t SwapTotal;
    size_t SwapFree;
} mem_info_t;

int read_mem_info(mem_info_t *info);
int write_mem_info(const mem_info_t *info);

#ifdef __cplusplus
}
#endif

#endif /* MEM_SENSOR_H */
