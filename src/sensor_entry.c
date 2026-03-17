/* * Program: Smart Alarm Clock - Sensor Module
 * Name: Yuanchen Zhang
 * Date: 2026-01-07
 * Description: Using IMU for fall detection and temperature monitoring.
 */
#include "hal_data.h"
#include "sensor.h"
#include <math.h>
#include "mpu6050.h"
volatile uint8_t g_display_temp = 0;//prevent optimization
/* sensor entry function */
void sensor_entry(void)
{
    float total_acc;//define a total acc variable to detect falling situation
    float ax,ay,az;
    uint8_t temp_update_counter = 0;
    mpu6050_Init();
    while (1)
    {
        tx_thread_sleep (1);//running interval 10ms
        mpu6050_Process();
        ax = (float)mpu6050_data.acc.x / 16384.0f;//transform parameters to physical value based on the datasheet P13
        ay = (float)mpu6050_data.acc.y / 16384.0f;
        az = (float)mpu6050_data.acc.z / 16384.0f;
        total_acc=sqrtf((ax*ax) + (ay*ay) + (az*az));
        if(total_acc>0.3)//falling situation judgement
        {
            uint32_t msg = 1;
            tx_queue_send(&g_acc_queue, &msg, TX_NO_WAIT);//using queue to transmit falling state
        }
        temp_update_counter++;
        if (temp_update_counter >= 20) // 10ms * 20 = 200ms
        {
            temp_update_counter = 0; // reset counter
            if(tx_mutex_get( &g_sensor_mutex, TX_WAIT_FOREVER)==TX_SUCCESS)//using mutex to transmit temperature information
            {
                g_display_temp = mpu6050_data.temperature;
                tx_mutex_put(&g_sensor_mutex);//release mutex
            }
        }
    }
}
