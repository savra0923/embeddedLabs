#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stddef.h>
#include <pthread.h>
#include "sysmond.h"
#include "proc_sensor.h"

void *proc_sensor_thread(void *arg)
{
    SystemState *state = (SystemState *)arg;

    while (1)
    {
        DIR *proc_dir = opendir(PROC_INFO);
        if (proc_dir == NULL)
        {
            perror("opendir /proc");
            sleep(INTERVAL);
            continue;
        }

        int total_procs = 0;
        struct dirent *entry;

        while ((entry = readdir(proc_dir)) != NULL)
        {
            if (isdigit(entry->d_name[0]))
            {
                total_procs++;
            }
        }

        closedir(proc_dir);

        // Update shared state
        pthread_mutex_lock(&state->lock);
        state->total_procs = total_procs;
        pthread_mutex_unlock(&state->lock);

        //sleep(INTERVAL);
    }

    return NULL;
}
