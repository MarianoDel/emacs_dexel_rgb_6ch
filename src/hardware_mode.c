//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARDWARE_MODE.C ###########################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "hardware_mode.h"
#include "options_menu.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    HARDWARE_MODE_INIT = 0,
    HARDWARE_MODE_SELECT,
    HARDWARE_MODE_CURRENTS,
    HARDWARE_MODE_LIMIT,
    HARDWARE_MODE_CHANNELS,
    HARDWARE_MODE_TEMP,
    HARDWARE_MODE_VERSION
    
} hardware_mode_state_e;

// variables re-use
#define hardware_mode_state    mode_state

// Externals -------------------------------------------------------------------
extern unsigned char mode_state;
extern options_menu_st mem_options;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Funtions -------------------------------------------------------------
void HardwareModeReset (void)
{
    hardware_mode_state = HARDWARE_MODE_INIT;
}


resp_t HardwareMode (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;

    switch (hardware_mode_state)
    {
    case HARDWARE_MODE_INIT:
        mem_options.argv[0] = "CURRENT IN CHANNELS";
        mem_options.argv[1] = "CURRENT LIMIT";
        mem_options.argv[2] = "CHANNELS SELECTION";
        mem_options.argv[3] = "TEMP CONFIG";
        mem_options.argv[4] = "VERSION";
        mem_options.argv[5] = "EXIT";        
        mem_options.options_qtty = 6;
        mem_options.argv[7] = "        Hardware Mode";
        OptionsModeReset();

        hardware_mode_state++;
        break;

    case HARDWARE_MODE_SELECT:
        resp = OptionsMode(&mem_options, actions);
        
        if (resp == resp_finish)
        {
            resp = resp_continue;
            
            switch (mem_options.options_selected)
            {
            case 0:
                hardware_mode_state = HARDWARE_MODE_CURRENTS;
                break;

            case 1:
                hardware_mode_state = HARDWARE_MODE_LIMIT;
                break;

            case 2:
                hardware_mode_state = HARDWARE_MODE_CHANNELS;
                break;

            case 3:
                hardware_mode_state = HARDWARE_MODE_TEMP;
                break;

            case 4:
                hardware_mode_state = HARDWARE_MODE_VERSION;
                break;

            case 5:
                hardware_mode_state = HARDWARE_MODE_INIT;
                resp = resp_finish;
                break;
                
            default:
                hardware_mode_state = HARDWARE_MODE_INIT;
                break;
                
            }
        }
        break;

    case HARDWARE_MODE_CURRENTS:
        break;

    case HARDWARE_MODE_LIMIT:
        break;

    case HARDWARE_MODE_CHANNELS:
        break;

    case HARDWARE_MODE_TEMP:
        break;

    case HARDWARE_MODE_VERSION:
        break;
        
    default:
        hardware_mode_state = HARDWARE_MODE_INIT;
        break;
    }

    return resp;
    
}


//--- end of file ---//
