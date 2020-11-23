//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### RESET_MODE.C ##############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "reset_mode.h"
#include "options_menu.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    RESET_MODE_INIT = 0,
    RESET_MODE_SELECT
    
} reset_mode_state_e;

// variables re-use
#define reset_state    mode_state

// Externals -------------------------------------------------------------------
extern unsigned char mode_state;
extern options_menu_st mem_options;

// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Funtions -------------------------------------------------------------
void ResetModeReset (void)
{
    reset_state = RESET_MODE_INIT;
}


resp_t ResetMode (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;

    switch (reset_state)
    {
    case RESET_MODE_INIT:
        mem_options.argv[0] = "EXIT";
        mem_options.argv[1] = "FACTORY DEFAULTS";
        mem_options.options_qtty = 2;
        mem_options.argv[7] = "           Reset Mode";
        OptionsMenuReset();

        reset_state++;
        break;

    case RESET_MODE_SELECT:
        resp = OptionsMenu(&mem_options, actions);

        if (resp == resp_finish)
        {
            switch (mem_options.options_selected)
            {
            case 0:
                // do nothing here
                break;

            case 1:
                ResetMode_Factory_Defaults(mem);
                break;

            default:
                resp = resp_continue;
                reset_state = RESET_MODE_INIT;
                break;
                
            }
        }
        break;
            
    default:
        reset_state = RESET_MODE_INIT;
        break;
    }

    return resp;
    
}


void ResetMode_Factory_Defaults (parameters_typedef * mem)
{
    //memory empty use some defaults
    mem->program_type = DMX1_MODE;
    mem->master_send_dmx_enable = 0;
    mem->program_inner_type = MANUAL_NO_INNER_MODE;
    mem->program_inner_type_speed = 0;
    mem->dmx_first_channel = 1;
    mem->dmx_channel_quantity = 6;
    mem->max_power = 200;

}


//--- end of file ---//
