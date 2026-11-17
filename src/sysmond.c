#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <stddef.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include "sysmond.h"
#include "mem_sensor.h"
#include "cpu_sensor.h"
#include "proc_sensor.h"

int shutdown_requested = 0;

void handle_sigint(int sig)
{
    shutdown_requested = 1;
}

void *alert_thread_func(void *arg)
{
    SystemState *state = (SystemState *)arg;

    while (1)
    {
        pthread_mutex_lock(&state->lock);

        pthread_cond_wait(&state->alert_cond, &state->lock);

        if (shutdown_requested)
        {
            pthread_mutex_unlock(&state->lock);
            break;
        }

        fprintf(stderr, "URGENT: High CPU load detected!\n");

        pthread_mutex_unlock(&state->lock);
    }

    return NULL;
}

int main(void)
{
    signal(SIGINT, handle_sigint);

    SystemState state;

    pthread_t mem_thread;
    pthread_t cpu_thread;
    pthread_t proc_thread;
    pthread_t alert_thread;

    pthread_mutex_init(&state.lock, NULL);
    pthread_cond_init(&state.alert_cond, NULL);

    pthread_create(&mem_thread, NULL, mem_sensor_thread, &state);
    pthread_create(&cpu_thread, NULL, cpu_sensor_thread, &state);
    pthread_create(&proc_thread, NULL, proc_sensor_thread, &state);
    pthread_create(&alert_thread, NULL, alert_thread_func, &state);

    while (!shutdown_requested)
    {
        pthread_mutex_lock(&state.lock);

        printf("\nSystem Metrics:\n");
        printf("Total memory: %ld kB\n", state.mem_total);
        printf("Free memory:  %ld kB\n", state.mem_free);
        printf("Available:    %ld kB\n", state.mem_avail);
        printf("Swap total:   %ld kB\n", state.swap_total);
        printf("Swap free:    %ld kB\n", state.swap_free);
        printf("CPU Load 1m:  %.2f\n", state.load1);
        printf("CPU Load 5m:  %.2f\n", state.load5);
        printf("CPU Load 15m: %.2f\n", state.load15);
        printf("Total processes: %d\n", state.total_procs);
        printf("-----------------------------\n");

        pthread_mutex_unlock(&state.lock);

        sleep(PRINT_INTERVAL);
    }

    printf("\nShutting down...\n");

    pthread_mutex_lock(&state.lock);
    pthread_cond_broadcast(&state.alert_cond);
    pthread_mutex_unlock(&state.lock);

    pthread_join(mem_thread, NULL);
    pthread_join(cpu_thread, NULL);
    pthread_join(proc_thread, NULL);
    pthread_join(alert_thread, NULL);

	return 0;
}
