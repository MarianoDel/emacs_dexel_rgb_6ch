//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MANUAL_MODE.C #############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "manual_mode.h"
#include "manual_menu.h"
#include "fixed_menu.h"
#include "colors_menu.h"
#include "display_utils.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    MANUAL_MODE_INIT = 0,
    MANUAL_MODE_CHECK_INNER_MODE,
    MANUAL_MODE_GLOBAL_MENU,
    MANUAL_MODE_IN_COLORS_FIXED,
    MANUAL_MODE_IN_COLORS_SKIPPING,
    MANUAL_MODE_IN_COLORS_GRADUAL,
    MANUAL_MODE_IN_COLORS_STROBE
    
} manual_mode_state_e;


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
//TODO: hacer esto con defines de variables generales para rehuso
static manual_mode_state_e manual_state = MANUAL_MODE_INIT;
unsigned char manual_selected = 0;
unsigned char manual_need_display_update = 0;

// Module Private Functions ----------------------------------------------------


// Module Funtions -------------------------------------------------------------
void ManualModeReset (void)
{
    manual_state = MANUAL_MODE_INIT;
}


resp_t ManualMode (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;

    switch (manual_state)
    {
    case MANUAL_MODE_INIT:

        switch (mem->program_inner_type)
        {
        case MANUAL_INNER_FIXED_MODE:
            FixedMenuReset();
            manual_state = MANUAL_MODE_IN_COLORS_FIXED;
            break;

        case MANUAL_INNER_SKIPPING_MODE:
            ColorsMenuReset();
            manual_state = MANUAL_MODE_IN_COLORS_SKIPPING;
            break;

        case MANUAL_INNER_GRADUAL_MODE:
            ColorsMenuReset();            
            manual_state = MANUAL_MODE_IN_COLORS_GRADUAL;
            break;

        case MANUAL_INNER_STROBE_MODE:
            ColorsMenuReset();            
            manual_state = MANUAL_MODE_IN_COLORS_STROBE;
            break;

        default:
            ManualMenuReset ();
            manual_state = MANUAL_MODE_CHECK_INNER_MODE;
            break;
        }
        break;

    case MANUAL_MODE_CHECK_INNER_MODE:
        resp = ManualMenu (mem, actions);

        if (resp == resp_finish)
            manual_state = MANUAL_MODE_INIT;

        break;
        
    case MANUAL_MODE_IN_COLORS_FIXED:
        resp = FixedMenu(mem, actions);

        if (resp == resp_finish)
        {
            mem->program_inner_type = MANUAL_NO_INNER_MODE;
            manual_state = MANUAL_MODE_INIT;
            resp = resp_continue;
        }
        break;

    case MANUAL_MODE_IN_COLORS_SKIPPING:
    case MANUAL_MODE_IN_COLORS_GRADUAL:
    case MANUAL_MODE_IN_COLORS_STROBE:        
        
        resp = ColorsMenu (mem, actions);

        if (resp == resp_finish)
        {
            mem->program_type = MANUAL_NO_INNER_MODE;
            manual_state = MANUAL_MODE_INIT;
            resp = resp_continue;
        }
        break;
        
    default:
        manual_state = MANUAL_MODE_INIT;
        break;
    }

    return resp;
    
}


//--- end of file ---//
