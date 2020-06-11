//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F303
// ##
// #### MAINMENU.C ############################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "mainmenu.h"
#include "ssd1306.h"
#include "hard.h"
#include "i2c.h"
#include "flash_program.h"
#include <string.h>
#include <stdio.h>
#include "uart.h"

#include "tim.h"    //quitar esto al final del debug

// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
typedef enum {
    MAIN_MENU_INIT = 0,

    MAIN_MENU_PAGE_MM_A,
    MAIN_MENU_PAGE_MM_B,

    MAIN_MENU_PAGE_MASTER_A,
    MAIN_MENU_PAGE_MASTER_B,

    MAIN_MENU_PAGE_SLAVE_A,
    MAIN_MENU_PAGE_SLAVE_B,
    MAIN_MENU_PAGE_SLAVE_CHNLS_QTTY_A,
    MAIN_MENU_PAGE_SLAVE_CHNLS_QTTY_B,    
    MAIN_MENU_PAGE_SLAVE_FIRST_CHNL_A,
    MAIN_MENU_PAGE_SLAVE_FIRST_CHNL_B,    
    MAIN_MENU_PAGE_SLAVE_GRANDM_A,
    MAIN_MENU_PAGE_SLAVE_GRANDM_B,
    
    MAIN_MENU_PAGE_PROGRAMS_A,
    MAIN_MENU_PAGE_PROGRAMS_B,
    MAIN_MENU_PAGE_PROGRAMS_PROG_A,
    MAIN_MENU_PAGE_PROGRAMS_PROG_B,
    MAIN_MENU_PAGE_PROGRAMS_SEQ_A,
    MAIN_MENU_PAGE_PROGRAMS_SEQ_B,
    
    MAIN_MENU_PAGE_HARDWARE_A,
    MAIN_MENU_PAGE_HARDWARE_B,
    MAIN_MENU_PAGE_HARDWARE_CURR_A,
    MAIN_MENU_PAGE_HARDWARE_CURR_B,
    MAIN_MENU_PAGE_HARDWARE_POWER_A,
    MAIN_MENU_PAGE_HARDWARE_POWER_B,
    MAIN_MENU_PAGE_HARDWARE_TEST_LED_A,
    MAIN_MENU_PAGE_HARDWARE_TEST_LED_B,

    MAIN_MENU_PAGE_SAVE_AND_EXIT
    
} mmenu_state_t;

static mmenu_state_t mmenu_state = MAIN_MENU_INIT;

unsigned char mm_selected = 0;
char s_opt [6][18] = { 0 };
char s_blankl [] = {"                "}; 

// Module Private Functions ----------------------------------------------------
#define set_option_string1(X)    strcpy(&s_opt[0][0], X)
#define set_option_string2(X)    strcpy(&s_opt[1][0], X)
#define set_option_string3(X)    strcpy(&s_opt[2][0], X)
#define set_option_string4(X)    strcpy(&s_opt[3][0], X)
#define set_option_string5(X)    strcpy(&s_opt[4][0], X)
#define set_option_string6(X)    strcpy(&s_opt[5][0], X)
#define blank_option_string1()    (s_opt[0][0] = '\0')
#define blank_option_string2()    (s_opt[1][0] = '\0')
#define blank_option_string3()    (s_opt[2][0] = '\0')
#define blank_option_string4()    (s_opt[3][0] = '\0')
#define blank_option_string5()    (s_opt[4][0] = '\0')
#define blank_option_string6()    (s_opt[5][0] = '\0')

// Module Funtions -------------------------------------------------------------
void MainMenu_Init (void)
{
    mmenu_state = MAIN_MENU_INIT;
}


