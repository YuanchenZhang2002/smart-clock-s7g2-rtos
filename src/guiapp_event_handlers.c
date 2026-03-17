#include "gui/guiapp_resources.h"
#include "gui/guiapp_specifications.h"
#include <stdio.h>
#include "main_thread.h"
#include "sensor.h"
#include "mpu6050.h"
#include "clock_alarm.h"
//extern volatile TX_MUTEX g_sensor_mutex;

extern volatile uint8_t g_display_temp;//import global thread
static bool button_enabled = false;
extern TX_SEMAPHORE g_sw4_semaphore;
extern TX_SEMAPHORE g_sw5_semaphore;
extern GX_WINDOW_ROOT * p_window_root;
extern volatile rtc_time_t g_current_time;      //form clock_alarm thread
extern volatile rtc_time_t g_alarm_time;        // alarm time

extern volatile bool g_alarm_triggered;// clock trigger symbol
extern volatile bool g_alarm_enabled;  // clock enable symbol

typedef enum {//state machine types
    normal_display = 0,
    set_normal_hour,
    set_normal_minutes,
    set_normal_seconds,
    set_alarm_hour,
    set_alarm_minutes,
    set_alarm_seconds
} system_state_t;
uint8_t inc = 0;
static system_state_t current_state = normal_display;
static UINT show_window(GX_WINDOW * p_new, GX_WIDGET * p_widget, bool detach_old);
static void update_text_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT string_id);
static volatile uint8_t temp_val = 0;
UINT WindowScreenEventHandler(GX_WINDOW *widget, GX_EVENT *event_ptr)
{
    uint32_t received_msg;
    UINT status;
    static char inc_text[32];
    static char time_text[32];
    UINT result = gx_window_event_process(widget, event_ptr);
    GX_PROMPT *text_prompt = &window_screen.window_screen_led_status;  //std
    GX_PROMPT *text_prompt_inc = &window_screen.window_screen_prompt_inc;  //std
    GX_PROMPT *text_prompt_time = &window_screen.window_screen_prompt_time;  //std
    static uint8_t clock_status=0;  // std

    switch (event_ptr->gx_event_type)
    {
        case GX_EVENT_SHOW:  // std
            gx_system_timer_start(widget, GX_EVENT_TIMER, 20*1, 20*1);  // update screen every 200ms
            return gx_window_event_process(widget, event_ptr); //std
            break;
        case GX_EVENT_TIMER:  //std
            status = GX_SUCCESS;  //std
            if(tx_mutex_get( &g_sensor_mutex, TX_NO_WAIT)==TX_SUCCESS)//using mutex to transmit temperature information
                    {   g_display_temp = mpu6050_data.temperature;
                        temp_val=g_display_temp;
                        tx_mutex_put(&g_sensor_mutex);//release mutex
                    }
            snprintf(inc_text, sizeof(inc_text), "temp: %d C", temp_val);
            gx_prompt_text_set(text_prompt_inc, inc_text);

                                // 5. update GUI

            status = tx_queue_receive(&g_acc_queue, &received_msg, TX_NO_WAIT);
           if (status == TX_SUCCESS && received_msg == 1)
               {// when meg is 1，show "FALL"
                   clock_status=1;
                   gx_prompt_text_set(text_prompt, "FALL");
               }

           if (tx_semaphore_get(&g_sw5_semaphore, TX_NO_WAIT) == TX_SUCCESS)
              {
               if (g_alarm_triggered) {
                  g_alarm_triggered = false; // stop alarm when switch is pressed
               } else
                   {

                   switch (current_state)
                  {
                      case normal_display:
                          g_alarm_enabled = !g_alarm_enabled; // switch alarm ON/OFF
                          break;

                      //change time (back to RTC) */
                      case set_normal_hour:
                          if (++g_current_time.tm_hour >= 24) g_current_time.tm_hour = 0;
                          g_rtc.p_api->calendarTimeSet(g_rtc.p_ctrl, &g_current_time, true);
                          break;
                      case set_normal_minutes:
                          if (++g_current_time.tm_min >= 60) g_current_time.tm_min = 0;
                          g_rtc.p_api->calendarTimeSet(g_rtc.p_ctrl, &g_current_time, true);
                          break;
                      case set_normal_seconds:
                          g_current_time.tm_sec = 0;
                          g_rtc.p_api->calendarTimeSet(g_rtc.p_ctrl, &g_current_time, true);
                          break;

                      /* change alarm time */
                      case set_alarm_hour:
                          if (++g_alarm_time.tm_hour >= 24) g_alarm_time.tm_hour = 0;
                          break;
                      case set_alarm_minutes:
                          if (++g_alarm_time.tm_min >= 60) g_alarm_time.tm_min = 0;
                          break;
                      case set_alarm_seconds:
                          if (++g_alarm_time.tm_sec >= 60) g_alarm_time.tm_sec = 0;
                          break;
                  }
                    }
              }

           if (current_state >= set_alarm_hour && current_state <= set_alarm_seconds)
                       {

                           snprintf(time_text, sizeof(time_text), "Alarm %02d:%02d:%02d",
                                    g_alarm_time.tm_hour, g_alarm_time.tm_min, g_alarm_time.tm_sec);
                       }
                       else if(current_state>0)
                       {
                           snprintf(time_text, sizeof(time_text), "modifying %02d:%02d:%02d",
                                    g_current_time.tm_hour, g_current_time.tm_min, g_current_time.tm_sec);
                       }else{
                           snprintf(time_text, sizeof(time_text), " %02d:%02d:%02d",
                                    g_current_time.tm_hour, g_current_time.tm_min, g_current_time.tm_sec);
                       }
                       gx_prompt_text_set(text_prompt_time, time_text);

           if (tx_semaphore_get(&g_sw4_semaphore, TX_NO_WAIT) == TX_SUCCESS)
             {
                   if (g_alarm_triggered) {
                     g_alarm_triggered = false; // stop alarm when switch is pressed
                 } else {
                     // change state
                   if (current_state >= set_alarm_seconds)
                         current_state = normal_display;
                   else
                         current_state++;
                 }
             }
            break;  //std

        case GX_SIGNAL(ID_LED_BUTTON, GX_EVENT_CLICKED): //press button on screen then the clock state change

                if (clock_status)
                {
                    clock_status=0;
                    gx_prompt_text_set(text_prompt, "NORMAL");

                }


        break;


    default:
        gx_window_event_process(widget, event_ptr);
        break;
    }

    return result;
}

