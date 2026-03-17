/* generated common source file - do not edit */
#include "common_data.h"
#if SYNERGY_NOT_DEFINED != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_DRW)
SSP_VECTOR_DEFINE(drw_int_isr, DRW, INT);
#endif
#endif
SSP_VECTOR_DEFINE( jpeg_jdti_isr, JPEG, JDTI);
SSP_VECTOR_DEFINE( jpeg_jedi_isr, JPEG, JEDI);
static jpeg_decode_instance_ctrl_t g_jpeg_decode0_ctrl;
const jpeg_decode_cfg_t g_jpeg_decode0_cfg =
{ .input_data_format = JPEG_DECODE_DATA_FORMAT_NORMAL,
  .output_data_format = JPEG_DECODE_DATA_FORMAT_NORMAL,
  .pixel_format = JPEG_DECODE_PIXEL_FORMAT_RGB565,
  .alpha_value = 255,
  .p_callback = NULL,
  .jedi_ipl = (3),
  .jdti_ipl = (3), };
const jpeg_decode_instance_t g_jpeg_decode0 =
{ .p_api = (jpeg_decode_api_t const*) &g_jpeg_decode_on_jpeg_decode, .p_ctrl = &g_jpeg_decode0_ctrl, .p_cfg =
          &g_jpeg_decode0_cfg };
static sf_jpeg_decode_instance_ctrl_t g_sf_jpeg_decode0_ctrl;

static const sf_jpeg_decode_cfg_t g_sf_jpeg_decode0_cfg =
{ .p_lower_lvl_jpeg_decode = (jpeg_decode_instance_t const*) &g_jpeg_decode0 };
const sf_jpeg_decode_instance_t g_sf_jpeg_decode0 =
{ .p_api = &g_sf_jpeg_decode_on_sf_jpeg_decode, .p_ctrl = &g_sf_jpeg_decode0_ctrl, .p_cfg =
          (sf_jpeg_decode_cfg_t const*) &g_sf_jpeg_decode0_cfg };
#if (3) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_display) && !defined(SSP_SUPPRESS_ISR_GLCD)
SSP_VECTOR_DEFINE( glcdc_line_detect_isr, GLCDC, LINE_DETECT);
#endif
#endif
#if (3) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_display) && !defined(SSP_SUPPRESS_ISR_GLCD)
SSP_VECTOR_DEFINE( glcdc_underflow_1_isr, GLCDC, UNDERFLOW_1);
#endif
#endif
#if (3) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_display) && !defined(SSP_SUPPRESS_ISR_GLCD)
SSP_VECTOR_DEFINE( glcdc_underflow_2_isr, GLCDC, UNDERFLOW_2);
#endif
#endif

/** Display frame buffer */
#if (true)
        uint8_t g_display_fb_background[2][((256 * 320) * DISPLAY_BITS_PER_PIXEL_INPUT0) >> 3] BSP_ALIGN_VARIABLE_V2(64) BSP_PLACE_IN_SECTION_V2(".bss");
        #else
/** Graphics screen1 is specified not to be used when starting */
#endif
#if (false)
        uint8_t g_display_fb_foreground[2][((800 * 480) * DISPLAY_BITS_PER_PIXEL_INPUT1) >> 3] BSP_ALIGN_VARIABLE_V2(64) BSP_PLACE_IN_SECTION_V2(".sdram");
        #else
/** Graphics screen2 is specified not to be used when starting */
#endif

#if (false)
        /** Display CLUT buffer to be used for updating CLUT */
        static uint32_t CLUT_buffer[256];

        /** Display CLUT configuration(only used if using CLUT format) */
        display_clut_cfg_t g_display_clut_cfg_glcd =
        {
            .p_base              = (uint32_t *)CLUT_buffer,
            .start               = 0,   /* User have to update this setting when using */
            .size                = 256  /* User have to update this setting when using */
        };
        #else
/** CLUT is specified not to be used */
#endif

#if (false | false | false)
        /** Display interface configuration */
        static const display_gamma_correction_t g_display_gamma_cfg =
        {
            .r =
            {
                .enable      = false,
                .gain        = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                .threshold   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
            },
            .g =
            {
                .enable      = false,
                .gain        = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                .threshold   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
            },
            .b =
            {
                .enable      = false,
                .gain        = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                .threshold   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
            }
        };
        #endif

