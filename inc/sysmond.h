#ifndef SYSMOND_H
#define SYSMOND_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Basic header template for a sysmond module.
 */

#define SYSMOND_NAME "sysmond"

#define PRINT_INTERVAL 2

extern int shutdown_requested;

typedef struct 
{
    // Memory Metrics
    long mem_total;
    long mem_free;
    long mem_avail;
    long swap_total;
    long swap_free;

    // CPU Load Metrics
    double load1;
    double load5;
    double load15;

    
    // Process Metrics
    int total_procs;

    // Synchronization
    pthread_mutex_t lock;
    pthread_cond_t alert_cond;
} SystemState;

int make_output_dir(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSMOND_H */