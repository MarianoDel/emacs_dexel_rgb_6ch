//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MASTER_SLAVE_MENU.C #######################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "master_slave_menu.h"
#include "display_utils.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    MASTER_SLAVE_MENU_INIT = 0,
    MASTER_SLAVE_MENU_CHECK_OPTIONS,
    MASTER_SLAVE_MENU_SELECTED,
    MASTER_SLAVE_MENU_WAIT_FREE
    
} master_slave_menu_state_e;


// variables re-use
#define ms_state    mode_state
#define ms_selected    mode_selected
#define ms_need_display_update    mode_need_display_update
#define ms_opt_lines    mode_opt_lines


// Externals -------------------------------------------------------------------
extern unsigned char mode_state;
extern unsigned char mode_selected;
extern unsigned char mode_need_display_update;
extern char * mode_opt_lines [];


// Globals ---------------------------------------------------------------------
const char ms_line1 [] = {"MASTER"};
const char ms_line2 [] = {"SLAVE"};



// Module Private Functions ----------------------------------------------------


// Module Funtions -------------------------------------------------------------
void MasterSlaveMenuReset (void)
{
    ms_state = MASTER_SLAVE_MENU_INIT;
}


resp_t MasterSlaveMenu (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;

    switch (ms_state)
    {
    case MASTER_SLAVE_MENU_INIT:
        Display_StartLines ();
        Display_ClearLines();

        ms_opt_lines[0] = ms_line1;
        ms_opt_lines[1] = ms_line2;
        
        Display_SetLine1(ms_opt_lines[0]);
        Display_SetLine2(ms_opt_lines[1]);
        // Display_SetLine3(ms_opt_lines[2]);
        // Display_SetLine4(ms_opt_lines[3]);
        // Display_SetLine5(ms_opt_lines[4]);

        // Display_SetLine8("            Master_Slave Menu");
        Display_SetLine8("    Master/Slave Menu");        

        ms_selected = 1;
        Display_SetOptions(ms_selected, ms_opt_lines[ms_selected - 1]);

        ms_need_display_update = 1;
        ms_state++;
        break;

    case MASTER_SLAVE_MENU_CHECK_OPTIONS:

        if (actions == selection_dwn)
        {
            if (ms_selected > 1)
            {
                Display_BlankLine(ms_selected);
                Display_SetLine(ms_selected, ms_opt_lines[ms_selected - 1]);
                ms_selected--;
                Display_SetOptions(ms_selected, ms_opt_lines[ms_selected - 1]);
                ms_need_display_update = 1;
            }
        }
        
        if (actions == selection_up)
        {
            if (ms_selected < 5)
            {
                Display_BlankLine(ms_selected);
                Display_SetLine(ms_selected, ms_opt_lines[ms_selected - 1]);
                ms_selected++;
                Display_SetOptions(ms_selected, ms_opt_lines[ms_selected - 1]);
                ms_need_display_update = 1;
            }
        }
        
        if (actions == selection_enter)
        {
            switch (ms_selected)
            {
            case 1:
                mem->program_inner_type = MASTER_NO_INNER_MODE;
                break;

            case 2:
                mem->program_inner_type = MASTER_INNER_SLAVE;
                break;

            }
            ms_state++;
        }
        break;

    case MASTER_SLAVE_MENU_SELECTED:
        ms_state++;
        break;

    case MASTER_SLAVE_MENU_WAIT_FREE:
        resp = resp_finish;
        ms_state = MASTER_SLAVE_MENU_INIT;
        break;
        
    default:
        ms_state = MASTER_SLAVE_MENU_INIT;
        break;
    }

    if (ms_need_display_update)
    {
        display_update();
        ms_need_display_update = 0;
    }

    return resp;
    
}


//--- end of file ---//