/** Display device extended configuration */
static const glcd_cfg_t g_display_extend_cfg =
{ .tcon_hsync = GLCD_TCON_PIN_2,
  .tcon_vsync = GLCD_TCON_PIN_1,
  .tcon_de = GLCD_TCON_PIN_0,
  .correction_proc_order = GLCD_CORRECTION_PROC_ORDER_BRIGHTNESS_CONTRAST2GAMMA,
  .clksrc = GLCD_CLK_SRC_INTERNAL,
  .clock_div_ratio = GLCD_PANEL_CLK_DIVISOR_32,
  .dithering_mode = GLCD_DITHERING_MODE_TRUNCATE,
  .dithering_pattern_A = GLCD_DITHERING_PATTERN_11,
  .dithering_pattern_B = GLCD_DITHERING_PATTERN_11,
  .dithering_pattern_C = GLCD_DITHERING_PATTERN_11,
  .dithering_pattern_D = GLCD_DITHERING_PATTERN_11 };

/** Display control block instance */
glcd_instance_ctrl_t g_display_ctrl;

/** Display interface configuration */
const display_cfg_t g_display_cfg =
        {
        /** Input1(Graphics1 screen) configuration */
        .input[0
                  ] =
                  {
#if (true)
                .p_base              = (uint32_t *)&g_display_fb_background[0],
                #else
                    .p_base = NULL,
#endif
                    .hsize = 256,
                    .vsize = 320, .hstride = 256, .format = DISPLAY_IN_FORMAT_16BITS_RGB565, .line_descending_enable =
                            false,
                    .lines_repeat_enable = false, .lines_repeat_times = 0 },

          /** Input2(Graphics2 screen) configuration */
          .input[1
                  ] =
                  {
#if (false)
                .p_base              = (uint32_t *)&g_display_fb_foreground[0],
                #else
                    .p_base = NULL,
#endif
                    .hsize = 800,
                    .vsize = 480, .hstride = 800, .format = DISPLAY_IN_FORMAT_16BITS_RGB565, .line_descending_enable =
                            false,
                    .lines_repeat_enable = false, .lines_repeat_times = 0 },

          /** Input1(Graphics1 screen) layer configuration */
          .layer[0] =
          { .coordinate =
          { .x = 0, .y = 0 },
            .bg_color =
            { .byte =
            { .a = 255, .r = 255, .g = 255, .b = 255 } },
            .fade_control = DISPLAY_FADE_CONTROL_NONE, .fade_speed = 0 },

          /** Input2(Graphics2 screen) layer configuration */
          .layer[1] =
          { .coordinate =
          { .x = 0, .y = 0 },
            .bg_color =
            { .byte =
            { .a = 255, .r = 255, .g = 255, .b = 255 } },
            .fade_control = DISPLAY_FADE_CONTROL_NONE, .fade_speed = 0 },

          /** Output configuration */
          .output =
                  { .htiming =
                  { .total_cyc = 320, .display_cyc = 240, .back_porch = 6, .sync_width = 4, .sync_polarity =
                            DISPLAY_SIGNAL_POLARITY_LOACTIVE },
                    .vtiming =
                    { .total_cyc = 328, .display_cyc = 320, .back_porch = 4, .sync_width = 4, .sync_polarity =
                              DISPLAY_SIGNAL_POLARITY_LOACTIVE },
                    .format = DISPLAY_OUT_FORMAT_16BITS_RGB565, .endian = DISPLAY_ENDIAN_LITTLE, .color_order =
                            DISPLAY_COLOR_ORDER_RGB,
                    .data_enable_polarity = DISPLAY_SIGNAL_POLARITY_HIACTIVE, .sync_edge =
                            DISPLAY_SIGNAL_SYNC_EDGE_RISING,
                    .bg_color =
                    { .byte =
                    { .a = 255, .r = 0, .g = 0, .b = 0 } },
                    .brightness =
                    { .enable = false, .r = 512, .g = 512, .b = 512 },
                    .contrast =
                    { .enable = false, .r = 128, .g = 128, .b = 128 },
#if (false | false | false)
                .p_gamma_correction  = (display_gamma_correction_t *)(&g_display_gamma_cfg),
#else
                    .p_gamma_correction = NULL,
#endif
                    .dithering_on = false },

          /** Display device callback function pointer */
          .p_callback = NULL,
          .p_context = (void*) &g_display,

          /** Display device extended configuration */
          .p_extend = (void*) (&g_display_extend_cfg),

          .line_detect_ipl = (3),
          .underflow_1_ipl = (3), .underflow_2_ipl = (3), };