#define last_program    mem_conf.last_program_in_flash
#define last_seq    mem_conf.last_program_deep_in_flash
#define dmx_chnls_qtty    mem_conf.dmx_channel_quantity
#define dmx_first_chnl    mem_conf.dmx_channel
#define dmx_master    mem_conf.dmx_grandmaster
#define curr_current    mem_conf.max_current_ma
#define curr_power    mem_conf.max_power
resp_t MainMenu_Update (sw_actions_t mm_action)
{
    resp_t resp = resp_continue;
    unsigned char mm_changed = 0;
    char s_temp [18] = { 0 };
    
    switch (mmenu_state)
    {
    case MAIN_MENU_INIT:
        display_clear();
        mm_changed = 1;
        mmenu_state = MAIN_MENU_PAGE_MM_A;
        break;

    case MAIN_MENU_PAGE_MM_A:
        // Menu Title
        MainMenu_SetTitle("      Main Menu");

        // Menu options
        set_option_string1("Master config.");
        set_option_string2("Slave/DMX config.");
        set_option_string3("Programs config.");
        set_option_string4("Hardware config.");
        set_option_string5("Save & Exit");
        blank_option_string6();
        
        mm_selected = 1;
        MainMenu_SetOptions(mm_selected);

        mm_changed = 1;
        
        mmenu_state = MAIN_MENU_PAGE_MM_B;
        break;

    case MAIN_MENU_PAGE_MM_B:
        if (mm_action == selection_dwn)
        {
            if (mm_selected < 5)
                mm_selected++;
            else
                mm_selected = 1;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_up)
        {
            if (mm_selected > 1)
                mm_selected--;
            else
                mm_selected = 5;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_enter)
        {
            switch (mm_selected)
            {
            case 1:
                mmenu_state = MAIN_MENU_PAGE_MASTER_A;
                break;
            case 2:
                mmenu_state = MAIN_MENU_PAGE_SLAVE_A;
                break;
            case 3:
                mmenu_state = MAIN_MENU_PAGE_PROGRAMS_A;
                break;
            case 4:
                mmenu_state = MAIN_MENU_PAGE_HARDWARE_A;
                break;
            case 5:
                mmenu_state = MAIN_MENU_PAGE_SAVE_AND_EXIT;
                break;
            }            
        }

        if (mm_action == selection_back)
            resp = resp_finish;
        
        break;

        //////////////////////////
        // MASTER CONFIGURATION //
        //////////////////////////
    case MAIN_MENU_PAGE_MASTER_A:
        // Menu Title
        MainMenu_SetTitle("Master Mode Config");

        // Menu options
        MainMenu_BlankOptions();
        set_option_string1("DMX send Enable");
        mem_conf.master_enable = 1;
        set_option_string2("Mstr Mode selected");
        mem_conf.program_type = MASTER_MODE;
        blank_option_string3();
        sprintf(s_temp, "Curr. prog: %2d", last_program);
        set_option_string4(s_temp);
        sprintf(s_temp, "Curr. pseq: %2d", last_seq);
        set_option_string5(s_temp);        
        blank_option_string6();

        mm_selected = 1;
        MainMenu_SetOptions(mm_selected);

        mm_changed = 1;
        
        mmenu_state = MAIN_MENU_PAGE_MASTER_B;
        break;

    case MAIN_MENU_PAGE_MASTER_B:
        if ((mm_action == selection_dwn) ||
            (mm_action == selection_up) ||
            (mm_action == selection_enter))
        {
            if (mem_conf.master_enable)
            {
                set_option_string1("DMX send Disable");
                mem_conf.master_enable = 0;
            }
            else
            {
                set_option_string1("DMX send Enable");
                mem_conf.master_enable = 1;
            }

            MainMenu_SetOptions(1);
            mm_changed = 1;
        }

        if (mm_action == selection_back)
            mmenu_state = MAIN_MENU_PAGE_MM_A;
        
        break;

        /////////////////////////////////
        // SLAVE AND DMX CONFIGURATION //
        /////////////////////////////////
    case MAIN_MENU_PAGE_SLAVE_A:
        // Menu Title
        MainMenu_SetTitle(" Slave/DMX Mode");

        // Menu options
        MainMenu_BlankOptions();
        set_option_string1("Channels qtty");
        set_option_string2("First channel    ");
        set_option_string3("Set Grandmaster");
        sprintf(s_temp, "Curr. qtty: %2d", dmx_chnls_qtty);
        set_option_string4(s_temp);
        sprintf(s_temp, "Curr. frst: %3d", dmx_first_chnl);
        set_option_string5(s_temp);
        if (dmx_master)
            set_option_string6("Grandmaster: Y");
        else
            set_option_string6("Grandmaster: N");

        mm_selected = 1;
        MainMenu_SetOptions(mm_selected);

        mm_changed = 1;
        
        mmenu_state = MAIN_MENU_PAGE_SLAVE_B;
        break;

    case MAIN_MENU_PAGE_SLAVE_B:
        if (mm_action == selection_dwn)
        {
            if (mm_selected < 3)
                mm_selected++;
            else
                mm_selected = 1;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_up)
        {
            if (mm_selected > 1)
                mm_selected--;
            else
                mm_selected = 3;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_enter)
        {
            switch (mm_selected)
            {
            case 1:
                mmenu_state = MAIN_MENU_PAGE_SLAVE_CHNLS_QTTY_A;
                break;
            case 2:
                mmenu_state = MAIN_MENU_PAGE_SLAVE_FIRST_CHNL_A;
                break;
            case 3:
                mmenu_state = MAIN_MENU_PAGE_SLAVE_GRANDM_A;
                break;
            }            
        }

        if (mm_action == selection_back)
            mmenu_state = MAIN_MENU_PAGE_MM_A;
        
        break;

    case MAIN_MENU_PAGE_SLAVE_CHNLS_QTTY_A:
        // Menu Title
        MainMenu_SetTitle(" Slave/DMX Mode");

        // Menu options
        MainMenu_BlankOptions();
        set_option_string1("Set with UP/DWN");
        sprintf(s_temp, "Chnls qtty: %2d", dmx_chnls_qtty);
        set_option_string2(s_temp);
        blank_option_string3();        
        blank_option_string4();
        blank_option_string5();
        blank_option_string6();
        mm_selected = 0;
        MainMenu_SetOptions(mm_selected);

        mm_changed = 1;
        
        mmenu_state = MAIN_MENU_PAGE_SLAVE_CHNLS_QTTY_B;
        break;

    case MAIN_MENU_PAGE_SLAVE_CHNLS_QTTY_B:
        if (mm_action == selection_dwn)
        {
            if (dmx_chnls_qtty > 1)
            {
                dmx_chnls_qtty--;
                MainMenu_BlankOptions();
                sprintf(s_temp, "Chnls qtty: %2d", dmx_chnls_qtty);
                set_option_string2(s_temp);
                MainMenu_SetOptions(0);
                mm_changed = 1;
            }
        }

        if (mm_action == selection_up)
        {
            if (dmx_chnls_qtty < 6)
            {
                dmx_chnls_qtty++;
                MainMenu_BlankOptions();
                sprintf(s_temp, "Chnls qtty: %2d", dmx_chnls_qtty);
                set_option_string2(s_temp);
                MainMenu_SetOptions(0);
                mm_changed = 1;
            }
        }

        if ((mm_action == selection_enter) || (mm_action == selection_back))
        {
            mmenu_state = MAIN_MENU_PAGE_SLAVE_A;
        }

        break;

    case MAIN_MENU_PAGE_SLAVE_FIRST_CHNL_A:
        // Menu Title
        MainMenu_SetTitle(" Slave/DMX Mode");

        // Menu options
        MainMenu_BlankOptions();
        set_option_string1("Set with UP/DWN");
        sprintf(s_temp, "First chnl: %2d", dmx_first_chnl);
        set_option_string2(s_temp);
        blank_option_string3();        
        blank_option_string4();
        blank_option_string5();
        blank_option_string6();
        mm_selected = 0;
        MainMenu_SetOptions(mm_selected);

        mm_changed = 1;
        
        mmenu_state = MAIN_MENU_PAGE_SLAVE_FIRST_CHNL_B;
        break;

    case MAIN_MENU_PAGE_SLAVE_FIRST_CHNL_B:
        if (mm_action == selection_dwn)
        {
            if (dmx_first_chnl > 1)
            {
                dmx_first_chnl--;
                MainMenu_BlankOptions();
                sprintf(s_temp, "First chnl: %2d", dmx_first_chnl);
                set_option_string2(s_temp);
                MainMenu_SetOptions(0);
                mm_changed = 1;
            }
        }

        if (mm_action == selection_up)
        {
            if (dmx_first_chnl <= 6)
            {
                dmx_first_chnl++;
                MainMenu_BlankOptions();
                sprintf(s_temp, "First chnl: %2d", dmx_first_chnl);
                set_option_string2(s_temp);
                MainMenu_SetOptions(0);
                mm_changed = 1;
            }
        }

        if ((mm_action == selection_enter) || (mm_action == selection_back))
        {
            mmenu_state = MAIN_MENU_PAGE_SLAVE_A;
        }

        break;

    case MAIN_MENU_PAGE_SLAVE_GRANDM_A:
        // Menu Title
        MainMenu_SetTitle(" Slave/DMX Mode");


        // Menu options
        MainMenu_BlankOptions();
        set_option_string1("Set with UP/DWN");
        if (dmx_master)
            set_option_string2("Grandmaster: Y");
        else
            set_option_string2("Grandmaster: N");
        
        blank_option_string3();        
        blank_option_string4();
        blank_option_string5();
        blank_option_string6();
        mm_selected = 0;
        MainMenu_SetOptions(mm_selected);

        mm_changed = 1;
        
        mmenu_state = MAIN_MENU_PAGE_SLAVE_GRANDM_B;
        break;

    case MAIN_MENU_PAGE_SLAVE_GRANDM_B:
        if ((mm_action == selection_dwn) || (mm_action == selection_up))
        {
            if (dmx_master)
            {
                dmx_master = 0;
                MainMenu_BlankOptions();
                set_option_string2("Grandmaster: N");
                MainMenu_SetOptions(0);
                mm_changed = 1;                
            }
            else
            {
                dmx_master = 1;
                MainMenu_BlankOptions();
                set_option_string2("Grandmaster: Y");
                MainMenu_SetOptions(0);
                mm_changed = 1;                
            }                
        }

        if ((mm_action == selection_enter) || (mm_action == selection_back))
        {
            mmenu_state = MAIN_MENU_PAGE_SLAVE_A;
        }

        break;
        
        ////////////////////////////
        // PROGRAMS CONFIGURATION //
        ////////////////////////////
    case MAIN_MENU_PAGE_PROGRAMS_A:
        // Menu Title
        MainMenu_SetTitle("  Programs Mode");

        // Menu options
        MainMenu_BlankOptions();
        set_option_string1("Set program     ");
        set_option_string2("Set program seq.");
        sprintf(s_temp, "Curr. prog: %2d", last_program);
        set_option_string3(s_temp);
        sprintf(s_temp, "Curr. pseq: %2d", last_seq);
        set_option_string4(s_temp);        
        blank_option_string5();
        blank_option_string6();
        mm_selected = 1;
        MainMenu_SetOptions(mm_selected);

        mm_changed = 1;
        
        mmenu_state = MAIN_MENU_PAGE_PROGRAMS_B;
        break;

    case MAIN_MENU_PAGE_PROGRAMS_B:
        if (mm_action == selection_dwn)
        {
            if (mm_selected < 2)
                mm_selected++;
            else
                mm_selected = 1;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_up)
        {
            if (mm_selected > 1)
                mm_selected--;
            else
                mm_selected = 2;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_enter)
        {
            switch (mm_selected)
            {
            case 1:
                mmenu_state = MAIN_MENU_PAGE_PROGRAMS_PROG_A;
                break;
            case 2:
                mmenu_state = MAIN_MENU_PAGE_PROGRAMS_SEQ_A;
                break;
            }            
        }

        if (mm_action == selection_back)
            mmenu_state = MAIN_MENU_PAGE_MM_A;
        
        break;

    case MAIN_MENU_PAGE_PROGRAMS_PROG_A:
        // Menu Title
        MainMenu_SetTitle("  Programs Mode");

        // Menu options
        MainMenu_BlankOptions();
        set_option_string1("Set with UP/DWN");
        sprintf(s_temp, "Program: %2d", last_program);
        set_option_string2(s_temp);
        blank_option_string3();        
        blank_option_string4();
        blank_option_string5();
        blank_option_string6();
        mm_selected = 0;
        MainMenu_SetOptions(mm_selected);

        mm_changed = 1;
        
        mmenu_state = MAIN_MENU_PAGE_PROGRAMS_PROG_B;
        break;

    case MAIN_MENU_PAGE_PROGRAMS_PROG_B:
        if (mm_action == selection_dwn)
        {
            if (last_program > 0)
            {
                last_program--;
                MainMenu_BlankOptions();
                sprintf(s_temp, "Program: %2d", last_program);
                set_option_string2(s_temp);
                MainMenu_SetOptions(0);
                mm_changed = 1;
            }
        }

        if (mm_action == selection_up)
        {
            if (last_program < 9)
            {
                last_program++;
                MainMenu_BlankOptions();
                sprintf(s_temp, "Program: %2d", last_program);
                set_option_string2(s_temp);
                MainMenu_SetOptions(0);
                mm_changed = 1;
            }
        }

        if ((mm_action == selection_enter) || (mm_action == selection_back))
        {
            mmenu_state = MAIN_MENU_PAGE_PROGRAMS_A;
        }

        break;

    case MAIN_MENU_PAGE_PROGRAMS_SEQ_A:
        // Menu Title
        MainMenu_SetTitle("  Programs Mode");

        // Menu options
        MainMenu_BlankOptions();
        set_option_string1("Set with UP/DWN");
        sprintf(s_temp, "P seq: %2d", last_seq);
        set_option_string2(s_temp);
        blank_option_string3();        
        blank_option_string4();
        blank_option_string5();
        blank_option_string6();
        mm_selected = 0;
        MainMenu_SetOptions(mm_selected);

        mm_changed = 1;
        
        mmenu_state = MAIN_MENU_PAGE_PROGRAMS_SEQ_B;
        break;

    case MAIN_MENU_PAGE_PROGRAMS_SEQ_B:
        if (mm_action == selection_dwn)
        {
            if (last_seq > 0)
            {
                last_seq--;
                MainMenu_BlankOptions();
                sprintf(s_temp, "P seq: %2d", last_seq);
                set_option_string2(s_temp);
                MainMenu_SetOptions(0);
                mm_changed = 1;
            }
        }

        if (mm_action == selection_up)
        {
            if (last_seq < 9)
            {
                last_seq++;
                MainMenu_BlankOptions();
                sprintf(s_temp, "P seq: %2d", last_seq);
                set_option_string2(s_temp);
                MainMenu_SetOptions(0);
                mm_changed = 1;
            }
        }

        if ((mm_action == selection_enter) || (mm_action == selection_back))
        {
            mmenu_state = MAIN_MENU_PAGE_PROGRAMS_A;
        }

        break;

        ////////////////////////////
        // HARDWARE CONFIGURATION //
        ////////////////////////////
    case MAIN_MENU_PAGE_HARDWARE_A:
        // Menu Title
        MainMenu_SetTitle("Hardware config.");

        // Menu options
        MainMenu_BlankOptions();
        set_option_string1("Set max current");
        set_option_string2("Set max power");
        set_option_string3("Test LEDs current");
        sprintf(s_temp, "Max Curr: %4dmA", curr_current);
        set_option_string4(s_temp);
        sprintf(s_temp, "Max power: %3dW", curr_power);
        set_option_string5(s_temp);        
        blank_option_string6();

        mm_selected = 1;
        MainMenu_SetOptions(mm_selected);

        mm_changed = 1;
        
        mmenu_state = MAIN_MENU_PAGE_HARDWARE_B;
        break;

    case MAIN_MENU_PAGE_HARDWARE_B:
        if (mm_action == selection_dwn)
        {
            if (mm_selected < 3)
                mm_selected++;
            else
                mm_selected = 1;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_up)
        {
            if (mm_selected > 1)
                mm_selected--;
            else
                mm_selected = 3;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_enter)
        {
            switch (mm_selected)
            {
            case 1:
                mmenu_state = MAIN_MENU_PAGE_HARDWARE_CURR_A;
                break;
            case 2:
                mmenu_state = MAIN_MENU_PAGE_HARDWARE_POWER_A;
                break;
            case 3:
                mmenu_state = MAIN_MENU_PAGE_HARDWARE_TEST_LED_A;
                break;
            }            
        }

        if (mm_action == selection_back)
            mmenu_state = MAIN_MENU_PAGE_MM_A;
        
        break;

    case MAIN_MENU_PAGE_HARDWARE_CURR_A:
        // Menu Title
        MainMenu_SetTitle("Hardware config.");

        // Menu options
        MainMenu_BlankOptions();
        set_option_string1("Set with UP/DWN");
        sprintf(s_temp, "Max curr: %4dmA", curr_current);
        set_option_string2(s_temp);
        blank_option_string3();        
        blank_option_string4();
        blank_option_string5();
        blank_option_string6();
        mm_selected = 0;
        MainMenu_SetOptions(mm_selected);

        mm_changed = 1;
        
        mmenu_state = MAIN_MENU_PAGE_HARDWARE_CURR_B;
        break;

    case MAIN_MENU_PAGE_HARDWARE_CURR_B:
        if (mm_action == selection_dwn)
        {
            if (curr_current > 500)
            {
                curr_current -= 100;
                MainMenu_BlankOptions();
                sprintf(s_temp, "Max curr: %4dmA", curr_current);
                set_option_string2(s_temp);
                MainMenu_SetOptions(0);
                mm_changed = 1;
            }
        }

        if (mm_action == selection_up)
        {
            if (curr_current < 2000)
            {
                curr_current += 100;
                MainMenu_BlankOptions();
                sprintf(s_temp, "Max curr: %4dmA", curr_current);
                set_option_string2(s_temp);
                MainMenu_SetOptions(0);
                mm_changed = 1;
            }
        }

        if ((mm_action == selection_enter) || (mm_action == selection_back))
        {
            mmenu_state = MAIN_MENU_PAGE_HARDWARE_A;
        }

        break;

    case MAIN_MENU_PAGE_HARDWARE_POWER_A:
        // Menu Title
        MainMenu_SetTitle("Hardware config.");

        // Menu options
        MainMenu_BlankOptions();
        set_option_string1("Set with UP/DWN");
        sprintf(s_temp, "Max pwr: %3dW", curr_power);
        set_option_string2(s_temp);
        blank_option_string3();        
        blank_option_string4();
        blank_option_string5();
        blank_option_string6();
        mm_selected = 0;
        MainMenu_SetOptions(mm_selected);

        mm_changed = 1;
        
        mmenu_state = MAIN_MENU_PAGE_HARDWARE_POWER_B;
        break;

    case MAIN_MENU_PAGE_HARDWARE_POWER_B:
        if (mm_action == selection_dwn)
        {
            if (curr_power > 50)
            {
                curr_power -= 10;
                MainMenu_BlankOptions();
                sprintf(s_temp, "Max pwr: %3dW", curr_power);
                set_option_string2(s_temp);
                MainMenu_SetOptions(0);
                mm_changed = 1;
            }
        }

        if (mm_action == selection_up)
        {
            if (curr_power < 200)
            {
                curr_power += 10;
                MainMenu_BlankOptions();
                sprintf(s_temp, "Max pwr: %3dW", curr_power);
                set_option_string2(s_temp);
                MainMenu_SetOptions(0);
                mm_changed = 1;
            }
        }

        if ((mm_action == selection_enter) || (mm_action == selection_back))
        {
            mmenu_state = MAIN_MENU_PAGE_HARDWARE_A;
        }

        break;

        ///////////////////////////////////////////
        // HARDWARE CONFIGURATION LED TESTING... //
        ///////////////////////////////////////////
    case MAIN_MENU_PAGE_HARDWARE_TEST_LED_A:
        // Menu Title
        MainMenu_SetTitle("Testing LEDs");

        // Menu options
        MainMenu_BlankOptions();
        display_update();        
        
        unsigned short * p_seg;
        // led_current_settings_t led_curr;
        p_seg = &mem_conf.segments[0][0];
        // HARD_Find_Current_Segments(&led_curr, p_seg);

        set_option_string1("Done!");
        blank_option_string2();
        sprintf(s_temp, "c1: %4d c2: %4d",
                mem_conf.segments[0][15],
                mem_conf.segments[1][15]);
        set_option_string3(s_temp);
        sprintf(s_temp, "c3: %4d c4: %4d",
                mem_conf.segments[2][15],
                mem_conf.segments[3][15]);
        set_option_string4(s_temp);
        sprintf(s_temp, "c5: %4d c6: %4d",
                mem_conf.segments[4][15],
                mem_conf.segments[5][15]);
        set_option_string5(s_temp);
        blank_option_string6();
        mm_selected = 0;
        MainMenu_SetOptions(mm_selected);

        mm_changed = 1;
        mmenu_state = MAIN_MENU_PAGE_HARDWARE_TEST_LED_B;

        //mando info al puerto
#ifdef USART2_DEBUG_MODE
        char s_to_send [100];
        for (unsigned char j = 0; j < 6; j++)
        {        
            sprintf(s_to_send, "segments[%d]: ", j);
            Usart2Send(s_to_send);
            // for (unsigned char i = 0; i < SEGMENTS_QTTY; i++)
            for (unsigned char i = 0; i < 16; i++)            
            {
                // sprintf(s_to_send, "%d ", segments[j][i]);
                sprintf(s_to_send, "%d ", mem_conf.segments[j][i]);
                Usart2Send(s_to_send);
                Wait_ms(10);
            }
            Usart2Send("\n");
        }
#endif
        break;

    case MAIN_MENU_PAGE_HARDWARE_TEST_LED_B:
        if ((mm_action == selection_enter) || (mm_action == selection_back))
        {
            mmenu_state = MAIN_MENU_PAGE_HARDWARE_A;
        }
        break;
        
        /////////////////
        // SAVE & EXIT //
        /////////////////
    case MAIN_MENU_PAGE_SAVE_AND_EXIT:
        mmenu_state = MAIN_MENU_INIT;
        resp = resp_need_to_save;
        break;
        
    default:
        mmenu_state = MAIN_MENU_INIT;
        break;
    }

    if (mm_changed)
        display_update();

    return resp;
}


