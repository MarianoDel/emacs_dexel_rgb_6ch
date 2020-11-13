//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MAIN_MENU.C ###############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "mainmenu.h"
#include "programs_functions.h"
#include "ssd1306_gfx.h"
// #include "ssd1306_display.h"
#include "parameters.h"

#include <string.h>
#include <stdio.h>

// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
typedef enum {
    MAIN_MENU_INIT = 0,

    MAIN_MENU_PAGE_MM_AA,
    MAIN_MENU_PAGE_MM_A,
    MAIN_MENU_PAGE_MM_B,

    MAIN_MENU_PAGE_MASTER_A,
    MAIN_MENU_PAGE_MASTER_B,
    MAIN_MENU_PAGE_MASTER_BACK,    

    MAIN_MENU_PAGE_DMX1_A,
    MAIN_MENU_PAGE_DMX1_B,
    MAIN_MENU_PAGE_DMX1_CHNLS_QTTY_A,
    MAIN_MENU_PAGE_DMX1_CHNLS_QTTY_B,    
    MAIN_MENU_PAGE_DMX1_FIRST_CHNL_A,
    MAIN_MENU_PAGE_DMX1_FIRST_CHNL_B,    
    MAIN_MENU_PAGE_DMX1_BACK,    
    
    MAIN_MENU_PAGE_PROGRAMS_A,
    MAIN_MENU_PAGE_PROGRAMS_B,
    MAIN_MENU_PAGE_PROGRAMS_PROG_A,
    MAIN_MENU_PAGE_PROGRAMS_PROG_B,
    MAIN_MENU_PAGE_PROGRAMS_SEQ_A,
    MAIN_MENU_PAGE_PROGRAMS_SEQ_B,
    MAIN_MENU_PAGE_PROGRAMS_BACK,    
    
    MAIN_MENU_PAGE_HARDWARE_A,
    MAIN_MENU_PAGE_HARDWARE_B,
    MAIN_MENU_PAGE_HARDWARE_BACK,

    MAIN_MENU_PAGE_EXIT,
    MAIN_MENU_PAGE_SAVE_AND_EXIT
    
} mmenu_state_t;

static mmenu_state_t mmenu_state = MAIN_MENU_INIT;

#define ALL_LINE_LENGTH    22
unsigned char mm_selected = 0;
char s_opt [6][ALL_LINE_LENGTH] = { 0 };
char s_blankl [] = {"                     "}; 

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

resp_t MainMenu_CheckFree (sw_actions_t);

char s_dmx1_title [] =    {"   Dmx1/DMX Mode"};
char s_programs_title [] = {"   Programs Mode "};
char s_hardware_title [] = {"   Hardware Config"};

// Module Funtions -------------------------------------------------------------
void MainMenu_Init (void)
{
    mmenu_state = MAIN_MENU_INIT;
}


