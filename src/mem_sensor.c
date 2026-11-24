#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <stddef.h>
#include <stdlib.h>
#include <sysmon_ipc.h>
#include "mem_sensor.h"

int writer(FILE *stream, MemData *info)
{
    char buffer[1024];

    while (1) {
        if (read_mem_info(info) == 0) {
            snprintf(buffer, sizeof(buffer),
                    "%ld %ld %ld %ld %ld\n",
                    info->mem_total,
                    info->mem_free,
                    info->mem_avail,
                    info->swap_total,
                    info->swap_free);

            fputs(buffer, stream);
            fflush(stream);
        }

        sleep(INTERVAL);
    }
    return 0;
}

int read_mem_info(MemData *info)
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
        if (sscanf(line, "MemTotal: %lu", &info->mem_total) == 1)
            continue;
        if (sscanf(line, "MemFree: %lu", &info->mem_free) == 1)
            continue;
        if (sscanf(line, "MemAvailable: %lu", &info->mem_avail) == 1)
            continue;
        if (sscanf(line, "SwapTotal: %lu", &info->swap_total) == 1)
            continue;
        if (sscanf(line, "SwapFree: %lu", &info->swap_free) == 1)
            continue;
    }

    fclose(fp);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        return 1;

    int fd = atoi(argv[1]);

    FILE *stream = fdopen(fd, "w");

    if (stream == NULL)
    {
        perror("fdopen");
        close(fd);
        return 1;
    }

    MemData info;

    while (1)
    {
        if (read_mem_info(&info) == 0)
        {
            if (writer(stream, &info) != 0)
                break;
        }

        sleep(INTERVAL);
    }

    fclose(stream);
    return 0;
}