void MainMenu_SetTitle (char * title)
{
    gfx_fillRect(0, 0, 127, 7, 0);

    // title position, color and size
    gfx_setCursor(0, 0);
    gfx_setTextSize(1);
    gfx_setTextColor(1);

    gfx_println(title);
    gfx_drawLine(0,8,127,8,1);
}


void MainMenu_BlankOptions (void)
{
    gfx_fillRect(0, 12, 127, 63 - 12, 0);    
}


void MainMenu_SetOptions (unsigned char sel)
{
    char displaced [19] = { 0 };
    char * p_s_options;

    gfx_setCursor(0, 12);

    for (unsigned char i = 1; i <= 6; i++)
    {
        if (sel == i)
        {
            gfx_setTextColor(0);
            gfx_setTextBg(1);
            p_s_options = &s_opt[i - 1][0];
            sprintf(displaced, " %s", p_s_options);
            gfx_println(displaced);
            gfx_setTextColor(1);
            gfx_setTextBg(0);        
        }
        else
        {
            p_s_options = &s_opt[i - 1][0];
            sprintf(displaced, "%s ", p_s_options);
            gfx_println(displaced);
        }
    }
}


void MainMenu_BlankAllLines (void)
{
    blank_option_string1();
    blank_option_string2();
    blank_option_string3();
    blank_option_string4();
    blank_option_string5();
    blank_option_string6();
}


void MainMenu_BlankLine1 (void)
{
    blank_option_string1();
}


void MainMenu_BlankLine2 (void)
{
    blank_option_string2();
}


void MainMenu_BlankLine3 (void)
{
    blank_option_string3();
}


void MainMenu_BlankLine4 (void)
{
    blank_option_string4();
}


void MainMenu_BlankLine5 (void)
{
    blank_option_string5();
}


void MainMenu_BlankLine6 (void)
{
    blank_option_string6();
}


void MainMenu_SetLine1 (char * s)
{
    set_option_string1(s);
}


void MainMenu_SetLine2 (char * s)
{
    set_option_string2(s);
}


void MainMenu_SetLine3 (char * s)
{
    set_option_string3(s);
}


void MainMenu_SetLine4 (char * s)
{
    set_option_string4(s);
}


void MainMenu_SetLine5 (char * s)
{
    set_option_string5(s);
}


void MainMenu_SetLine6 (char * s)
{
    set_option_string6(s);
}


//--- end of file ---//