// general config
#define program_type    mem_conf.program_type
// programs mode conf
#define last_program    mem_conf.last_program_in_flash
#define last_seq    mem_conf.last_program_deep_in_flash
// dmx master conf
#define dmx_master_enable    mem_conf.master_send_dmx_enable
// dmx dmx1 conf
#define dmx_chnls_qtty    mem_conf.dmx_channel_quantity
#define dmx_first_chnl    mem_conf.dmx_first_channel
#define curr_power    mem_conf.max_power
resp_t MainMenu_Update (sw_actions_t mm_action)
{
    resp_t resp = resp_continue;
    unsigned char mm_changed = 0;
    char s_temp [ALL_LINE_LENGTH] = { 0 };
    
    switch (mmenu_state)
    {
    case MAIN_MENU_INIT:
        display_clear();

        // Menu Title
        MainMenu_SetTitle(" Entering Main Menu");
        
        mm_changed = 1;
        mmenu_state = MAIN_MENU_PAGE_MM_AA;
        break;

    case MAIN_MENU_PAGE_MM_AA:
        if (mm_action == do_nothing)
        {
            display_clear();
            mmenu_state = MAIN_MENU_PAGE_MM_A;
        }
        break;

    case MAIN_MENU_PAGE_MM_A:
        // Menu Title
        // MainMenu_SetTitle("      Main Menu");

        // Menu options
        set_option_string1("DMX 1");
        set_option_string2("DMX 2");
        set_option_string3("MASTER/SLAVE");
        set_option_string4("MANUAL");
        set_option_string5("RESET");

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

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_up)
        {
            if (mm_selected > 1)
                mm_selected--;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_enter)
        {
            switch (mm_selected)
            {
            case 1:
                mmenu_state = MAIN_MENU_PAGE_DMX1_A;
                break;
            case 2:
                mmenu_state = MAIN_MENU_PAGE_DMX1_A;
                break;
            case 3:
                mmenu_state = MAIN_MENU_PAGE_PROGRAMS_A;
                break;
            case 4:
                mmenu_state = MAIN_MENU_PAGE_HARDWARE_A;
                break;
            case 5:
                mmenu_state = MAIN_MENU_PAGE_EXIT;
                break;
            case 6:
                mmenu_state = MAIN_MENU_PAGE_SAVE_AND_EXIT;
                break;
            }            
        }
        break;

        //////////////////////////
        // MASTER CONFIGURATION //
        //////////////////////////
    case MAIN_MENU_PAGE_MASTER_A:
        resp = MainMenu_CheckFree(mm_action);

        if (resp == resp_ok)
        {
            program_type = MASTER_MODE;
            // Menu Title
            MainMenu_SetTitle("Master Mode Config");

            // Menu options
            MainMenu_BlankOptions();
            if (dmx_master_enable)
                set_option_string1("DMX send Enable");
            else
                set_option_string1("DMX send Disable");
            set_option_string2("Back!");

            blank_option_string3();
            
            set_option_string4("Mstr Mode selected");
            
            sprintf(s_temp, "Curr. prog: %2d", last_program);
            set_option_string5(s_temp);
            sprintf(s_temp, "Curr. pseq: %2d", last_seq);
            set_option_string6(s_temp);        

            mm_selected = 1;
            MainMenu_SetOptions(mm_selected);

            mm_changed = 1;
            
            mmenu_state = MAIN_MENU_PAGE_MASTER_B;
            resp = resp_continue;
        }
        break;

    case MAIN_MENU_PAGE_MASTER_B:
        if (mm_action == selection_dwn)
        {
            if (mm_selected < 2)
                mm_selected++;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_up)
        {
            if (mm_selected > 1)
                mm_selected--;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_enter)
        {
            switch (mm_selected)
            {
            case 1:
                if (dmx_master_enable)
                {
                    set_option_string1("DMX send Disable");
                    dmx_master_enable = 0;
                }
                else
                {
                    set_option_string1("DMX send Enable");
                    dmx_master_enable = 1;
                }

                MainMenu_SetOptions(1);
                mm_changed = 1;
                break;

            case 2:
                mmenu_state = MAIN_MENU_PAGE_MASTER_BACK;
                break;
            }            
        }
        break;

    case MAIN_MENU_PAGE_MASTER_BACK:
        resp = MainMenu_CheckFree(mm_action);
        if (resp == resp_ok)
        {
            mmenu_state = MAIN_MENU_INIT;
            resp = resp_continue;
        }
        break;        

        /////////////////////
        // DMX1 Common Dmx //
        /////////////////////
    case MAIN_MENU_PAGE_DMX1_A:
        resp = MainMenu_CheckFree(mm_action);

        if (resp == resp_ok)
        {
            program_type = DMX1_MODE;

            // Menu options
            MainMenu_BlankOptions();

            sprintf(s_temp, "ADDR: %3d", dmx_first_chnl);            
            MainMenu_SetLine1(s_temp);
            MainMenu_BlankLine2();
            sprintf(s_temp, "CH1: %3d     CH4: %3d", 0, 255);            
            MainMenu_SetLine3(s_temp);
            sprintf(s_temp, "CH2: %3d     CH5: %3d", 255, 0);                        
            MainMenu_SetLine4(s_temp);
            sprintf(s_temp, "CH3: %3d     CH6: %3d", 128, 127);                        
            MainMenu_SetLine5(s_temp);
            MainMenu_SetLine6("012345678901234567890");            
            
            mm_selected = 0;
            MainMenu_SetOptions(mm_selected);

            mm_changed = 1;
            
            mmenu_state = MAIN_MENU_PAGE_DMX1_B;
            resp = resp_continue;
        }
        break;

    case MAIN_MENU_PAGE_DMX1_B:
        if (mm_action == selection_dwn)
        {
            if (mm_selected < 4)
                mm_selected++;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_up)
        {
            if (mm_selected > 1)
                mm_selected--;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_enter)
        {
            switch (mm_selected)
            {
            case 1:
                mmenu_state = MAIN_MENU_PAGE_DMX1_CHNLS_QTTY_A;
                break;
            case 2:
                mmenu_state = MAIN_MENU_PAGE_DMX1_FIRST_CHNL_A;
                break;
            case 3:
                break;
            case 4:
                mmenu_state = MAIN_MENU_PAGE_DMX1_BACK;
                break;
            }            
        }
        break;

    case MAIN_MENU_PAGE_DMX1_CHNLS_QTTY_A:
        resp = MainMenu_CheckFree(mm_action);

        if (resp == resp_ok)
        {
            // Menu Title
            MainMenu_SetTitle(s_dmx1_title);

            // Menu options
            MainMenu_BlankOptions();
            set_option_string1("Change UP/DWN");
            set_option_string2("Set to Finish");
            blank_option_string3();
        
            sprintf(s_temp, "Chnls qtty: %2d", dmx_chnls_qtty);
            set_option_string4(s_temp);
            blank_option_string5();
            blank_option_string6();
            mm_selected = 0;
            MainMenu_SetOptions(mm_selected);

            mm_changed = 1;
        
            mmenu_state = MAIN_MENU_PAGE_DMX1_CHNLS_QTTY_B;
            resp = resp_continue;
        }
        break;

    case MAIN_MENU_PAGE_DMX1_CHNLS_QTTY_B:
        
        if ((mm_action == selection_dwn) ||
            (mm_action == selection_up)) 
        {
            if ((mm_action == selection_dwn) &&
                (dmx_chnls_qtty > 1))
                dmx_chnls_qtty--;

            if ((mm_action == selection_up) &&
                (dmx_chnls_qtty < 6))
                dmx_chnls_qtty++;
                
            MainMenu_BlankOptions();
            sprintf(s_temp, "Chnls qtty: %2d", dmx_chnls_qtty);
            set_option_string4(s_temp);
            MainMenu_SetOptions(0);
            mm_changed = 1;
        }

        if (mm_action == selection_enter)
            mmenu_state = MAIN_MENU_PAGE_DMX1_A;

        break;

    case MAIN_MENU_PAGE_DMX1_FIRST_CHNL_A:
        resp = MainMenu_CheckFree(mm_action);

        if (resp == resp_ok)
        {
            // Menu Title
            MainMenu_SetTitle(s_dmx1_title);

            // Menu options
            MainMenu_BlankOptions();
            set_option_string1("Change UP/DWN");
            set_option_string2("Set to Finish");
            blank_option_string3();

            sprintf(s_temp, "First chnl: %2d", dmx_first_chnl);
            set_option_string4(s_temp);

            blank_option_string5();
            blank_option_string6();
            mm_selected = 0;
            MainMenu_SetOptions(mm_selected);

            mm_changed = 1;
        
            mmenu_state = MAIN_MENU_PAGE_DMX1_FIRST_CHNL_B;
            resp = resp_continue;
        }
        break;

    case MAIN_MENU_PAGE_DMX1_FIRST_CHNL_B:

        if ((mm_action == selection_dwn) ||
            (mm_action == selection_up))
        {
            if ((mm_action == selection_dwn) &&
                (dmx_first_chnl > 2))
                dmx_first_chnl--;

            if ((mm_action == selection_up) &&
                (dmx_first_chnl < (511 - dmx_chnls_qtty - 1)))
                dmx_first_chnl++;
            
            MainMenu_BlankOptions();
            sprintf(s_temp, "First chnl: %2d", dmx_first_chnl);
            set_option_string4(s_temp);
            MainMenu_SetOptions(0);
            mm_changed = 1;
        }

        if (mm_action == selection_enter)
            mmenu_state = MAIN_MENU_PAGE_DMX1_A;

        break;


    case MAIN_MENU_PAGE_DMX1_BACK:
        resp = MainMenu_CheckFree(mm_action);
        if (resp == resp_ok)
        {
            mmenu_state = MAIN_MENU_INIT;
            resp = resp_continue;
        }
        break;
        
        ////////////////////////////
        // PROGRAMS CONFIGURATION //
        ////////////////////////////
    case MAIN_MENU_PAGE_PROGRAMS_A:
        resp = MainMenu_CheckFree(mm_action);

        if (resp == resp_ok)
        {
            program_type = PROGRAMS_MODE;
            // Menu Title
            MainMenu_SetTitle(s_programs_title);

            // Menu options
            MainMenu_BlankOptions();
            set_option_string1("Set program     ");
            set_option_string2("Set program seq.");
            set_option_string3("Back!");
            blank_option_string4();

            sprintf(s_temp, "Curr. prog: %2d", last_program);
            set_option_string5(s_temp);
            sprintf(s_temp, "Curr. pseq: %2d", last_seq);
            set_option_string6(s_temp);        

            mm_selected = 1;
            MainMenu_SetOptions(mm_selected);

            mm_changed = 1;
        
            mmenu_state = MAIN_MENU_PAGE_PROGRAMS_B;
            resp = resp_continue;
        }
        break;

    case MAIN_MENU_PAGE_PROGRAMS_B:
        if (mm_action == selection_dwn)
        {
            if (mm_selected < 3)
                mm_selected++;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_up)
        {
            if (mm_selected > 1)
                mm_selected--;

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
            case 3:
                mmenu_state = MAIN_MENU_PAGE_PROGRAMS_BACK;
                break;
            }            
        }

        break;

    case MAIN_MENU_PAGE_PROGRAMS_PROG_A:
        resp = MainMenu_CheckFree(mm_action);

        if (resp == resp_ok)
        {
            // Menu Title
            MainMenu_SetTitle(s_programs_title);

            // Menu options
            MainMenu_BlankOptions();
            set_option_string1("Change UP/DWN");
            set_option_string2("Set to Finish");
            blank_option_string3();

            sprintf(s_temp, "Program: %2d", last_program);
            set_option_string4(s_temp);
            blank_option_string5();
            blank_option_string6();

            mm_selected = 0;
            MainMenu_SetOptions(mm_selected);

            mm_changed = 1;
        
            mmenu_state = MAIN_MENU_PAGE_PROGRAMS_PROG_B;
            resp = resp_continue;
        }
        break;

    case MAIN_MENU_PAGE_PROGRAMS_PROG_B:

        if ((mm_action == selection_dwn) ||
            (mm_action == selection_up))
        {
            if ((mm_action == selection_dwn) &&
                (last_program > 0))
                last_program--;

            if ((mm_action == selection_up) &&
                (last_program < 9))
                last_program++;
            
            MainMenu_BlankOptions();
            sprintf(s_temp, "Program: %2d", last_program);
            set_option_string4(s_temp);
            MainMenu_SetOptions(0);
            mm_changed = 1;
        }

        if (mm_action == selection_enter)
            mmenu_state = MAIN_MENU_PAGE_PROGRAMS_A;

        break;

    case MAIN_MENU_PAGE_PROGRAMS_SEQ_A:
        resp = MainMenu_CheckFree(mm_action);

        if (resp == resp_ok)
        {
            // Menu Title
            MainMenu_SetTitle(s_programs_title);

            // Menu options
            MainMenu_BlankOptions();
            set_option_string1("Change UP/DWN");
            set_option_string2("Set to Finish");
            blank_option_string3();

            sprintf(s_temp, "P seq: %2d", last_seq);
            set_option_string4(s_temp);
            blank_option_string5();
            blank_option_string6();
            
            mm_selected = 0;
            MainMenu_SetOptions(mm_selected);

            mm_changed = 1;
        
            mmenu_state = MAIN_MENU_PAGE_PROGRAMS_SEQ_B;
            resp = resp_continue;
        }
        break;

    case MAIN_MENU_PAGE_PROGRAMS_SEQ_B:
        if ((mm_action == selection_dwn) ||
            (mm_action == selection_up)) 
        {
            if ((mm_action == selection_dwn) &&
                (last_seq > MIN_PROGRAM_SEQ))
                last_seq--;

            if ((mm_action == selection_up) &&
                (last_seq < MAX_PROGRAM_SEQ))
                last_seq++;

            MainMenu_BlankOptions();
            sprintf(s_temp, "P seq: %2d", last_seq);
            set_option_string4(s_temp);
            MainMenu_SetOptions(0);
            mm_changed = 1;
        }

        if (mm_action == selection_enter)
            mmenu_state = MAIN_MENU_PAGE_PROGRAMS_A;

        break;

    case MAIN_MENU_PAGE_PROGRAMS_BACK:
        resp = MainMenu_CheckFree(mm_action);
        if (resp == resp_ok)
        {
            mmenu_state = MAIN_MENU_INIT;
            resp = resp_continue;
        }
        break;
        
        ////////////////////////////
        // HARDWARE CONFIGURATION //
        ////////////////////////////
    case MAIN_MENU_PAGE_HARDWARE_A:
        resp = MainMenu_CheckFree(mm_action);

        if (resp == resp_ok)
        {
            // Menu Title
            MainMenu_SetTitle(s_hardware_title);

            // Menu options
            MainMenu_BlankOptions();
            set_option_string1("Nothing to do here");
            set_option_string2("Back!");
            blank_option_string3();
            blank_option_string4();
            blank_option_string5();
            blank_option_string6();

            mm_selected = 2;
            MainMenu_SetOptions(mm_selected);

            mm_changed = 1;
        
            mmenu_state = MAIN_MENU_PAGE_HARDWARE_B;
            resp = resp_continue;
        }
        break;

    case MAIN_MENU_PAGE_HARDWARE_B:
        if (mm_action == selection_enter)
            mmenu_state = MAIN_MENU_PAGE_HARDWARE_BACK;
        
        break;

    case MAIN_MENU_PAGE_HARDWARE_BACK:
        resp = MainMenu_CheckFree(mm_action);
        if (resp == resp_ok)
        {
            mmenu_state = MAIN_MENU_INIT;
            resp = resp_continue;
        }
        break;

        
        //////////
        // EXIT //
        //////////
    case MAIN_MENU_PAGE_EXIT:
        resp = MainMenu_CheckFree(mm_action);
        if (resp == resp_ok)
        {
            mmenu_state = MAIN_MENU_INIT;
            resp = resp_finish;
        }
        break;

        /////////////////
        // SAVE & EXIT //
        /////////////////
    case MAIN_MENU_PAGE_SAVE_AND_EXIT:
        resp = MainMenu_CheckFree(mm_action);
        if (resp == resp_ok)
        {
            mmenu_state = MAIN_MENU_INIT;
            resp = resp_need_to_save;
        }
        break;
        
    default:
        mmenu_state = MAIN_MENU_INIT;
        break;
    }

    if (mm_changed)
        display_update();

    return resp;
}


unsigned char check_free = 0;
resp_t MainMenu_CheckFree (sw_actions_t a)
{
    resp_t resp = resp_continue;

    switch (check_free)
    {
    case 0:
        display_clear();
        MainMenu_SetTitle("      Selected!");
        display_update();
        check_free++;
        break;
        
    case 1:
        if (a == do_nothing)
        {
            display_clear();
            resp = resp_ok;
            check_free = 0;
        }
        break;
        
    default:
        check_free = 0;
        break;
    }

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
    char displaced [ALL_LINE_LENGTH] = { 0 };
    char * p_s_options;

    gfx_setCursor(0, 0);
    gfx_setTextWrap(0);
    gfx_setTextSize(1);
    gfx_setTextColor(1);    

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
            sprintf(displaced, "%s", p_s_options);
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
    gfx_fillRect(0, 0, 127, 8, 0);        
    // set_option_string1(s_blankl);    
    // blank_option_string1();    
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
