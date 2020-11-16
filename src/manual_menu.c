//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MANUAL_MENU.C #############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "manual_menu.h"
#include "display_utils.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    MANUAL_MENU_INIT = 0,
    MANUAL_MENU_CHECK_OPTIONS,
    MANUAL_MENU_SELECTED,
    MANUAL_MENU_WAIT_FREE
    
} manual_menu_state_e;


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
static manual_menu_state_e manual_state = MANUAL_MENU_INIT;
unsigned char manual_selected = 0;
unsigned char manual_need_display_update = 0;

const char line1 [] = {"MANUAL"};
const char line2 [] = {"COLORS SKIPPING"};
const char line3 [] = {"COLORS GRADUAL"};
const char line4 [] = {"COLORS STROBE"};

const char * opt_lines [4];
// Module Private Functions ----------------------------------------------------


// Module Funtions -------------------------------------------------------------
void ManualMenuReset (void)
{
    manual_state = MANUAL_MENU_INIT;
}


resp_t ManualMenu (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;

    switch (manual_state)
    {
    case MANUAL_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();

        opt_lines[0] = line1;
        opt_lines[1] = line2;
        opt_lines[2] = line3;
        opt_lines[3] = line4;
        
        Display_SetLine1(opt_lines[0]);
        Display_SetLine2(opt_lines[1]);
        Display_SetLine3(opt_lines[2]);
        Display_SetLine4(opt_lines[3]);

        if (mem->program_type == MANUAL_MODE)
        {
            Display_SetLine8("          Manual Menu");
        }

        if (mem->program_type == MASTER_SLAVE_MODE)
        {
            Display_SetLine8("          Master Menu");
        }

        manual_selected = 1;
        Display_SetOptions(manual_selected, opt_lines[manual_selected - 1]);

        manual_need_display_update = 1;
        manual_state++;
        break;

    case MANUAL_MENU_CHECK_OPTIONS:

        if (actions == selection_dwn)
        {
            if (manual_selected > 1)
            {
                Display_BlankLine(manual_selected);
                Display_SetLine(manual_selected, opt_lines[manual_selected - 1]);
                manual_selected--;
                Display_SetOptions(manual_selected, opt_lines[manual_selected - 1]);
                manual_need_display_update = 1;
            }
        }
        
        if (actions == selection_up)
        {
            if (manual_selected < 4)
            {
                Display_BlankLine(manual_selected);
                Display_SetLine(manual_selected, opt_lines[manual_selected - 1]);
                manual_selected++;
                Display_SetOptions(manual_selected, opt_lines[manual_selected - 1]);
                manual_need_display_update = 1;
            }
        }
        
        if (actions == selection_enter)
        {
            if (mem->program_type == MANUAL_MODE)
            {
                switch (manual_selected)
                {
                case 1:
                    mem->program_type = MANUAL_FIXED_MODE;
                    break;

                case 2:
                    mem->program_type = MANUAL_SKIPPING_MODE;
                    break;

                case 3:
                    mem->program_type = MANUAL_GRADUAL_MODE;
                    break;

                case 4:
                    mem->program_type = MANUAL_STROBE_MODE;
                    break;
                }
            }
            
            if (mem->program_type == MASTER_SLAVE_MODE)
            {
                switch (manual_selected)
                {
                case 1:
                    mem->program_type = MASTER_FIXED_MODE;
                    break;

                case 2:
                    mem->program_type = MASTER_SKIPPING_MODE;
                    break;

                case 3:
                    mem->program_type = MASTER_GRADUAL_MODE;
                    break;

                case 4:
                    mem->program_type = MASTER_STROBE_MODE;
                    break;
                }
            }
            
            manual_state++;
        }
        break;

    case MANUAL_MENU_SELECTED:
        manual_state++;
        break;

    case MANUAL_MENU_WAIT_FREE:
        resp = resp_finish;
        manual_state = MANUAL_MENU_INIT;
        break;
        
    default:
        manual_state = MANUAL_MENU_INIT;
        break;
    }

    if (manual_need_display_update)
    {
        display_update();
        manual_need_display_update = 0;
    }

    return resp;
    
}


//--- end of file ---//