#if (true)
        /** Display on GLCD run-time configuration(for the graphics1 screen) */
        display_runtime_cfg_t g_display_runtime_cfg_bg =
        {
            .input =
            {
                #if (true)
                .p_base              = (uint32_t *)&g_display_fb_background[0],
                #else
                .p_base              = NULL,
                #endif
                .hsize               = 256,
                .vsize               = 320,
                .hstride             = 256,
                .format              = DISPLAY_IN_FORMAT_16BITS_RGB565,
                .line_descending_enable = false,
                .lines_repeat_enable = false,
                .lines_repeat_times  = 0
            },
            .layer =
            {
                .coordinate = {
                        .x           = 0,
                        .y           = 0
                },
                .bg_color            =
                {
                    .byte            =
                    {
                        .a           = 255,
                        .r           = 255,
                        .g           = 255,
                        .b           = 255
                    }
                },
                .fade_control        = DISPLAY_FADE_CONTROL_NONE,
                .fade_speed          = 0
            }
        };
#endif
#if (false)
        /** Display on GLCD run-time configuration(for the graphics2 screen) */
        display_runtime_cfg_t g_display_runtime_cfg_fg =
        {
            .input =
            {
                #if (false)
                .p_base              = (uint32_t *)&g_display_fb_foreground[0],
                #else
                .p_base              = NULL,
                #endif
                .hsize               = 800,
                .vsize               = 480,
                .hstride             = 800,
                .format              = DISPLAY_IN_FORMAT_16BITS_RGB565,
                .line_descending_enable = false,
                .lines_repeat_enable = false,
                .lines_repeat_times  = 0
             },
            .layer =
            {
                .coordinate = {
                        .x           = 0,
                        .y           = 0
                },
                .bg_color            =
                {
                    .byte            =
                    {
                        .a           = 255,
                        .r           = 255,
                        .g           = 255,
                        .b           = 255
                    }
                },
                .fade_control        = DISPLAY_FADE_CONTROL_NONE,
                .fade_speed          = 0
            }
        };
#endif

/* Instance structure to use this module. */
const display_instance_t g_display =
{ .p_ctrl = &g_display_ctrl, .p_cfg = (display_cfg_t*) &g_display_cfg, .p_api = (display_api_t*) &g_display_on_glcd };
/** GUIX Canvas Buffer */
#if false
            #if (1 == 1) /* Inherit Frame Buffer Name from Graphics Screen 1 */
            uint8_t g_sf_el_gx_canvas[sizeof(g_display_fb_background[0])] BSP_ALIGN_VARIABLE_V2(4) BSP_PLACE_IN_SECTION_V2(".sdram");
            #else /* Inherit Frame Buffer Name from Graphics Screen 2 */
            uint8_t g_sf_el_gx_canvas[sizeof(g_display_fb_foreground[0])] BSP_ALIGN_VARIABLE_V2(4) BSP_PLACE_IN_SECTION_V2(".sdram");
            #endif
            #endif

/** JPEG Work Buffer */
#if GX_USE_SYNERGY_JPEG
            #if (81920)
            uint8_t g_sf_el_gx_jpegbuffer[81920] BSP_ALIGN_VARIABLE_V2(64) BSP_PLACE_IN_SECTION_V2(".bss");
            #endif
            #endif

/** GUIX Port module control block instance */
static sf_el_gx_instance_ctrl_t g_sf_el_gx_ctrl;

