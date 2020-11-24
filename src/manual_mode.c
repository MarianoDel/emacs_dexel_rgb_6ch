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

#include "colors_functions.h"

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

// variables re-use
#define manual_state    mode_state
#define manual_effect_timer    mode_effect_timer

// Externals -------------------------------------------------------------------
extern unsigned char mode_state;
extern volatile unsigned short mode_effect_timer;


// Globals ---------------------------------------------------------------------
void (* ptFManualMenuTT ) (void) = NULL;


// Module Private Functions ----------------------------------------------------


// Module Funtions -------------------------------------------------------------
void ManualMode_UpdateTimers (void)
{
    if (ptFManualMenuTT != NULL)
        ptFManualMenuTT();
    
    if (manual_effect_timer)
        manual_effect_timer--;
}


void ManualModeReset (void)
{
    manual_state = MANUAL_MODE_INIT;
}


resp_t ManualMode (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;
    unsigned char * ch_val;

    switch (manual_state)
    {
    case MANUAL_MODE_INIT:

        switch (mem->program_inner_type)
        {
        case MANUAL_INNER_FIXED_MODE:
            ptFManualMenuTT = &FixedMenu_UpdateTimer;
            FixedMenuReset();
            manual_state = MANUAL_MODE_IN_COLORS_FIXED;
            break;

        case MANUAL_INNER_SKIPPING_MODE:
            ptFManualMenuTT = &ColorsMenu_UpdateTimer;
            ColorsMenuReset();
            manual_state = MANUAL_MODE_IN_COLORS_SKIPPING;
            break;
            
        case MANUAL_INNER_GRADUAL_MODE:
            ptFManualMenuTT = &ColorsMenu_UpdateTimer;            
            ColorsMenuReset();
            manual_state = MANUAL_MODE_IN_COLORS_GRADUAL;
            break;
            
        case MANUAL_INNER_STROBE_MODE:
            ptFManualMenuTT = &ColorsMenu_UpdateTimer;
            ColorsMenuReset();
            manual_state = MANUAL_MODE_IN_COLORS_STROBE;
            break;

        default:
            ptFManualMenuTT = NULL;
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
        
        //resp_change goes strait up, resp_finish end of this mode
        resp = FixedMenu(mem, actions);    

        if (resp == resp_finish)
        {
            mem->program_inner_type = MANUAL_NO_INNER_MODE;
            manual_state = MANUAL_MODE_INIT;
            resp = resp_continue;
        }
        break;

    case MANUAL_MODE_IN_COLORS_SKIPPING:

        //resp_change do nothing, resp_finish end of this mode
        resp = ColorsMenu (mem, actions);

        if (resp == resp_finish)
        {
            mem->program_inner_type = MANUAL_NO_INNER_MODE;
            manual_state = MANUAL_MODE_INIT;
            resp = resp_continue;
            break;
        }

        if (!manual_effect_timer)
        {
            ch_val = mem->fixed_channels;

            resp = Colors_Fading_Pallete (ch_val);
            if (resp == resp_finish)
                resp = resp_continue;

            manual_effect_timer = 10 - mem->program_inner_type_speed;
            resp = resp_change;
        }
        break;

    case MANUAL_MODE_IN_COLORS_GRADUAL:

        //resp_change do nothing, resp_finish end of this mode
        resp = ColorsMenu (mem, actions);

        if (resp == resp_finish)
        {
            mem->program_inner_type = MANUAL_NO_INNER_MODE;
            manual_state = MANUAL_MODE_INIT;
            resp = resp_continue;
            break;
        }

        if (!manual_effect_timer)
        {
            ch_val = mem->fixed_channels;

            resp = Colors_Fading_Shuffle_Pallete (ch_val);
            if (resp == resp_finish)
                resp = resp_continue;

            manual_effect_timer = 10 - mem->program_inner_type_speed;
            resp = resp_change;
        }

        break;

    case MANUAL_MODE_IN_COLORS_STROBE:

        //resp_change do nothing, resp_finish end of this mode
        resp = ColorsMenu (mem, actions);

        if (resp == resp_finish)
        {
            mem->program_inner_type = MANUAL_NO_INNER_MODE;
            manual_state = MANUAL_MODE_INIT;
            resp = resp_continue;
            break;
        }

        if (!manual_effect_timer)
        {
            ch_val = mem->fixed_channels;

            resp = Colors_Strobe_Pallete (ch_val);
            if (resp == resp_finish)
                resp = resp_continue;

            manual_effect_timer = 2000 - mem->program_inner_type_speed * 200;
            resp = resp_change;
        }
        

        break;
        
    default:
        manual_state = MANUAL_MODE_INIT;
        break;
    }

    return resp;
    
}


//--- end of file ---//
