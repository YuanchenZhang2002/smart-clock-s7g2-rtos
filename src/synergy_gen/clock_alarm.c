/* generated thread source file - do not edit */
#include "clock_alarm.h"

TX_THREAD clock_alarm;
void clock_alarm_create(void);
static void clock_alarm_func(ULONG thread_input);
static uint8_t clock_alarm_stack[1024] BSP_PLACE_IN_SECTION_V2(".stack.clock_alarm") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init(void);
#if (3) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_timer1) && !defined(SSP_SUPPRESS_ISR_GPT1)
SSP_VECTOR_DEFINE_CHAN(gpt_counter_overflow_isr, GPT, COUNTER_OVERFLOW, 1);
#endif
#endif
static gpt_instance_ctrl_t g_timer1_ctrl;
static const timer_on_gpt_cfg_t g_timer1_extend =
{ .gtioca =
{ .output_enabled = false, .stop_level = GPT_PIN_LEVEL_LOW },
  .gtiocb =
  { .output_enabled = false, .stop_level = GPT_PIN_LEVEL_LOW },
  .shortest_pwm_signal = GPT_SHORTEST_LEVEL_OFF, };
static const timer_cfg_t g_timer1_cfg =
{ .mode = TIMER_MODE_PERIODIC,
  .period = 500,
  .unit = TIMER_UNIT_PERIOD_MSEC,
  .duty_cycle = 50,
  .duty_cycle_unit = TIMER_PWM_UNIT_RAW_COUNTS,
  .channel = 1,
  .autostart = true,
  .p_callback = cb_timer_ch1,
  .p_context = &g_timer1,
  .p_extend = &g_timer1_extend,
  .irq_ipl = (3), };
/* Instance structure to use this module. */
const timer_instance_t g_timer1 =
{ .p_ctrl = &g_timer1_ctrl, .p_cfg = &g_timer1_cfg, .p_api = &g_timer_on_gpt };
#if (3) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_timer0) && !defined(SSP_SUPPRESS_ISR_GPT0)
SSP_VECTOR_DEFINE_CHAN(gpt_counter_overflow_isr, GPT, COUNTER_OVERFLOW, 0);
#endif
#endif
static gpt_instance_ctrl_t g_timer0_ctrl;
static const timer_on_gpt_cfg_t g_timer0_extend =
{ .gtioca =
{ .output_enabled = false, .stop_level = GPT_PIN_LEVEL_LOW },
  .gtiocb =
  { .output_enabled = false, .stop_level = GPT_PIN_LEVEL_LOW },
  .shortest_pwm_signal = GPT_SHORTEST_LEVEL_OFF, };
static const timer_cfg_t g_timer0_cfg =
{ .mode = TIMER_MODE_ONE_SHOT,
  .period = 60,
  .unit = TIMER_UNIT_PERIOD_SEC,
  .duty_cycle = 50,
  .duty_cycle_unit = TIMER_PWM_UNIT_RAW_COUNTS,
  .channel = 0,
  .autostart = true,
  .p_callback = cb_timer_ch0,
  .p_context = &g_timer0,
  .p_extend = &g_timer0_extend,
  .irq_ipl = (3), };
/* Instance structure to use this module. */
const timer_instance_t g_timer0 =
{ .p_ctrl = &g_timer0_ctrl, .p_cfg = &g_timer0_cfg, .p_api = &g_timer_on_gpt };
#if (2) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_rtc) && !defined(SSP_SUPPRESS_ISR_RTC)
SSP_VECTOR_DEFINE( rtc_alarm_isr, RTC, ALARM);
#endif
#endif
#if (3) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_rtc) && !defined(SSP_SUPPRESS_ISR_RTC)
SSP_VECTOR_DEFINE( rtc_period_isr, RTC, PERIOD);
#endif
#endif
#if (3) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_rtc) && !defined(SSP_SUPPRESS_ISR_RTC)
SSP_VECTOR_DEFINE( rtc_carry_isr, RTC, CARRY);
#endif
#endif
rtc_instance_ctrl_t g_rtc_ctrl;
const rtc_cfg_t g_rtc_cfg =
{ .clock_source = RTC_CLOCK_SOURCE_SUBCLK, .hw_cfg = true, .error_adjustment_value = 0, .error_adjustment_type =
          RTC_ERROR_ADJUSTMENT_NONE,
  .p_callback = NULL, .p_context = &g_rtc, .alarm_ipl = (2), .periodic_ipl = (3), .carry_ipl = (3), };
/* Instance structure to use this module. */
const rtc_instance_t g_rtc =
{ .p_ctrl = &g_rtc_ctrl, .p_cfg = &g_rtc_cfg, .p_api = &g_rtc_on_rtc };
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void clock_alarm_create(void)
{
    /* Increment count so we will know the number of ISDE created threads. */
    g_ssp_common_thread_count++;

    /* Initialize each kernel object. */

    UINT err;
    err = tx_thread_create (&clock_alarm, (CHAR*) "Clock And Alarm", clock_alarm_func, (ULONG) NULL, &clock_alarm_stack,
                            1024, 5, 5, 1, TX_AUTO_START);
    if (TX_SUCCESS != err)
    {
        tx_startup_err_callback (&clock_alarm, 0);
    }
}

static void clock_alarm_func(ULONG thread_input)
{
    /* Not currently using thread_input. */
    SSP_PARAMETER_NOT_USED (thread_input);

    /* Initialize common components */
    tx_startup_common_init ();

    /* Initialize each module instance. */

    /* Enter user code for this thread. */
    clock_alarm_entry ();
}
