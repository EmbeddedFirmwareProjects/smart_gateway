#include<zwavemode_operation_pvt.h>
#include <sw_timer.h> 


/**==========================================    NORMAL STATE   =================================================**/
State_zwavemode* ZwavemodeHandler_get_zwavemode_STATE_NORMAL(ZwaveModeHandler *thiz)
{
    return thiz->zwavemode_state_normal;
}


/**==========================================    INCLUSION STATE   =================================================**/
State_zwavemode* ZwavemodeHandler_get_zwavemode_STATE_INCLUSION(ZwaveModeHandler *thiz)
{
    return thiz->zwavemode_state_inclusion;
}

/**==========================================    INCLUSION EXECUTE STATE   ============================================**/
State_zwavemode* ZwavemodeHandler_get_zwavemode_STATE_INCLUSION_EXECUTE(ZwaveModeHandler *thiz)
{
    return thiz->zwavemode_state_inclusion_execute;
}

/**=====================================    SYSTEM CLEARING STATE    ===================================================**/
State_zwavemode* ZwavemodeHandler_get_zwavemode_STATE_SYSTEM_CLEARING(ZwaveModeHandler *thiz)
{
    return thiz->zwavemode_state_system_clearing;
}

/**=====================================    SYSTEM CLEARING READY STATE    ==============================================**/
State_zwavemode* ZwavemodeHandler_get_zwavemode_STATE_SYSTEM_CLEARING_READY(ZwaveModeHandler *thiz)
{
    return thiz->zwavemode_state_system_clearing_ready;
}

/**=====================================    SYSTEM CLEARING EXECUTE STATE   ============================================**/
State_zwavemode* ZwavemodeHandler_get_zwavemode_STATE_SYSTEM_CLEARING_EXECUTE(ZwaveModeHandler *thiz)
{
    return thiz->zwavemode_state_system_clearing_execute;
}

/**=====================================    BOOT LOADING STATE   ============================================**/
State_zwavemode* ZwavemodeHandler_get_zwavemode_STATE_BOOT_LOADING(ZwaveModeHandler *thiz)
{
    return thiz->zwavemode_state_boot_loading;
}

/**=====================================    BOOT LOADING READY STATE   ============================================**/
State_zwavemode* ZwavemodeHandler_get_zwavemode_STATE_BOOT_LOADING_READY(ZwaveModeHandler *thiz)
{
    return thiz->zwavemode_state_boot_loading_ready;
}

/**=====================================    BOOT LOADING EXECUTE STATE   ============================================**/
State_zwavemode* ZwavemodeHandler_get_zwavemode_STATE_BOOT_LOADING_EXECUTE(ZwaveModeHandler *thiz)
{
    return thiz->zwavemode_state_boot_loading_execute;
}

/**==========================================    FAULTY STATE   =================================================**/
State_zwavemode* ZwavemodeHandler_get_zwavemode_STATE_FAULTY(ZwaveModeHandler *thiz)
{
    return thiz->zwavemode_state_faulty;
}


void ZwavemodeHandler_set_zwavemode_current_state(ZwaveModeHandler *thiz, State_zwavemode *state)
{
    /*calling exit function*/
    if(thiz->zwavemode_current_state->exit != NULL)
    {
        thiz->zwavemode_current_state->exit(thiz->zwavemode_current_state);
    }
    thiz->zwavemode_current_state = state;
    
    /*calling entry function*/
    if(thiz->zwavemode_current_state->entry != NULL)
    {
        thiz->zwavemode_current_state->entry(thiz->zwavemode_current_state);
    }
}

/**==========================================   BUTTON EVENT FUNCTION   =================================================**/
void ZwavemodeHandler_handle_EVT_BUTTON_RELEASE (ZwaveModeHandler *thiz)
{
    State_zwavemode *state = thiz->zwavemode_current_state;
    
    if(state->handle_EVT_BUTTON_RELEASED != NULL)
    {
        state->handle_EVT_BUTTON_RELEASED(state);
    }
}

void ZwavemodeHandler_handle_EVT_BUTTON_PRESS (ZwaveModeHandler *thiz)
{
    State_zwavemode *state = thiz->zwavemode_current_state;
    
    if(state->handle_EVT_BUTTON_PRESSED != NULL)
    {
        state->handle_EVT_BUTTON_PRESSED(state);
    }
}

/**==========================================   ZWAVE STATE MACHINE INIT   =================================================**/

