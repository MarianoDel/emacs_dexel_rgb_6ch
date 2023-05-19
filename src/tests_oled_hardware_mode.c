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
#include "hardware_mode.h"
#include "options_menu.h"


// Module Types Constants and Macros -------------------------------------------


// Externals -- Access to the tested Module ------------------------------------


// Globals -- Externals for the tested Module ----------------------------------
unsigned char mode_state = 0;
options_menu_st mem_options;
parameters_typedef mem_conf;

unsigned char menu_state = 0;
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
        
        HardwareModeReset();

        setup_done = 1;
    }

    if (setup_done == 1)
    {
        resp = HardwareMode (&mem_conf, switch_actions);

        if (resp == resp_finish)
        {
            printf("\nended config\n");
                
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

    HardwareMode_UpdateTimers ();

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
