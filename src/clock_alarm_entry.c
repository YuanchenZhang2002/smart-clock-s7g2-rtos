#include "clock_alarm.h"

volatile rtc_time_t g_current_time;
volatile rtc_time_t g_alarm_time = { .tm_hour = 23, .tm_min = 59, .tm_sec = 50 }; // default alarm 23:59：50
volatile bool g_alarm_triggered = false;
volatile bool g_alarm_enabled = false;//alarm enable
bool last_tick_triggered = false;//using for prevent repeating trigger in 1 min
volatile uint8_t g_alarm_state=IOPORT_LEVEL_HIGH;

/* Clock And Alarm entry function */
void clock_alarm_entry(void)
{
    ssp_err_t err = g_rtc.p_api->open(g_rtc.p_ctrl, g_rtc.p_cfg);
    rtc_time_t initial_time = { .tm_hour = 23, .tm_min = 59, .tm_sec = 50, .tm_mday = 1, .tm_mon = 0, .tm_year = 126 };
    g_rtc.p_api->calendarTimeSet(g_rtc.p_ctrl, &initial_time, true);
    g_timer1.p_api->open(g_timer1.p_ctrl,g_timer1.p_cfg);
    g_timer0.p_api->open(g_timer0.p_ctrl,g_timer0.p_cfg);
    while (1)
    {
        tx_thread_sleep (1);
        g_rtc.p_api->calendarTimeGet(g_rtc.p_ctrl,&g_current_time);

        if (!last_tick_triggered && !g_alarm_triggered && g_alarm_enabled && g_current_time.tm_hour ==
               g_alarm_time.tm_hour && g_current_time.tm_min  == g_alarm_time.tm_min &&g_current_time.tm_sec  == g_alarm_time.tm_sec)
           {
               g_alarm_triggered = true; // trigger alarm
               last_tick_triggered = true;//show this second trigger state
               g_timer0.p_api->start(g_timer0.p_ctrl);//1 minutes oneshot timer
               g_timer1.p_api->start(g_timer1.p_ctrl);//500ms timer period timer to toggle leds
           }
        if (g_current_time.tm_sec != 0)
           {
               last_tick_triggered = false;//update this second trigger state
           }


        if (g_alarm_triggered)
          {
               g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_00,g_alarm_state);
               g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_01,!g_alarm_state);
           }else
           {   timer_info_t info;
               g_timer1.p_api->infoGet(g_timer1.p_ctrl, &info);
               if(info.status==TIMER_STATUS_COUNTING)//prevent unnecessary manipulating
               {
               g_timer1.p_api->stop(g_timer1.p_ctrl);//stop alarm
               g_timer0.p_api->reset(g_timer0.p_ctrl);//reset 1 min timer0
               g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_00,IOPORT_LEVEL_HIGH);
               g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_01,IOPORT_LEVEL_HIGH);
               }
           }

        g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_02,!g_alarm_enabled);//the third led to show alarm state
    }
}


void cb_timer_ch0(timer_callback_args_t *p_args)
{
    g_alarm_triggered=false;
}
void cb_timer_ch1(timer_callback_args_t *p_args)
{
    g_alarm_state=1^g_alarm_state;
}
