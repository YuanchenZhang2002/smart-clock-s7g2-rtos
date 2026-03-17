/* generated common header file - do not edit */
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_
#include <stdint.h>
#include "bsp_api.h"

#include "r_jpeg_decode.h"
#include "sf_jpeg_decode.h"
#include "r_glcd.h"
#include "r_display_api.h"
#include "sf_el_gx.h"
#include "r_dtc.h"
#include "r_transfer_api.h"
#include "r_riic.h"
#include "r_i2c_api.h"
#include "r_i2c_api.h"
#include "sf_i2c.h"
#include "sf_i2c_api.h"
#include "r_ioport.h"
#include "r_ioport_api.h"
#include "r_cgc.h"
#include "r_cgc_api.h"
#include "r_elc.h"
#include "r_elc_api.h"
#include "r_fmi.h"
#include "r_fmi_api.h"
#ifdef __cplusplus
extern "C" {
#endif

extern const jpeg_decode_instance_t g_jpeg_decode0;
extern const jpeg_decode_cfg_t g_jpeg_decode0_cfg;
#ifndef NULL
void NULL(jpeg_decode_callback_args_t *p_args);
#endif
extern const sf_jpeg_decode_instance_t g_sf_jpeg_decode0;
/* Display on GLCD Instance. */
extern const display_instance_t g_display;
extern display_runtime_cfg_t g_display_runtime_cfg_fg;
extern display_runtime_cfg_t g_display_runtime_cfg_bg;
#if (false)
            extern display_clut_cfg_t g_display_clut_cfg_glcd;
            #endif
#ifndef NULL
void NULL(display_callback_args_t *p_args);
#endif
#if (true)
            #define DISPLAY_IN_FORMAT_16BITS_RGB565_0
            #if defined (DISPLAY_IN_FORMAT_32BITS_RGB888_0) || defined (DISPLAY_IN_FORMAT_32BITS_ARGB8888_0)
            #define DISPLAY_BITS_PER_PIXEL_INPUT0 (32)
            #elif defined (DISPLAY_IN_FORMAT_16BITS_RGB565_0) || defined (DISPLAY_IN_FORMAT_16BITS_ARGB1555_0) || defined (DISPLAY_IN_FORMAT_16BITS_ARGB4444_0)
            #define DISPLAY_BITS_PER_PIXEL_INPUT0 (16)
            #elif defined (DISPLAY_IN_FORMAT_CLUT8_0)
            #define DISPLAY_BITS_PER_PIXEL_INPUT0 (8)
            #elif defined (DISPLAY_IN_FORMAT_CLUT4_0)
            #define DISPLAY_BITS_PER_PIXEL_INPUT0 (4)
            #else
            #define DISPLAY_BITS_PER_PIXEL_INPUT0 (1)
            #endif
            extern uint8_t g_display_fb_background[2][((256 * 320) * DISPLAY_BITS_PER_PIXEL_INPUT0) >> 3];
            #endif
#if (false)
            #define DISPLAY_IN_FORMAT_16BITS_RGB565_1
            #if defined (DISPLAY_IN_FORMAT_32BITS_RGB888_1) || defined (DISPLAY_IN_FORMAT_32BITS_ARGB8888_1)
            #define DISPLAY_BITS_PER_PIXEL_INPUT1 (32)
            #elif defined (DISPLAY_IN_FORMAT_16BITS_RGB565_1) || defined (DISPLAY_IN_FORMAT_16BITS_ARGB1555_1) || defined (DISPLAY_IN_FORMAT_16BITS_ARGB4444_1)
            #define DISPLAY_BITS_PER_PIXEL_INPUT1 (16)
            #elif defined (DISPLAY_IN_FORMAT_CLUT8_1)
            #define DISPLAY_BITS_PER_PIXEL_INPUT1 (8)
            #elif defined (DISPLAY_IN_FORMAT_CLUT4_1)
            #define DISPLAY_BITS_PER_PIXEL_INPUT1 (4)
            #else
            #define DISPLAY_BITS_PER_PIXEL_INPUT1 (1)
            #endif
            extern uint8_t g_display_fb_foreground[2][((800 * 480) * DISPLAY_BITS_PER_PIXEL_INPUT1) >> 3];
            #endif
#ifndef NULL
extern void NULL(sf_el_gx_callback_args_t *p_arg);
#endif
extern sf_el_gx_instance_t g_sf_el_gx;
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer5;
#ifndef NULL
void NULL(transfer_callback_args_t *p_args);
#endif
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer0;
#ifndef NULL
void NULL(transfer_callback_args_t *p_args);
#endif
extern const i2c_cfg_t g_i2c_cfg;
/** I2C on RIIC Instance. */
extern const i2c_master_instance_t g_i2c;
#ifndef NULL
void NULL(i2c_callback_args_t *p_args);
#endif

extern riic_instance_ctrl_t g_i2c_ctrl;
extern const riic_extended_cfg g_i2c_extend;
#define SYNERGY_NOT_DEFINED (1)            
#if (SYNERGY_NOT_DEFINED == g_transfer0)
    #define g_i2c_P_TRANSFER_TX (NULL)
#else
#define g_i2c_P_TRANSFER_TX (&g_transfer0)
#endif
#if (SYNERGY_NOT_DEFINED == g_transfer5)
    #define g_i2c_P_TRANSFER_RX (NULL)
#else
#define g_i2c_P_TRANSFER_RX (&g_transfer5)
#endif
#undef SYNERGY_NOT_DEFINED
#define g_i2c_P_EXTEND (&g_i2c_extend)
extern sf_i2c_bus_t g_sf_i2c_bus0;
extern i2c_api_master_t const g_i2c_master_on_riic;

#define g_sf_i2c_bus0_CHANNEL        (2)
#define g_sf_i2c_bus0_RATE           (I2C_RATE_FAST)
#define g_sf_i2c_bus0_SLAVE          (0)
#define g_sf_i2c_bus0_ADDR_MODE      (I2C_ADDR_MODE_7BIT)          
#define g_sf_i2c_bus0_SDA_DELAY      (0)  
#define g_sf_i2c_bus0_P_CALLBACK     (NULL)
#define g_sf_i2c_bus0_P_CONTEXT      (&g_i2c)
#define g_sf_i2c_bus0_RXI_IPL        ((3))
#define g_sf_i2c_bus0_TXI_IPL        ((3))
#define g_sf_i2c_bus0_TEI_IPL        ((3))            
#define g_sf_i2c_bus0_ERI_IPL        ((3))

/** These are obtained by macros in the I2C driver XMLs. */
#define g_sf_i2c_bus0_P_TRANSFER_TX  (g_i2c_P_TRANSFER_TX)
#define g_sf_i2c_bus0_P_TRANSFER_RX  (g_i2c_P_TRANSFER_RX)            
#define g_sf_i2c_bus0_P_EXTEND       (g_i2c_P_EXTEND)
/** IOPORT Instance */
extern const ioport_instance_t g_ioport;
/** CGC Instance */
extern const cgc_instance_t g_cgc;
/** ELC Instance */
extern const elc_instance_t g_elc;
/** FMI on FMI Instance. */
extern const fmi_instance_t g_fmi;
void g_common_init(void);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* COMMON_DATA_H_ */
