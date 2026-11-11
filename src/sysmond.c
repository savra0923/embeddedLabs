#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <stddef.h>
#include "mem_sensor.h"
#include "cpu_sensor.h"
#include "proc_sensor.h"

#define MEM_FILE "/tmp/sysmon/mem.dat"
#define CPU_FILE "/tmp/sysmon/cpu.dat"
#define PROC_FILE "/tmp/sysmon/proc.dat"

int main(void)
{
    if (mkdir("/tmp/sysmon", 0755) == -1 && errno != EEXIST)
    {
        perror("mkdir");
        return -1;
    }

    
	
	return 0;
}
