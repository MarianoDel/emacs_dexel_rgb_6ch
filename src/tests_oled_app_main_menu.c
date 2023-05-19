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
#include "main_menu.h"
#include "options_menu.h"


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



// Testing Function loop -------------------------------------------------------
gboolean Test_Main_Loop (gpointer user_data)
{
    resp_t resp = resp_continue;

    //first screen
    if (setup_done == 0)
    {
        SCREEN_Init();        
        
        MainMenuReset();

        mem_conf.program_type = DMX1_MODE;

        setup_done = 1;
    }

    if (setup_done == 1)
    {
        resp = MainMenu(&mem_conf, switch_actions);

        if ((resp == resp_finish) || (resp == resp_need_to_save))
        {
            printf("\nended selections: ");

            switch (mem_conf.program_type)
            {
            case DMX1_MODE:
                printf("DMX1 MODE SELECTED\n\n");
                break;

            case DMX2_MODE:
                printf("DMX2 MODE SELECTED\n\n");                
                break;

            case MASTER_SLAVE_MODE:
                printf("MASTER/SLAVE MODE SELECTED\n\n");
                break;

            case MANUAL_MODE:
                printf("MANUAL MODE SELECTED\n\n");
                break;

            case RESET_MODE:
                printf("RESET MODE SELECTED\n\n");
                break;
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
