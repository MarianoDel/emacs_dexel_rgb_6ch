//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MASTER_MENU.C #############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "master_menu.h"
#include "options_menu.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    MASTER_MENU_INIT = 0,
    MASTER_MENU_SELECT
    
} master_menu_state_e;


// variables re-use
#define master_menu_state    menu_state


// Externals -------------------------------------------------------------------
extern unsigned char menu_state;
extern options_menu_st mem_options;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Funtions -------------------------------------------------------------
void MasterMenuReset (void)
{
    master_menu_state = MASTER_MENU_INIT;
}


resp_t MasterMenu (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;

    switch (master_menu_state)
    {
    case MASTER_MENU_INIT:
        mem_options.argv[0] = "MANUAL";
        mem_options.argv[1] = "COLORS SKIPPING";
        mem_options.argv[2] = "COLORS GRADUAL";
        mem_options.argv[3] = "COLORS STROBE";
        mem_options.argv[4] = "EXIT";
        mem_options.options_qtty = 5;
        mem_options.argv[7] = "          Master Mode";        
        OptionsMenuReset();

        master_menu_state++;
        break;

    case MASTER_MENU_SELECT:
        resp = OptionsMenu(&mem_options, actions);

        if (resp == resp_finish)
        {
            switch (mem_options.options_selected)
            {
            case 0:
                mem->program_inner_type = MASTER_INNER_FIXED_MODE;
                break;

            case 1:
                mem->program_inner_type = MASTER_INNER_SKIPPING_MODE;
                break;

            case 2:
                mem->program_inner_type = MASTER_INNER_GRADUAL_MODE;
                break;

            case 3:
                mem->program_inner_type = MASTER_INNER_STROBE_MODE;
                break;

            case 4:
                mem->program_inner_type = MASTER_NO_INNER_MODE;
                break;
                
            default:
                resp = resp_continue;
                master_menu_state = MASTER_MENU_INIT;
                break;
            }
        }
        break;

    default:
        master_menu_state = MASTER_MENU_INIT;
        break;
    }

    return resp;
    
}


//--- end of file ---//
