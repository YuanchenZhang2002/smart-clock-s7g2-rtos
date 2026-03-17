/* generated HAL source file - do not edit */
#include "hal_data.h"
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer4) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_SCI3_RXI)
#define DTC_ACTIVATION_SRC_ELC_EVENT_SCI3_RXI
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_0) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_0);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0
#endif
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_1) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_1);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1
#endif
#endif
#endif

dtc_instance_ctrl_t g_transfer4_ctrl;
transfer_info_t g_transfer4_info =
{ .dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
  .irq = TRANSFER_IRQ_END,
  .chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
  .src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .size = TRANSFER_SIZE_1_BYTE,
  .mode = TRANSFER_MODE_NORMAL,
  .p_dest = (void*) NULL,
  .p_src = (void const*) NULL,
  .num_blocks = 0,
  .length = 0, };
const transfer_cfg_t g_transfer4_cfg =
{ .p_info = &g_transfer4_info,
  .activation_source = ELC_EVENT_SCI3_RXI,
  .auto_enable = false,
  .p_callback = NULL,
  .p_context = &g_transfer4,
  .irq_ipl = (BSP_IRQ_DISABLED) };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer4 =
{ .p_ctrl = &g_transfer4_ctrl, .p_cfg = &g_transfer4_cfg, .p_api = &g_transfer_on_dtc };
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer3) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_SCI3_TXI)
#define DTC_ACTIVATION_SRC_ELC_EVENT_SCI3_TXI
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_0) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_0);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0
#endif
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_1) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_1);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1
#endif
#endif
#endif

dtc_instance_ctrl_t g_transfer3_ctrl;
transfer_info_t g_transfer3_info =
{ .dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
  .irq = TRANSFER_IRQ_END,
  .chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
  .src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .size = TRANSFER_SIZE_1_BYTE,
  .mode = TRANSFER_MODE_NORMAL,
  .p_dest = (void*) NULL,
  .p_src = (void const*) NULL,
  .num_blocks = 0,
  .length = 0, };
const transfer_cfg_t g_transfer3_cfg =
{ .p_info = &g_transfer3_info,
  .activation_source = ELC_EVENT_SCI3_TXI,
  .auto_enable = false,
  .p_callback = NULL,
  .p_context = &g_transfer3,
  .irq_ipl = (BSP_IRQ_DISABLED) };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer3 =
{ .p_ctrl = &g_transfer3_ctrl, .p_cfg = &g_transfer3_cfg, .p_api = &g_transfer_on_dtc };
#if !defined(SSP_SUPPRESS_ISR_g_i2c3) && !defined(SSP_SUPPRESS_ISR_SCI3)
SSP_VECTOR_DEFINE_CHAN(sci_i2c_rxi_isr, SCI, RXI, 3);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_i2c3) && !defined(SSP_SUPPRESS_ISR_SCI3)
SSP_VECTOR_DEFINE_CHAN(sci_i2c_txi_isr, SCI, TXI, 3);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_i2c3) && !defined(SSP_SUPPRESS_ISR_SCI3)
SSP_VECTOR_DEFINE_CHAN(sci_i2c_tei_isr, SCI, TEI, 3);
#endif
sci_i2c_instance_ctrl_t g_i2c3_ctrl;

/** I2C extended configuration */
const sci_i2c_extended_cfg g_i2c3_cfg_extend =
{ .bitrate_modulation = true };

const i2c_cfg_t g_i2c3_cfg =
{ .channel = 3,
  .rate = I2C_RATE_STANDARD,
  .slave = 0x68,
  .addr_mode = I2C_ADDR_MODE_7BIT,
  .sda_delay = 30,
#define SYNERGY_NOT_DEFINED (1)            
#if (SYNERGY_NOT_DEFINED == g_transfer3)
    .p_transfer_tx       = NULL,
#else
  .p_transfer_tx = &g_transfer3,
#endif
#if (SYNERGY_NOT_DEFINED == g_transfer4)
    .p_transfer_rx       = NULL,
#else
  .p_transfer_rx = &g_transfer4,
#endif
#undef SYNERGY_NOT_DEFINED
  .p_callback = NULL,
  .p_context = (void*) &g_i2c3,
  .rxi_ipl = (12),
  .txi_ipl = (12),
  .tei_ipl = (12),
  .eri_ipl = BSP_IRQ_DISABLED,
  .p_extend = &g_i2c3_cfg_extend, };
/* Instance structure to use this module. */
const i2c_master_instance_t g_i2c3 =
{ .p_ctrl = &g_i2c3_ctrl, .p_cfg = &g_i2c3_cfg, .p_api = &g_i2c_master_on_sci };
void g_hal_init(void)
{
    g_common_init ();
}
