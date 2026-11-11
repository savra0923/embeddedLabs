#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
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

    if (mkdir("/tmp/sysmon", 0755) == -1 && errno != EEXIST) {
    perror("mkdir");
    return -1;
    }
    

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
        if (sscanf(line, "MemTotal: %zu", &info.memTotal) == 1)
            continue;

        if (sscanf(line, "MemFree: %zu", &info.memFree) == 1)
            continue;

        if (sscanf(line, "MemAvailable: %zu", &info.memAvailable) == 1)
            continue;

        if (sscanf(line, "SwapTotal: %zu", &info.SwapTotal) == 1)
            continue;

        if (sscanf(line, "SwapFree: %zu", &info.SwapFree) == 1)
            continue;
    }

    printf("MemTotal: %zu kB\n", info.memTotal);
    printf("MemFree: %zu kB\n", info.memFree);
    printf("MemAvailable: %zu kB\n", info.memAvailable);
    printf("SwapTotal: %zu kB\n", info.SwapTotal);
    printf("SwapFree: %zu kB\n", info.SwapFree);

    fclose(fp);

    FILE *fw = fopen("/tmp/sysmon/mem.dat", "w");

    if (access("/tmp/sysmon/mem.dat", F_OK) == 0) {
        printf("File exists. Overwriting...\n");
    } else {
        printf("File does not exist. Creating...\n");
    }

    if (fw == NULL) {
        perror("fopen mem.dat");
        return -1;
    }

    fprintf(fw, "MemTotal:     %zu kB\n", info.memTotal);
    fprintf(fw, "MemFree:      %zu kB\n", info.memFree);
    fprintf(fw, "MemAvailable: %zu kB\n", info.memAvailable);
    fprintf(fw, "SwapTotal:    %zu kB\n", info.SwapTotal);
    fprintf(fw, "SwapFree:     %zu kB\n", info.SwapFree);

    fclose(fw);

	return 0;
}
