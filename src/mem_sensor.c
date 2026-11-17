#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stddef.h>
#include "sysmond.h"
#include "mem_sensor.h"

void *mem_sensor_thread(void *arg)
{
    SystemState *state = (SystemState *)arg;

    while (1)
    {
        FILE *fp = fopen(PROC_MEMINFO, "r");

        if (fp == NULL)
        {
            perror("fopen /proc/meminfo");
            sleep(INTERVAL);
            continue;
        }

        char line[256];

        // Temporary local variables
        long mem_total = 0;
        long mem_free = 0;
        long mem_avail = 0;
        long swap_total = 0;
        long swap_free = 0;

        while (fgets(line, sizeof(line), fp) != NULL)
        {
            sscanf(line, "MemTotal: %ld", &mem_total);
            sscanf(line, "MemFree: %ld", &mem_free);
            sscanf(line, "MemAvailable: %ld", &mem_avail);
            sscanf(line, "SwapTotal: %ld", &swap_total);
            sscanf(line, "SwapFree: %ld", &swap_free);
        }

        fclose(fp);

        // Update shared state
        pthread_mutex_lock(&state->lock);

        state->mem_total = mem_total;
        state->mem_free = mem_free;
        state->mem_avail = mem_avail;
        state->swap_total = swap_total;
        state->swap_free = swap_free;

        pthread_mutex_unlock(&state->lock);

        //sleep(INTERVAL);
    }

    return NULL;
}