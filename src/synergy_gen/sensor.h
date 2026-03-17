/* generated thread header file - do not edit */
#ifndef SENSOR_H_
#define SENSOR_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void sensor_entry(void);
                #else
extern void sensor_entry(void);
#endif
#ifdef __cplusplus
extern "C" {
#endif
extern TX_MUTEX g_sensor_mutex;
extern TX_QUEUE g_acc_queue;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* SENSOR_H_ */
