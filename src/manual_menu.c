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
#include "options_menu.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    MANUAL_MENU_INIT = 0,
    MANUAL_MENU_SELECT
    
} manual_menu_state_e;


// variables re-use
#define manual_menu_state    menu_state

// Externals -------------------------------------------------------------------
extern unsigned char menu_state;
extern options_menu_st mem_options;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Funtions -------------------------------------------------------------
void ManualMenuReset (void)
{
    manual_menu_state = MANUAL_MENU_INIT;
}


resp_t ManualMenu (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;

    switch (manual_menu_state)
    {
    case MANUAL_MENU_INIT:
        mem_options.argv[0] = "MANUAL";
        mem_options.argv[1] = "COLORS SKIPPING";
        mem_options.argv[2] = "COLORS GRADUAL";
        mem_options.argv[3] = "COLORS STROBE";
        mem_options.options_qtty = 4;
        mem_options.argv[7] = "          Manual Mode";
        OptionsMenuReset();

        manual_menu_state++;        
        break;

    case MANUAL_MENU_SELECT:
        resp = OptionsMenu(&mem_options, actions);

        if (resp == resp_finish)
        {
            switch (mem_options.options_selected)
            {
            case 0:
                mem->program_inner_type = MANUAL_INNER_FIXED_MODE;
                break;

            case 1:
                mem->program_inner_type = MANUAL_INNER_SKIPPING_MODE;
                break;

            case 2:
                mem->program_inner_type = MANUAL_INNER_GRADUAL_MODE;
                break;

            case 3:
                mem->program_inner_type = MANUAL_INNER_STROBE_MODE;
                break;

            default:
                resp = resp_continue;
                mem->program_inner_type = MANUAL_NO_INNER_MODE;                
                manual_menu_state = MANUAL_MENU_INIT;
                break;
                
            }
        }
        break;

    default:
        manual_menu_state = MANUAL_MENU_INIT;
        break;
    }

    return resp;
    
}


//--- end of file ---//