/** GUIX Port module configuration */
static const sf_el_gx_cfg_t g_sf_el_gx_cfg =
{
/* Display Instance Configuration */
.p_display_instance = (display_instance_t*) &g_display,

  /* Display Driver Runtime Configuration */
#if (1 == 1) /* Inherit Frame Buffer Name from Graphics Screen 1 */
  .p_display_runtime_cfg = &g_display_runtime_cfg_bg,
#else /* Inherit Frame Buffer Name from Graphics Screen 2 */
                .p_display_runtime_cfg = &g_display_runtime_cfg_fg,
            #endif

  /* GUIX Canvas Configuration */
#if (false)
                .p_canvas        = g_sf_el_gx_canvas,
            #else
  .p_canvas = NULL,
#endif

  /* Display Driver Frame Buffer A Configuration */
#if (1 == 1) /* Inherit Frame Buffer Name from Graphics Screen 1 */
  .p_framebuffer_a = &g_display_fb_background[0], /* Always array[0] is used */
  .inherit_frame_layer = DISPLAY_FRAME_LAYER_1,
#else /* Inherit Frame Buffer Name from Graphics Screen 2 */
                .p_framebuffer_a = &g_display_fb_foreground[0], /* Always array[0] is used */
	         .inherit_frame_layer = DISPLAY_FRAME_LAYER_2,
            #endif

  /* Display Driver Frame Buffer B Configuration */
#if (1 == 1) /* Inherit Frame Buffer Name from Graphics Screen 1 */
#if (2 > 1) /* Multiple frame buffers are used for Graphics Screen 1 */
  .p_framebuffer_b = &g_display_fb_background[1], /* Always array[1] is used */
#else /* Single Frame Buffer is used for Graphics Screen 1 */
                .p_framebuffer_b = NULL,
                #endif
#else /* Inherit Frame Buffer Name from Graphics Screen 2 */
                #if (2 > 1) /* Multiple frame buffers are used for Graphics Screen 2 */
                .p_framebuffer_b = &g_display_fb_foreground[1], /* Always array[1] is used */
                #else /* Single Frame Buffer is used for Graphics Screen 2 */
                .p_framebuffer_b = NULL,
                #endif
            #endif

  /* User Callback Configuration */
  .p_callback = NULL,

  /* JPEG Work Buffer Configuration */
#if GX_USE_SYNERGY_JPEG
                .p_jpegbuffer    = g_sf_el_gx_jpegbuffer,
                .jpegbuffer_size = 81920,
                .p_sf_jpeg_decode_instance = (void *)&g_sf_jpeg_decode0,
            #else
  .p_jpegbuffer = NULL,
  .jpegbuffer_size = 0, .p_sf_jpeg_decode_instance = NULL,
#endif

  /* D/AVE 2D Buffer Cache */
  .dave2d_buffer_cache_enabled = true };

/** GUIX Port module instance */
sf_el_gx_instance_t g_sf_el_gx =
{ .p_api = &sf_el_gx_on_guix, .p_ctrl = &g_sf_el_gx_ctrl, .p_cfg = &g_sf_el_gx_cfg };
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer5) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_IIC2_RXI)
#define DTC_ACTIVATION_SRC_ELC_EVENT_IIC2_RXI
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

dtc_instance_ctrl_t g_transfer5_ctrl;
transfer_info_t g_transfer5_info =
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
const transfer_cfg_t g_transfer5_cfg =
{ .p_info = &g_transfer5_info,
  .activation_source = ELC_EVENT_IIC2_RXI,
  .auto_enable = false,
  .p_callback = NULL,
  .p_context = &g_transfer5,
  .irq_ipl = (BSP_IRQ_DISABLED) };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer5 =
{ .p_ctrl = &g_transfer5_ctrl, .p_cfg = &g_transfer5_cfg, .p_api = &g_transfer_on_dtc };
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer0) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_IIC2_TXI)
#define DTC_ACTIVATION_SRC_ELC_EVENT_IIC2_TXI
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

