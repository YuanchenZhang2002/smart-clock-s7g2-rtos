/* generated thread header file - do not edit */
#ifndef CLOCK_ALARM_H_
#define CLOCK_ALARM_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void clock_alarm_entry(void);
                #else
extern void clock_alarm_entry(void);
#endif
#include "r_gpt.h"
#include "r_timer_api.h"
#include "r_rtc.h"
#include "r_rtc_api.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer1;
#ifndef cb_timer_ch1
void cb_timer_ch1(timer_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer0;
#ifndef cb_timer_ch0
void cb_timer_ch0(timer_callback_args_t *p_args);
#endif
/** RTC on RTC Instance. */
extern const rtc_instance_t g_rtc;
#ifndef NULL
void NULL(rtc_callback_args_t *p_args);
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* CLOCK_ALARM_H_ */
