#ifndef MEM_SENSOR_H
#define MEM_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Basic header template for a memory sensor module.
 */

#define MEM_SENSOR_NAME "mem_sensor"

typedef struct {
    int id;
    float value;
} mem_sensor_data_t;

void mem_sensor_init(void);
void mem_sensor_update(void);
float mem_sensor_read(void);

#ifdef __cplusplus
}
#endif

#endif /* MEM_SENSOR_H */
