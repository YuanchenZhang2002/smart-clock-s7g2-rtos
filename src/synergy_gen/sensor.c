/* generated thread source file - do not edit */
#include "sensor.h"

TX_THREAD sensor;
void sensor_create(void);
static void sensor_func(ULONG thread_input);
static uint8_t sensor_stack[1024] BSP_PLACE_IN_SECTION_V2(".stack.sensor") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init(void);
TX_MUTEX g_sensor_mutex;
TX_QUEUE g_acc_queue;
static uint8_t queue_memory_g_acc_queue[20];
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void sensor_create(void)
{
    /* Increment count so we will know the number of ISDE created threads. */
    g_ssp_common_thread_count++;

    /* Initialize each kernel object. */
    UINT err_g_sensor_mutex;
    err_g_sensor_mutex = tx_mutex_create (&g_sensor_mutex, (CHAR*) "Sensor Mutex", TX_INHERIT);
    if (TX_SUCCESS != err_g_sensor_mutex)
    {
        tx_startup_err_callback (&g_sensor_mutex, 0);
    }
    UINT err_g_acc_queue;
    err_g_acc_queue = tx_queue_create (&g_acc_queue, (CHAR*) "ACC Queue", 1, &queue_memory_g_acc_queue,
                                       sizeof(queue_memory_g_acc_queue));
    if (TX_SUCCESS != err_g_acc_queue)
    {
        tx_startup_err_callback (&g_acc_queue, 0);
    }

    UINT err;
    err = tx_thread_create (&sensor, (CHAR*) "sensor", sensor_func, (ULONG) NULL, &sensor_stack, 1024, 1, 1, 1,
                            TX_AUTO_START);
    if (TX_SUCCESS != err)
    {
        tx_startup_err_callback (&sensor, 0);
    }
}

static void sensor_func(ULONG thread_input)
{
    /* Not currently using thread_input. */
    SSP_PARAMETER_NOT_USED (thread_input);

    /* Initialize common components */
    tx_startup_common_init ();

    /* Initialize each module instance. */

    /* Enter user code for this thread. */
    sensor_entry ();
}
