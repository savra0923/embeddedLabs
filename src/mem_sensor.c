#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <stddef.h>
#include "mem_sensor.h"

int read_mem_info(mem_info_t *info) 
{
    FILE *fp = fopen(PROC_MEMINFO, "r");
    if (fp == NULL) 
    {
        perror("fopen /proc/meminfo");
        return -1;
    }

    char line[256];
    memset(info, 0, sizeof(mem_info_t));

    while (fgets(line, sizeof(line), fp) != NULL) 
    {
        if (sscanf(line, "MemTotal: %zu", &info->memTotal) == 1)
            continue;
        if (sscanf(line, "MemFree: %zu", &info->memFree) == 1)
            continue;
        if (sscanf(line, "MemAvailable: %zu", &info->memAvailable) == 1)
            continue;
        if (sscanf(line, "SwapTotal: %zu", &info->SwapTotal) == 1)
            continue;
        if (sscanf(line, "SwapFree: %zu", &info->SwapFree) == 1)
            continue;
    }

    fclose(fp);
    return 0;
}

int write_mem_info(const mem_info_t *info)
{
    FILE *fw = fopen(OUTPUT_FILE, "w");
    if (fw == NULL) 
    {
        perror("fopen mem.dat");
        return -1;
    }

    fprintf(fw, "MemTotal:     %zu kB\n", info->memTotal);
    fprintf(fw, "MemFree:      %zu kB\n", info->memFree);
    fprintf(fw, "MemAvailable: %zu kB\n", info->memAvailable);
    fprintf(fw, "SwapTotal:    %zu kB\n", info->SwapTotal);
    fprintf(fw, "SwapFree:     %zu kB\n", info->SwapFree);

    fclose(fw);
    return 0;
}

int main(void)
{
    mem_info_t info;

    while (1)
    {
        if (read_mem_info(&info) == 0) 
        {
            if (write_mem_info(&info) != 0) 
                fprintf(stderr, "Failed to write memory info\n");
        } 
        else 
            fprintf(stderr, "Failed to read memory info\n");
        sleep(INTERVAL);
    }
	
	return 0;
}
