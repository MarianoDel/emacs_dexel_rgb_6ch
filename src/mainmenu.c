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
#include "display_utils.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    MAIN_MENU_INIT = 0,

    MAIN_MENU_CHECK_OPTIONS,
    MAIN_MENU_SELECTED,
    MAIN_MENU_WAIT_FREE
    
} main_menu_state_e;


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
static main_menu_state_e mm_state = MAIN_MENU_INIT;
unsigned char mm_selected = 0;
unsigned char mm_need_display_update = 0;

const char mm_line1 [] = {"DMX1"};
const char mm_line2 [] = {"DMX2"};
const char mm_line3 [] = {"MASTER/SLAVE"};
const char mm_line4 [] = {"MANUAL"};
const char mm_line5 [] = {"RESET"};

const char * opt_lines [5];
// Module Private Functions ----------------------------------------------------


// Module Funtions -------------------------------------------------------------
void MainMenuReset (void)
{
    mm_state = MAIN_MENU_INIT;
}


resp_t MainMenu (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;

    switch (mm_state)
    {
    case MAIN_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();

        opt_lines[0] = mm_line1;
        opt_lines[1] = mm_line2;
        opt_lines[2] = mm_line3;
        opt_lines[3] = mm_line4;
        opt_lines[4] = mm_line5;
        
        Display_SetLine1(opt_lines[0]);
        Display_SetLine2(opt_lines[1]);
        Display_SetLine3(opt_lines[2]);
        Display_SetLine4(opt_lines[3]);
        Display_SetLine5(opt_lines[4]);

        Display_SetLine8("            Main Menu");        

        mm_selected = 1;
        Display_SetOptions(mm_selected, opt_lines[mm_selected - 1]);

        mm_need_display_update = 1;
        mm_state++;
        break;

    case MAIN_MENU_CHECK_OPTIONS:

        if (actions == selection_dwn)
        {
            if (mm_selected > 1)
            {
                Display_BlankLine(mm_selected);
                Display_SetLine(mm_selected, opt_lines[mm_selected - 1]);
                mm_selected--;
                Display_SetOptions(mm_selected, opt_lines[mm_selected - 1]);
                mm_need_display_update = 1;
            }
        }
        
        if (actions == selection_up)
        {
            if (mm_selected < 5)
            {
                Display_BlankLine(mm_selected);
                Display_SetLine(mm_selected, opt_lines[mm_selected - 1]);
                mm_selected++;
                Display_SetOptions(mm_selected, opt_lines[mm_selected - 1]);
                mm_need_display_update = 1;
            }
        }
        
        if (actions == selection_enter)
        {
            switch (mm_selected)
            {
            case 1:
                mem->program_type = DMX1_MODE;
                break;

            case 2:
                mem->program_type = DMX2_MODE;
                break;

            case 3:
                mem->program_type = MASTER_SLAVE_MODE;
                break;

            case 4:
                mem->program_type = MANUAL_MODE;
                break;

            case 5:
                mem->program_type = RESET_MODE;
                break;
            }
            mm_state++;
        }
        break;

    case MAIN_MENU_SELECTED:
        mm_state++;
        break;

    case MAIN_MENU_WAIT_FREE:
        resp = resp_finish;
        mm_state = MAIN_MENU_INIT;
        break;
        
    default:
        mm_state = MAIN_MENU_INIT;
        break;
    }

    if (mm_need_display_update)
    {
        display_update();
        mm_need_display_update = 0;
    }

    return resp;
    
}


//--- end of file ---//
