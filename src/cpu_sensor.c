#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <stddef.h>
#include "cpu_sensor.h"

int read_cpu_info(cpu_info_t *info) 
{
    FILE *fp = fopen(PROC_CPUINFO, "r");
    if (fp == NULL) 
    {
        perror("fopen /proc/loadavg");
        return -1;
    }

    char line[256];
    memset(info, 0, sizeof(cpu_info_t));

    while (fgets(line, sizeof(line), fp) != NULL) 
    {
        if (sscanf(line, "%lf %lf %lf", &info->cpu1min, &info->cpu5min, &info->cpu15min) == 3)
        continue; 
    }

    fclose(fp);
    return 0;
}

int write_cpu_info(const cpu_info_t *info)
{
    FILE *fw = fopen(OUTPUT_FILE, "w");
    if (fw == NULL) 
    {
        perror("fopen cpu.dat");
        return -1;
    }

    fprintf(fw, "1 min:  %.2f\n", info->cpu1min);
    fprintf(fw, "5 min:  %.2f\n", info->cpu5min);
    fprintf(fw, "15 min: %.2f\n", info->cpu15min);

    fclose(fw);
    return 0;
}

int main(void)
{
    cpu_info_t info;

    while (1)
    {
        if (read_cpu_info(&info) == 0) 
        {
            if (write_cpu_info(&info) != 0) 
                fprintf(stderr, "Failed to write CPU info\n");
        } 
        else 
            fprintf(stderr, "Failed to read CPU info\n");
        sleep(INTERVAL);
    }
	
	return 0;
}
