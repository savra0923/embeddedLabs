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

int print_system_info(MemData *mem_info, CpuData *cpu_info, ProcData *proc_info);

#ifdef __cplusplus
}
#endif

#endif /* SYSMOND_H */