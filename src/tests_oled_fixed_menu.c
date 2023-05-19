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
#include "fixed_menu.h"
#include "options_menu.h"


// Module Types Constants and Macros -------------------------------------------


// Externals -- Access to the tested Module ------------------------------------


// Globals -- Externals for the tested Module ----------------------------------
unsigned char menu_state = 0;
options_menu_st mem_options;
parameters_typedef mem_conf;

unsigned char menu_selected;
unsigned char menu_need_display_update;
unsigned char menu_selection_show;
volatile unsigned short menu_menu_timer;



// Globals ---------------------------------------------------------------------
static GMutex mutex;
int setup_done = 0;
unsigned int timer_standby = 0;
sw_actions_t switch_actions = do_nothing;



// Testing Function loop -------------------------------------------------------
gboolean Test_Main_Loop (gpointer user_data)
{
    resp_t resp = resp_continue;

    //first screen
    if (setup_done == 0)
    {
        SCREEN_Init();        
        
        FixedMenuReset();

        mem_conf.program_type = MANUAL_MODE;
        mem_conf.program_inner_type = MANUAL_INNER_FIXED_MODE;    
        mem_conf.fixed_channels[0] = 0;
        mem_conf.fixed_channels[1] = 0;
        mem_conf.fixed_channels[2] = 0;
        mem_conf.fixed_channels[3] = 0;
        mem_conf.fixed_channels[4] = 0;
        mem_conf.fixed_channels[5] = 0;
        mem_conf.dmx_channel_quantity = 4;

        setup_done = 1;
    }

    if (setup_done == 1)
    {
        resp = FixedMenu(&mem_conf, switch_actions);

        if (resp == resp_change)
        {
            printf("\nconfig change: ");

            char s_temp [100] = { 0 };            
            sprintf(s_temp, "CH1: %03d CH2: %03d CH3: %03d CH4: %03d CH5: %03d CH6: %03d",
                    mem_conf.fixed_channels[0],
                    mem_conf.fixed_channels[1],
                    mem_conf.fixed_channels[2],
                    mem_conf.fixed_channels[3],
                    mem_conf.fixed_channels[4],
                    mem_conf.fixed_channels[5]);

            printf("%s\n\n", s_temp);
                
            switch_actions = do_nothing;
        }

        if (resp == resp_finish)
        {
            printf("\nended config: ");

            char s_temp [100] = { 0 };            
            sprintf(s_temp, "CH1: %03d CH2: %03d CH3: %03d CH4: %03d CH5: %03d CH6: %03d",
                    mem_conf.fixed_channels[0],
                    mem_conf.fixed_channels[1],
                    mem_conf.fixed_channels[2],
                    mem_conf.fixed_channels[3],
                    mem_conf.fixed_channels[4],
                    mem_conf.fixed_channels[5]);

            printf("%s\n\n", s_temp);
            
            switch_actions = do_nothing;
        }        

        display_update_int_state_machine ();        
    }

    //wraper to clean sw
    g_mutex_lock (&mutex);

    if (switch_actions != do_nothing)
        switch_actions = do_nothing;
    
    g_mutex_unlock (&mutex);
        
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
