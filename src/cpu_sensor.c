#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <pthread.h>
#include "sysmond.h"
#include "cpu_sensor.h"



void *cpu_sensor_thread(void *arg)
{
    SystemState *state = (SystemState *)arg;

    while (1)
    {
        FILE *fp = fopen(PROC_CPUINFO, "r");

        if (fp == NULL)
        {
            perror("fopen /proc/loadavg");
            sleep(INTERVAL);
            continue;
        }

        double load1 = 0.0;
        double load5 = 0.0;
        double load15 = 0.0;

        fscanf(fp, "%lf %lf %lf", &load1, &load5, &load15);

        fclose(fp);

        // Update shared state
        pthread_mutex_lock(&state->lock);

        state->load1 = load1;
        state->load5 = load5;
        state->load15 = load15;

        if (state->load1 > 1.50)
            pthread_cond_signal(&state->alert_cond);
        

        pthread_mutex_unlock(&state->lock);

        //sleep(INTERVAL);
    }
	
	return 0;
}
