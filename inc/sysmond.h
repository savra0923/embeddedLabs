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
#define READ_INTERVAL 6
#define Write_INTERVAL 8

#include "mem_sensor.h"
#include "cpu_sensor.h"
#include "proc_sensor.h"

cpu_info_t cpu_info;
mem_info_t mem_info;

#ifdef __cplusplus
}
#endif

#endif /* SYSMOND_H */