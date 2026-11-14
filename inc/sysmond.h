#ifndef SYSMOND_H
#define SYSMOND_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Basic header template for a sysmond module.
 */

#define SYSMOND_NAME "sysmond"

#define MEM_FILE "/tmp/sysmon/mem.dat"
#define CPU_FILE "/tmp/sysmon/cpu.dat"
#define PROC_FILE "/tmp/sysmon/proc.dat"
#define READ_INTERVAL 3
#define PRINT_INTERVAL 5

#include "mem_sensor.h"
#include "cpu_sensor.h"
#include "proc_sensor.h"

cpu_info_t cpu_info;
mem_info_t mem_info;


int make_output_dir(void);
int read_mem_data(const char *file_path, mem_info_t *info);
int read_cpu_data(const char *file_path, cpu_info_t *info);
int read_proc_data(const char *file_path, int *pid_count);
int print_system_info(mem_info_t *mem_info, cpu_info_t *cpu_info, int *pid_count);

#ifdef __cplusplus
}
#endif

#endif /* SYSMOND_H */