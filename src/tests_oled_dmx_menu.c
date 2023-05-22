//----------------------------------------------------
// Tests for gtk lib
// Implementation of tests_lcd_application.h functions
//----------------------------------------------------

#include <gtk/gtk.h>
#include <stdint.h>
#include "tests_oled_application.h"

// Application Includes needed for this test
#include "screen.h"
#include "switches_answers.h"
#include "dmx_menu.h"
#include "options_menu.h"
#include "parameters.h"


// Module Types Constants and Macros -------------------------------------------


// Externals -- Access to the tested Module ------------------------------------


// Globals -- Externals for the tested Module ----------------------------------
unsigned char menu_state = 0;
options_menu_st mem_options;
parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
static GMutex mutex;
int setup_done = 0;
unsigned int timer_standby = 0;
sw_actions_t switch_actions = do_nothing;
unsigned char ch_val [6] = { 0 };



// Testing Function loop -------------------------------------------------------
gboolean Test_Main_Loop (gpointer user_data)
{
    resp_t resp = resp_continue;
    dmx_menu_data_t dmx1_st;
    

    //first screen
    if (setup_done == 0)
    {
        SCREEN_Init();        
        
        DMXModeMenuReset();

        *(ch_val + 0) = 0;
        *(ch_val + 1) = 0;
        *(ch_val + 2) = 0;
        *(ch_val + 3) = 0;
        *(ch_val + 4) = 0;
        *(ch_val + 5) = 0;

        mem_conf.program_type = DMX1_MODE;
        mem_conf.dmx_first_channel = 1;
        mem_conf.dmx_channel_quantity = 6;
        
        setup_done = 1;
    }

    if (setup_done == 1)
    {
        dmx1_st.dmx_first_chnl = &mem_conf.dmx_first_channel;
        dmx1_st.mode = DMX1_MODE;
        dmx1_st.pchannels = ch_val;
        dmx1_st.chnls_qtty = mem_conf.dmx_channel_quantity;
        dmx1_st.show_addres = 1;
        
        resp = DMXModeMenu(&dmx1_st);

        if (resp == resp_finish)
        {
            if (!timer_standby)
            {
                timer_standby = 50;
                
                unsigned char last_val = *(ch_val + 0);            
                if (last_val < 255)
                {
                    for (int i = 0; i < 6; i++)
                        *(ch_val + i) = last_val + 1;
                }
                else
                {
                    for (int i = 0; i < 6; i++)
                        *(ch_val + i) = 0;

                    if (mem_conf.dmx_first_channel < (511 - mem_conf.dmx_channel_quantity))
                        mem_conf.dmx_first_channel += 1;
                    else
                    {
                        mem_conf.dmx_first_channel = 1;                        
                    }
                }
            }
            
            switch_actions = do_nothing;
        }

        display_update_int_state_machine ();        
    }

    //wraper to clean sw
    g_mutex_lock (&mutex);

    if (switch_actions != do_nothing)
        switch_actions = do_nothing;
    
    g_mutex_unlock (&mutex);

    usleep(1000);

    return TRUE;
}


gboolean Test_Timeouts_Loop_1ms (gpointer user_data)
{
    if (timer_standby)
        timer_standby--;
    // //timeout lcd_utils internal
    // if (show_select_timer)
    //     show_select_timer--;

    return TRUE;
}


gboolean Test_Timeouts_Loop_1000ms (gpointer user_data)
{
    return TRUE;
}


// Module Implementation of buttons functions
void ccw_button_function (void)
{
    g_mutex_lock (&mutex);
    switch_actions = selection_dwn;
    g_mutex_unlock (&mutex);
}


void cw_button_function (void)
{
    g_mutex_lock (&mutex);
    switch_actions = selection_up;
    g_mutex_unlock (&mutex);
}


void enter_button_function (void)
{
    g_mutex_lock (&mutex);
    switch_actions = selection_enter;
    g_mutex_unlock (&mutex);
}


//--- end of file ---//