UINT WindowScreen2EventHandler(GX_WINDOW *widget, GX_EVENT *event_ptr)
{
    UINT result = gx_window_event_process(widget, event_ptr);

    switch (event_ptr->gx_event_type){
        case GX_EVENT_PEN_UP:
            show_window((GX_WINDOW*)&window_screen, (GX_WIDGET*)widget, true);
            break;
        default:
            result = gx_window_event_process(widget, event_ptr);
            break;
    }
    return result;
}

static UINT show_window(GX_WINDOW * p_new, GX_WIDGET * p_widget, bool detach_old)
{
    UINT err = GX_SUCCESS;

    if (!p_new->gx_widget_parent)
    {
        err = gx_widget_attach(p_window_root, p_new);
    }
    else
    {
        err = gx_widget_show(p_new);
    }

    gx_system_focus_claim(p_new);

    GX_WIDGET * p_old = p_widget;
    if (p_old && detach_old)
    {
        if (p_old != (GX_WIDGET*)p_new)
        {
            gx_widget_detach(p_old);
        }
    }

    return err;
}

static void update_text_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT string_id)
{
    GX_PROMPT * p_prompt = NULL;

    ssp_err_t err = gx_widget_find(p_widget, (USHORT)id, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_prompt);
    if (TX_SUCCESS == err)
    {
        gx_prompt_text_id_set(p_prompt, string_id);
    }
}

