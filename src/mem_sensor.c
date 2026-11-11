#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "inc/mem_sensor.h"

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

int main(void)
{
    mem_info_t info;

	/* Write your code here. */
	//printf("Hello, world!\n");
    FILE *fp = fopen("/proc/meminfo", "r");

    if (fp == NULL) {
        perror("fopen /proc/meminfo");
        return -1;
    }

    char line[256];

    /* Initialize all values */
    memset(&info, 0, sizeof(info));

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line, "MemTotal: %lu", &info.memTotal) == 1)
            continue;

        if (sscanf(line, "MemFree: %lu", &info.memFree) == 1)
            continue;

        if (sscanf(line, "MemAvailable: %lu", &info.memAvailable) == 1)
            continue;

        if (sscanf(line, "SwapTotal: %lu", &info.SwapTotal) == 1)
            continue;

        if (sscanf(line, "SwapFree: %lu", &info.SwapFree) == 1)
            continue;
    }

    printf("MemTotal: %lu kB\n", info.memTotal);
    printf("MemFree: %lu kB\n", info.memFree);
    printf("MemAvailable: %lu kB\n", info.memAvailable);
    printf("SwapTotal: %lu kB\n", info.SwapTotal);
    printf("SwapFree: %lu kB\n", info.SwapFree);

    fclose(fp);

	return 0;
}
