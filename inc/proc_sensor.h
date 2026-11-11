#ifndef PROC_SENSOR_H
#define PROC_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Basic header template for a proc sensor module.
 */

#define PROC_SENSOR_NAME "proc_sensor"

#define PROC_INFO "/proc"
#define OUTPUT_DIR   "/tmp/sysmon"
#define OUTPUT_FILE  "/tmp/sysmon/proc.dat"


int read_pid_count(void);
int write_pid_count(int count);
#ifdef __cplusplus
}
#endif

#endif /* PROC_SENSOR_H */