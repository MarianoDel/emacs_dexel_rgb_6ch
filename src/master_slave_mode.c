//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MASTER_SLAVE_MODE.C #######################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "master_slave_mode.h"
#include "master_slave_menu.h"
#include "master_menu.h"
#include "slave_menu.h"
#include "fixed_menu.h"
#include "colors_menu.h"
#include "display_utils.h"

#include "colors_functions.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    MASTER_SLAVE_MODE_INIT = 0,
    MASTER_SLAVE_MODE_CHECK_INNER_MODE_0,
    MASTER_SLAVE_MODE_CHECK_INNER_MODE_1,    
    MASTER_SLAVE_MODE_GLOBAL_MENU,
    MASTER_SLAVE_MODE_IN_COLORS_FIXED,
    MASTER_SLAVE_MODE_IN_COLORS_SKIPPING,
    MASTER_SLAVE_MODE_IN_COLORS_GRADUAL,
    MASTER_SLAVE_MODE_IN_COLORS_STROBE,
    MASTER_SLAVE_MODE_IN_SLAVE

    
} master_slave_mode_state_e;

// variables re-use
#define master_slave_state    mode_state

// Externals -------------------------------------------------------------------
extern unsigned char mode_state;

// Globals ---------------------------------------------------------------------
volatile unsigned short master_effect_timer = 0;

// Module Private Functions ----------------------------------------------------


// Module Funtions -------------------------------------------------------------
void MasterSlaveMode_UpdateTimer (void)
{
    //TODO: ESTO ESTA LLAMANDO DOS VECES A LA MISMA VARIABLE DE TIMER
    //DESCUENTA DE A 2!!!!!
    FixedMenu_UpdateTimer();

    ColorsMenu_UpdateTimer();

    if (master_effect_timer)
        master_effect_timer--;
}


void MasterSlaveModeReset (void)
{
    master_slave_state = MASTER_SLAVE_MODE_INIT;
}


resp_t MasterSlaveMode (parameters_typedef * mem, sw_actions_t actions)
{
    resp_t resp = resp_continue;

    unsigned char * ch_val;

    switch (master_slave_state)
    {
    case MASTER_SLAVE_MODE_INIT:

        switch (mem->program_inner_type)
        {
        case MASTER_INNER_FIXED_MODE:
            FixedMenuReset();
            master_slave_state = MASTER_SLAVE_MODE_IN_COLORS_FIXED;
            break;

        case MASTER_INNER_SKIPPING_MODE:
            ColorsMenuReset();
            master_slave_state = MASTER_SLAVE_MODE_IN_COLORS_SKIPPING;
            break;
            
        case MASTER_INNER_GRADUAL_MODE:
            ColorsMenuReset();
            master_slave_state = MASTER_SLAVE_MODE_IN_COLORS_GRADUAL;
            break;
            
        case MASTER_INNER_STROBE_MODE:
            ColorsMenuReset();
            master_slave_state = MASTER_SLAVE_MODE_IN_COLORS_STROBE;
            break;

        case MASTER_INNER_SLAVE:
            master_slave_state = MASTER_SLAVE_MODE_IN_SLAVE;
            break;
            
        default:
            MasterSlaveMenuReset ();
            master_slave_state = MASTER_SLAVE_MODE_CHECK_INNER_MODE_0;
            break;
        }
        break;

    case MASTER_SLAVE_MODE_CHECK_INNER_MODE_0:
        resp = MasterSlaveMenu (mem, actions);

        if (resp == resp_finish)
        {
            if (mem->program_inner_type == MASTER_INNER_SLAVE)
            {
                SlaveMenuReset();
                master_slave_state = MASTER_SLAVE_MODE_INIT;
            }
            else
            {
                MasterMenuReset();
                master_slave_state++;
            }
        }
        break;

    case MASTER_SLAVE_MODE_CHECK_INNER_MODE_1:
        resp = MasterMenu (mem, actions);

        if (resp == resp_finish)
            master_slave_state = MASTER_SLAVE_MODE_INIT;

        break;
        
    case MASTER_SLAVE_MODE_IN_COLORS_FIXED:
        
        //resp_change goes strait up, resp_finish end of this mode
        resp = FixedMenu(mem, actions);    

        if (resp == resp_finish)
        {
            mem->program_inner_type = MASTER_NO_INNER_MODE;
            master_slave_state = MASTER_SLAVE_MODE_INIT;
            resp = resp_continue;
        }
        break;

    case MASTER_SLAVE_MODE_IN_COLORS_SKIPPING:

        //resp_change do nothing, resp_finish end of this mode
        resp = ColorsMenu (mem, actions);

        if (resp == resp_finish)
        {
            mem->program_inner_type = MASTER_NO_INNER_MODE;
            master_slave_state = MASTER_SLAVE_MODE_INIT;
            resp = resp_continue;
            break;
        }

        if (!master_effect_timer)
        {
            ch_val = mem->fixed_channels;

            resp = Colors_Fading_Pallete (ch_val);
            if (resp == resp_finish)
                resp = resp_continue;

            master_effect_timer = 10 - mem->program_inner_type_speed;
            resp = resp_change;
        }
        break;

    case MASTER_SLAVE_MODE_IN_COLORS_GRADUAL:

        //resp_change do nothing, resp_finish end of this mode
        resp = ColorsMenu (mem, actions);

        if (resp == resp_finish)
        {
            mem->program_inner_type = MASTER_NO_INNER_MODE;
            master_slave_state = MASTER_SLAVE_MODE_INIT;
            resp = resp_continue;
            break;
        }

        if (!master_effect_timer)
        {
            ch_val = mem->fixed_channels;

            resp = Colors_Fading_Shuffle_Pallete (ch_val);
            if (resp == resp_finish)
                resp = resp_continue;

            master_effect_timer = 10 - mem->program_inner_type_speed;
            resp = resp_change;
        }

        break;

    case MASTER_SLAVE_MODE_IN_COLORS_STROBE:

        //resp_change do nothing, resp_finish end of this mode
        resp = ColorsMenu (mem, actions);

        if (resp == resp_finish)
        {
            mem->program_inner_type = MASTER_NO_INNER_MODE;
            master_slave_state = MASTER_SLAVE_MODE_INIT;
            resp = resp_continue;
            break;
        }

        if (!master_effect_timer)
        {
            ch_val = mem->fixed_channels;

            resp = Colors_Strobe_Pallete (ch_val);
            if (resp == resp_finish)
                resp = resp_continue;

            master_effect_timer = 2000 - mem->program_inner_type_speed * 200;
            resp = resp_change;
        }
        

        break;

    case MASTER_SLAVE_MODE_IN_SLAVE:
        resp = SlaveMenu (mem, actions);

        if (resp == resp_finish)
        {
            mem->program_inner_type = MASTER_NO_INNER_MODE;
            master_slave_state = MASTER_SLAVE_MODE_INIT;
            resp = resp_continue;
        }
        break;
        
    default:
        master_slave_state = MASTER_SLAVE_MODE_INIT;
        break;
    }

    return resp;
    
}


//--- end of file ---//
