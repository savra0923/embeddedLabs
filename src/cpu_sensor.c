#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <fcntl.h>
#include <sysmon_ipc.h>
#include "cpu_sensor.h"

int main(void)
{
    int fd = open(CPU_FIFO_PATH, O_WRONLY);

    if (fd == -1)
    {
        perror("open CPU FIFO");
    }

    while (1)
    {
        FILE *fp = fopen(PROC_CPUINFO, "r");

        if (fp == NULL)
        {
            perror("fopen loadavg");
            break;
        }

        CpuData data;

        if (fscanf(fp, "%lf %lf %lf",
                   &data.load1,
                   &data.load5,
                   &data.load15) != 3)
        {
            fclose(fp);
            continue;
        }

        fclose(fp);

        if (write(fd, &data, sizeof(data)) != sizeof(data))
        {
            perror("write CPU FIFO");
            break;
        }

        sleep(INTERVAL+10);
    }
}
