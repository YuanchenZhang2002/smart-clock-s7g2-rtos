#include "gui/guiapp_resources.h"
#include "gui/guiapp_specifications.h"

#include "main_thread.h"

static bool button_enabled = false;

extern GX_WINDOW_ROOT * p_window_root;

static UINT show_window(GX_WINDOW * p_new, GX_WIDGET * p_widget, bool detach_old);
static void update_text_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT string_id);

UINT WindowScreenEventHandler(GX_WINDOW *widget, GX_EVENT *event_ptr)
{
    UINT status;
    UINT result = gx_window_event_process(widget, event_ptr);
    GX_PROMPT *text_prompt = &window_screen.window_screen_led_status;  //std
    uint8_t led_status;  // std

    switch (event_ptr->gx_event_type)
    {
        case GX_EVENT_SHOW:  // std
            gx_system_timer_start(widget, GX_EVENT_TIMER, 20*5, 0);  // std
            return gx_window_event_process(widget, event_ptr); //std
            break;
        case GX_EVENT_TIMER:  //std
            status = GX_SUCCESS;  //std
            break;  //std

        case GX_SIGNAL(ID_LED_BUTTON, GX_EVENT_CLICKED): //
                g_ioport.p_api->pinRead(IOPORT_PORT_06_PIN_00, &led_status); //
                if (led_status)
                {
                    gx_prompt_text_set(text_prompt, "LED ON");
                    g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_00,IOPORT_LEVEL_LOW);
                }
                else
                {
                    gx_prompt_text_set(text_prompt, "LED OFF");
                    g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_00,IOPORT_LEVEL_HIGH);
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

