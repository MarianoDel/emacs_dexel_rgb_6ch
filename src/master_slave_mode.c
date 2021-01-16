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
#include "dmx_transceiver.h"

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
#define master_effect_timer    mode_effect_timer


// Externals -------------------------------------------------------------------
extern volatile unsigned char data11[];
extern volatile unsigned char Packet_Detected_Flag;

extern unsigned char mode_state;
extern volatile unsigned short mode_effect_timer;


// Globals ---------------------------------------------------------------------
void (* ptFMasterMenuTT ) (void) = NULL;


// Module Private Functions ----------------------------------------------------
void MasterResetInnerMode (parameters_typedef *);

// Module Funtions -------------------------------------------------------------
void MasterSlaveMode_UpdateTimers (void)
{
    if (ptFMasterMenuTT != NULL)
        ptFMasterMenuTT();

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
            ptFMasterMenuTT = &FixedMenu_UpdateTimer;            
            FixedMenuReset();
            master_slave_state = MASTER_SLAVE_MODE_IN_COLORS_FIXED;
            DMX_EnableTx();
            break;

        case MASTER_INNER_SKIPPING_MODE:
            ptFMasterMenuTT = &ColorsMenu_UpdateTimer;
            ColorsMenuReset();
            master_slave_state = MASTER_SLAVE_MODE_IN_COLORS_SKIPPING;
            DMX_EnableTx();            
            break;
            
        case MASTER_INNER_GRADUAL_MODE:
            ptFMasterMenuTT = &ColorsMenu_UpdateTimer;
            ColorsMenuReset();
            master_slave_state = MASTER_SLAVE_MODE_IN_COLORS_GRADUAL;
            DMX_EnableTx();            
            break;
            
        case MASTER_INNER_STROBE_MODE:
            ptFMasterMenuTT = &ColorsMenu_UpdateTimer;
            ColorsMenuReset();
            master_slave_state = MASTER_SLAVE_MODE_IN_COLORS_STROBE;
            DMX_EnableTx();            
            break;

        case MASTER_INNER_SLAVE:
            // ptFMasterMenuTT = &SlaveMenu_UpdateTimer;    //this is not nedded
            SlaveMenuReset();
            master_slave_state = MASTER_SLAVE_MODE_IN_SLAVE;
            DMX_EnableRx();
            break;
            
        default:
            ptFMasterMenuTT = NULL;
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
                master_slave_state = MASTER_SLAVE_MODE_INIT;
                resp = resp_need_to_save;
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
        {
            master_slave_state = MASTER_SLAVE_MODE_INIT;
            resp = resp_need_to_save;
        }
        break;
        
    case MASTER_SLAVE_MODE_IN_COLORS_FIXED:
        
        //resp_change translates to resp_change_all_up in this mode, resp_finish end of this mode
        resp = FixedMenu(mem, actions);

        if (resp == resp_change)
            resp = resp_change_all_up;

        if (resp == resp_finish)
        {
            MasterResetInnerMode(mem);
            resp = resp_change;
        }
        break;

    case MASTER_SLAVE_MODE_IN_COLORS_SKIPPING:

        //resp_change do nothing, resp_finish end of this mode, resp_need_to_save goes straight up
        resp = ColorsMenu (mem, actions);

        if (resp == resp_finish)
        {
            MasterResetInnerMode(mem);            
            resp = resp_change;
            break;
        }

        // speed change, save it
        if (resp == resp_need_to_save)
            break;

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

        //resp_change do nothing, resp_finish end of this mode, resp_need_to_save goes straight up
        resp = ColorsMenu (mem, actions);

        if (resp == resp_finish)
        {
            MasterResetInnerMode(mem);
            resp = resp_change;
            break;
        }

        // speed change, save it
        if (resp == resp_need_to_save)
            break;
        
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

        //resp_change do nothing, resp_finish end of this mode, resp_need_to_save goes straight up
        resp = ColorsMenu (mem, actions);

        if (resp == resp_finish)
        {
            MasterResetInnerMode(mem);
            resp = resp_change;
            break;
        }

        // speed change, save it
        if (resp == resp_need_to_save)
            break;

        if (!master_effect_timer)
        {
            ch_val = mem->fixed_channels;

            resp = Colors_Strobe_Pallete (ch_val);
            if (resp == resp_finish)
                resp = resp_continue;

            // master_effect_timer = 2000 - mem->program_inner_type_speed * 200;
            master_effect_timer = 1000 - mem->program_inner_type_speed * 100;                        
            resp = resp_change;
        }
        break;

    case MASTER_SLAVE_MODE_IN_SLAVE:
        //update del dmx - generalmente a 40Hz -
        if (Packet_Detected_Flag)
        {
            Packet_Detected_Flag = 0;

            if (data11[0] == 0x00)    //dmx packet
            {
                //update the colors channels
                for (unsigned char i = 0; i < 6; i++)
                    mem->fixed_channels[i] = data11[i + 1];
                
                resp = resp_change;
                break;
            }
        }
        
        resp = SlaveMenu (mem, actions);

        if (resp == resp_finish)
        {
            MasterResetInnerMode(mem);            
            resp = resp_change;
        }
        break;
        
    default:
        master_slave_state = MASTER_SLAVE_MODE_INIT;
        break;
    }

    return resp;
    
}


void MasterResetInnerMode (parameters_typedef * mem)
{
    mem->program_inner_type = MASTER_NO_INNER_MODE;
    master_slave_state = MASTER_SLAVE_MODE_INIT;
    
    //colors reset
    for (unsigned char i = 0; i < 6; i++)
        mem->fixed_channels[i] = 0;

    DMX_Disable();
        
}
//--- end of file ---//
