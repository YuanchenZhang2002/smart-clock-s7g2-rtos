/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_dtc.h"
#include "r_transfer_api.h"
#include "r_sci_i2c.h"
#include "r_i2c_api.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer4;
#ifndef NULL
void NULL(transfer_callback_args_t *p_args);
#endif
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer3;
#ifndef NULL
void NULL(transfer_callback_args_t *p_args);
#endif
extern const i2c_cfg_t g_i2c3_cfg;
/** I2C on SCI Instance. */
extern const i2c_master_instance_t g_i2c3;
#ifndef NULL
void NULL(i2c_callback_args_t *p_args);
#endif

extern const sci_i2c_extended_cfg g_i2c3_cfg_extend;
extern sci_i2c_instance_ctrl_t g_i2c3_ctrl;
#define SYNERGY_NOT_DEFINED (1)            
#if (SYNERGY_NOT_DEFINED == g_transfer3)
    #define g_i2c3_P_TRANSFER_TX (NULL)
#else
#define g_i2c3_P_TRANSFER_TX (&g_transfer3)
#endif
#if (SYNERGY_NOT_DEFINED == g_transfer4)
    #define g_i2c3_P_TRANSFER_RX (NULL)
#else
#define g_i2c3_P_TRANSFER_RX (&g_transfer4)
#endif
#undef SYNERGY_NOT_DEFINED
#define g_i2c3_P_EXTEND (&g_i2c3_cfg_extend)
void hal_entry(void);
void g_hal_init(void);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* HAL_DATA_H_ */
