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
#include "options_menu.h"
// #include "display_utils.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    MASTER_SLAVE_MENU_INIT = 0,
    MASTER_SLAVE_MENU_SELECT
    
} master_slave_menu_state_e;


// variables re-use
#define ms_state    menu_state


// Externals -------------------------------------------------------------------
extern unsigned char menu_state;
extern options_menu_st mem_options;


// Globals ---------------------------------------------------------------------


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
        mem_options.argv[0] = "MASTER";
        mem_options.argv[1] = "SLAVE";
        mem_options.options_qtty = 2;
        mem_options.argv[7] = "    Master/Slave Mode";
        OptionsMenuReset();

        ms_state++;
        break;

    case MASTER_SLAVE_MENU_SELECT:
        resp = OptionsMenu(&mem_options, actions);

        if (resp == resp_finish)
        {
            switch (mem_options.options_selected)
            {
            case 0:
                mem->program_inner_type = MASTER_NO_INNER_MODE;
                break;

            case 1:
                mem->program_inner_type = MASTER_INNER_SLAVE;
                break;

            default:
                resp = resp_continue;
                ms_state = MASTER_SLAVE_MENU_INIT;
                break;
            }
        }
        break;

    default:
        ms_state = MASTER_SLAVE_MENU_INIT;
        break;
    }

    return resp;
    
}


//--- end of file ---//
