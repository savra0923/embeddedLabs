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
    
#ifdef __cplusplus
}
#endif

#endif /* SYSMOND_H */