dtc_instance_ctrl_t g_transfer0_ctrl;
transfer_info_t g_transfer0_info =
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
const transfer_cfg_t g_transfer0_cfg =
{ .p_info = &g_transfer0_info,
  .activation_source = ELC_EVENT_IIC2_TXI,
  .auto_enable = false,
  .p_callback = NULL,
  .p_context = &g_transfer0,
  .irq_ipl = (BSP_IRQ_DISABLED) };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer0 =
{ .p_ctrl = &g_transfer0_ctrl, .p_cfg = &g_transfer0_cfg, .p_api = &g_transfer_on_dtc };
#if !defined(SSP_SUPPRESS_ISR_g_i2c) && !defined(SSP_SUPPRESS_ISR_IIC2)
SSP_VECTOR_DEFINE_CHAN(iic_rxi_isr, IIC, RXI, 2);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_i2c) && !defined(SSP_SUPPRESS_ISR_IIC2)
SSP_VECTOR_DEFINE_CHAN(iic_txi_isr, IIC, TXI, 2);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_i2c) && !defined(SSP_SUPPRESS_ISR_IIC2)
SSP_VECTOR_DEFINE_CHAN(iic_tei_isr, IIC, TEI, 2);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_i2c) && !defined(SSP_SUPPRESS_ISR_IIC2)
SSP_VECTOR_DEFINE_CHAN(iic_eri_isr, IIC, ERI, 2);
#endif
riic_instance_ctrl_t g_i2c_ctrl;
const riic_extended_cfg g_i2c_extend =
{ .timeout_mode = RIIC_TIMEOUT_MODE_SHORT, };
const i2c_cfg_t g_i2c_cfg =
        { .channel = 2, .rate = I2C_RATE_FAST, .slave = 0, .addr_mode = I2C_ADDR_MODE_7BIT, .sda_delay = 0,
#define SYNERGY_NOT_DEFINED (1)            
#if (SYNERGY_NOT_DEFINED == g_transfer0)
                .p_transfer_tx       = NULL,
#else
          .p_transfer_tx = &g_transfer0,
#endif
#if (SYNERGY_NOT_DEFINED == g_transfer5)
                .p_transfer_rx       = NULL,
#else
          .p_transfer_rx = &g_transfer5,
#endif
#undef SYNERGY_NOT_DEFINED	
          .p_callback = NULL,
          .p_context = (void*) &g_i2c, .rxi_ipl = (3), .txi_ipl = (3), .tei_ipl = (3), .eri_ipl = (3), .p_extend =
                  &g_i2c_extend, };
/* Instance structure to use this module. */
const i2c_master_instance_t g_i2c =
{ .p_ctrl = &g_i2c_ctrl, .p_cfg = &g_i2c_cfg, .p_api = &g_i2c_master_on_riic };
static TX_MUTEX sf_bus_mutex_g_sf_i2c_bus0;
static TX_EVENT_FLAGS_GROUP sf_bus_eventflag_g_sf_i2c_bus0;
static sf_i2c_instance_ctrl_t *sf_curr_ctrl_g_sf_i2c_bus0;
static sf_i2c_instance_ctrl_t *sf_curr_bus_ctrl_g_sf_i2c_bus0;
sf_i2c_bus_t g_sf_i2c_bus0 =
{ .p_bus_name = (uint8_t*) "g_sf_i2c_bus0",
  .channel = 2,
  .p_lock_mutex = &sf_bus_mutex_g_sf_i2c_bus0,
  .p_sync_eventflag = &sf_bus_eventflag_g_sf_i2c_bus0,
  .pp_curr_ctrl = (sf_i2c_ctrl_t**) &sf_curr_ctrl_g_sf_i2c_bus0,
  .p_lower_lvl_api = (i2c_api_master_t*) &g_i2c_master_on_riic,
  .device_count = 0,
  .pp_curr_bus_ctrl = (sf_i2c_ctrl_t**) &sf_curr_bus_ctrl_g_sf_i2c_bus0, };
const ioport_instance_t g_ioport =
{ .p_api = &g_ioport_on_ioport, .p_cfg = NULL };
const cgc_instance_t g_cgc =
{ .p_api = &g_cgc_on_cgc, .p_cfg = NULL };
const elc_instance_t g_elc =
{ .p_api = &g_elc_on_elc, .p_cfg = NULL };
/* Instance structure to use this module. */
const fmi_instance_t g_fmi =
{ .p_api = &g_fmi_on_fmi };
void g_common_init(void)
{
}