static ZwaveModeHandler zwavemode_handler                   = NEW_ZwaveModeHandler();
static sw_timer zwavemode_state_machine_sw_timer_object     = {0};

void zwavemode_init(void)
{
    printf("\nzwavemode_init");
    zwavemode_handler.zwavemode_state_normal                     = get_zwavemode_STATE_NORMAL();
    
    zwavemode_handler.zwavemode_state_inclusion                  = get_zwavemode_STATE_INCLUSION();
    zwavemode_handler.zwavemode_state_inclusion_execute          = get_zwavemode_STATE_INCLUSION_EXECUTE();
    
    zwavemode_handler.zwavemode_state_system_clearing            = get_zwavemode_STATE_SYSTEM_CLEARING();
    zwavemode_handler.zwavemode_state_system_clearing_ready      = get_zwavemode_STATE_SYSTEM_CLEARING_READY();
    zwavemode_handler.zwavemode_state_system_clearing_execute    = get_zwavemode_STATE_SYSTEM_CLEARING_EXECUTE();
    
    
    zwavemode_handler.zwavemode_state_boot_loading               = get_zwavemode_STATE_BOOT_LOADING();
    zwavemode_handler.zwavemode_state_boot_loading_ready         = get_zwavemode_STATE_BOOT_LOADING_READY();
    zwavemode_handler.zwavemode_state_boot_loading_execute       = get_zwavemode_STATE_BOOT_LOADING_EXECUTE();
    
    zwavemode_handler.zwavemode_state_faulty                     = get_zwavemode_STATE_FAULTY();
    
    
    /*Pass the object of state machine to states its nice if we have seperate call to do so*/
    zwavemode_handler.zwavemode_state_normal->zwavemode_state_machine                    = &zwavemode_handler;

    zwavemode_handler.zwavemode_state_inclusion->zwavemode_state_machine                 = &zwavemode_handler;
    zwavemode_handler.zwavemode_state_inclusion_execute->zwavemode_state_machine         = &zwavemode_handler;

    zwavemode_handler.zwavemode_state_system_clearing->zwavemode_state_machine           = &zwavemode_handler;
    zwavemode_handler.zwavemode_state_system_clearing_ready->zwavemode_state_machine     = &zwavemode_handler;
    zwavemode_handler.zwavemode_state_system_clearing_execute->zwavemode_state_machine   = &zwavemode_handler;

    zwavemode_handler.zwavemode_state_boot_loading->zwavemode_state_machine              = &zwavemode_handler;
    zwavemode_handler.zwavemode_state_boot_loading_ready->zwavemode_state_machine        = &zwavemode_handler;
    zwavemode_handler.zwavemode_state_boot_loading_execute->zwavemode_state_machine      = &zwavemode_handler;

    zwavemode_handler.zwavemode_state_faulty->zwavemode_state_machine                    = &zwavemode_handler;
    
    
    /*Load sw_timer_object & functions to zwavemode state machine*/
    zwavemode_handler.zwavemode_sw_timer_object     =   &zwavemode_state_machine_sw_timer_object;

    zwavemode_handler.zwavemode_sw_timer_create     =   sw_timer_create;
    zwavemode_handler.zwavemode_sw_timer_stop       =   sw_timer_stop;
    zwavemode_handler.zwavemode_sw_timer_start      =   sw_timer_start;
    
   
    /*set current_state as normal*/
    zwavemode_handler.zwavemode_current_state = zwavemode_handler.zwavemode_state_normal;
    
    // start state machine with entry event of normal state
    if(zwavemode_handler.zwavemode_current_state->entry != NULL)             
    {
        zwavemode_handler.zwavemode_current_state->entry(zwavemode_handler.zwavemode_current_state);
    }
    
}




/**==========================================   ZWAVE STATE MACHINE TESTING   =================================================**/

void zwavemode_housekeeping(void)
{
    while(1)
    {    
        State_zwavemode *state = zwavemode_handler.zwavemode_current_state;

        if(state->house_keeping != NULL)
        {
            state->house_keeping(state);
        }
    }

}





void zwavemode_state_event_test(void)
{

    ZwavemodeHandler_handle_EVT_BUTTON_PRESS (&zwavemode_handler);     
    sleep(2);
    ZwavemodeHandler_handle_EVT_BUTTON_RELEASE (&zwavemode_handler);
    sleep(1);

    printf("\n");
}

