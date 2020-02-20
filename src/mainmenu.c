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
#include <string.h>
#include <stdio.h>

// Externals -------------------------------------------------------------------

// Globals ---------------------------------------------------------------------
typedef enum {
    MAIN_MENU_INIT = 0,

    MAIN_MENU_PAGE_MM_A,
    MAIN_MENU_PAGE_MM_B,

    MAIN_MENU_PAGE_MASTER_A,
    MAIN_MENU_PAGE_MASTER_B,

    MAIN_MENU_PAGE_SLAVE_A,
    MAIN_MENU_PAGE_SLAVE_B,
    
    MAIN_MENU_PAGE_PROGRAMS_A,
    MAIN_MENU_PAGE_PROGRAMS_B,

    MAIN_MENU_PAGE_HARDWARE_A,
    MAIN_MENU_PAGE_HARDWARE_B
    
} mmenu_state_t;

static mmenu_state_t mmenu_state = MAIN_MENU_INIT;

unsigned char mm_selected = 0;
char s_opt [6][18] = { 0 };


// Module Private Functions ----------------------------------------------------
void MainMenu_SetTitle (char *);
void MainMenu_SetOptions (unsigned char);

#define set_option_string1(X)    strcpy(&s_opt[0][0], X)
#define set_option_string2(X)    strcpy(&s_opt[1][0], X)
#define set_option_string3(X)    strcpy(&s_opt[2][0], X)
#define set_option_string4(X)    strcpy(&s_opt[3][0], X)
#define set_option_string5(X)    strcpy(&s_opt[4][0], X)
#define set_option_string6(X)    strcpy(&s_opt[5][0], X)

// Module Funtions -------------------------------------------------------------
void MainMenu_Init (void)
{
    mmenu_state = MAIN_MENU_INIT;
}


resp_t MainMenu_Update (mm_action_t mm_action)
{
    resp_t resp = resp_continue;
    unsigned char mm_changed = 0;

    switch (mmenu_state)
    {
    case MAIN_MENU_INIT:
        display_init(I2C_ADDRESS_SLV);    //TODO: mejorar esto que no tenga que conocer la direccion
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
        mm_selected = 1;
        MainMenu_SetOptions(mm_selected);

        mm_changed = 1;
        
        mmenu_state = MAIN_MENU_PAGE_MM_B;
        break;

    case MAIN_MENU_PAGE_MM_B:
        if (mm_action == selection_up)
        {
            if (mm_selected < 4)
                mm_selected++;
            else
                mm_selected = 1;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_dwn)
        {
            if (mm_selected > 1)
                mm_selected--;
            else
                mm_selected = 4;

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
            }            
        }

        if (mm_action == selection_back)
            resp = resp_finish;
        
        break;

    case MAIN_MENU_PAGE_MASTER_A:
        // Menu Title
        MainMenu_SetTitle("    Master Mode");

        // Menu options
        set_option_string1("Send DMX to slvs");
        set_option_string2("Set program");
        set_option_string3("Set program seq.");
        set_option_string4("");
        set_option_string5("");
        set_option_string6("");
        mm_selected = 1;
        MainMenu_SetOptions(mm_selected);

        mm_changed = 1;
        
        mmenu_state = MAIN_MENU_PAGE_MASTER_B;
        break;

    case MAIN_MENU_PAGE_MASTER_B:
        if (mm_action == selection_up)
        {
            if (mm_selected < 4)
                mm_selected++;
            else
                mm_selected = 1;

            MainMenu_SetOptions(mm_selected);
            mm_changed = 1;
        }

        if (mm_action == selection_dwn)
        {
            if (mm_selected > 1)
                mm_selected--;
            else
                mm_selected = 4;

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
            }            
        }

        if (mm_action == selection_back)
            resp = resp_finish;
        
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
    // title position, color and size
    gfx_setCursor(0, 0);
    gfx_setTextSize(1);
    gfx_setTextColor(1);

    gfx_println(title);
    gfx_drawLine(0,8,127,8,1);
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


//--- end of file ---//
