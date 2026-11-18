#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <stddef.h>
#include <sysmon_ipc.h>
#include "mem_sensor.h"

int writer(FILE *stream, const MemData *info)
{
    if (fwrite(info, sizeof(MemData), 1, stream) != 1)
    {
        perror("fwrite");
        return -1;
    }

    if (fflush(stream) == EOF)
    {
        perror("fflush");
        return -1;
    }
}

int read_mem_info() 
{
    FILE *fp = fopen(PROC_MEMINFO, "r");
    if (fp == NULL) 
    {
        perror("fopen /proc/meminfo");
        return -1;
    }

    char line[256];
    memset(info, 0, sizeof(MemData));

    while (fgets(line, sizeof(line), fp) != NULL) 
    {
        if (sscanf(line, "MemTotal: %zu", &info->mem_total) == 1)
            continue;
        if (sscanf(line, "MemFree: %zu", &info->mem_free) == 1)
            continue;
        if (sscanf(line, "MemAvailable: %zu", &info->mem_avail) == 1)
            continue;
        if (sscanf(line, "SwapTotal: %zu", &info->swap_total) == 1)
            continue;
        if (sscanf(line, "SwapFree: %zu", &info->swap_free) == 1)
            continue;
    }

    fclose(fp);
    return 0;
}

int write_mem_info(const mem_info_t *info)
{
    FILE *fw = fopen(OUTPUT_MEM_FILE, "w");
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

int main(int argc, char *argv[])
{
    MemData info;

    while (1)
    {
        if (read_mem_info() == 0) 
        {
            if (writer(argv[1]) != 0) 
                fprintf(stderr, "Failed to write memory info\n");
        } 
        else 
            fprintf(stderr, "Failed to read memory info\n");
        sleep(INTERVAL);
    }
	
	return 0;
}
