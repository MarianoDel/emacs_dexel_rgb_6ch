//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX2_MODE.C ###############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "dmx2_mode.h"
#include "dmx_menu.h"
#include "colors_functions.h"
#include "flash_program.h"

#include <stdio.h>
#include <string.h>


// Private Types Constants and Macros ------------------------------------------
typedef enum {
    DMX2_MODE_INIT = 0,
    DMX2_MODE_RUNNING

} dmx2_mode_e;


#define DMX2_PKT_TYPE    0
#define DMX2_DIM_CH    1
#define DMX2_STB_CH    2
#define DMX2_SPD_CH    3
#define DMX2_ECT_CH    4
#define DMX2_CLR_CH1    5
#define DMX2_CLR_CH2    6
#define DMX2_CLR_CH3    7
#define DMX2_CLR_CH4    8
#define DMX2_CLR_CH5    9
#define DMX2_CLR_CH6    10


#define TT_MENU_TIMEOUT    30000
#define TT_DMX_RECEIVING    1000

// variables re-use
#define dmx2_mode_state    mode_state
#define dmx2_mode_effect_timer    mode_effect_timer


// Externals -------------------------------------------------------------------
extern volatile unsigned char data11[];

// -- externals re-used
extern unsigned char mode_state;
extern volatile unsigned short mode_effect_timer;


//del main para dmx
extern volatile unsigned char Packet_Detected_Flag;
extern volatile unsigned short DMX_channel_selected;

extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
unsigned char dmx2_end_of_packet_update = 0;


//-- timers del modulo --------------------
volatile unsigned short dmx2_mode_enable_menu_timer = 0;
// volatile unsigned short dmx2_mode_dmx_receiving_timer = 0;


// Module Private Functions ----------------------------------------------------
void DMX2Mode_ChannelsDimmer (unsigned char *, unsigned char *);
// void DMX2Mode_ChannelsStrobe (unsigned char *, unsigned char *);
// void DMX2Mode_ChannelsEffect (unsigned char *, unsigned char *);
unsigned char DMX2Mode_MapEffect (unsigned char);
unsigned char DMX2Mode_MapSpeed (unsigned char);

// Module Functions ------------------------------------------------------------
void DMX2Mode_UpdateTimers (void)
{
    if (dmx2_mode_enable_menu_timer)
        dmx2_mode_enable_menu_timer--;

    if (dmx2_mode_effect_timer)
        dmx2_mode_effect_timer--;
    
    // if (dmx2_mode_dmx_receiving_timer)
    //     dmx2_mode_dmx_receiving_timer--;

}

void DMX2ModeReset (void)
{
    dmx2_mode_state = DMX2_MODE_INIT;
}

#define timer_address    dmx2_mode_enable_menu_timer
unsigned char dmx2_address_show = 0;
resp_t DMX2Mode (unsigned char * ch_val, sw_actions_t action)
{
    resp_t resp = resp_continue;
    
    
    switch (dmx2_mode_state)
    {
    case DMX2_MODE_INIT:
        DMXModeMenuReset();
        mem_conf.program_inner_type = DMX2_INNER_DIMMER_MODE;
        DMXModeMenu_ChangeAddressReset();
        dmx2_address_show = 1;
        dmx2_mode_state++;
        break;

    case DMX2_MODE_RUNNING:

        //update del dmx - generalmente a 40Hz -
        //desde el dmx al sp
        if (Packet_Detected_Flag)
        {
            Packet_Detected_Flag = 0;

            //le aviso al menu que se estan recibiendo paquetes dmx
            // dmx2_mode_dmx_receiving_timer = TT_DMX_RECEIVING;            

            if (data11[DMX2_PKT_TYPE] == 0x00)    //dmx packet
            {
                if (data11[DMX2_ECT_CH] != 0)    // check if we have an effect
                {
                    // map the effect in this mode
                    if (DMX2Mode_MapEffect(data11[DMX2_ECT_CH]))
                    {
                        if (mem_conf.program_inner_type != DMX2_INNER_GRADUAL_MODE)
                            mem_conf.program_inner_type = DMX2_INNER_GRADUAL_MODE;
                    }
                    else
                    {
                        if (mem_conf.program_inner_type != DMX2_INNER_SKIPPING_MODE)
                            mem_conf.program_inner_type = DMX2_INNER_SKIPPING_MODE;
                    }

                    // map the speed in this mode
                    mem_conf.program_inner_type_speed = DMX2Mode_MapSpeed(data11[DMX2_SPD_CH]);
                }
                else if (data11[DMX2_STB_CH])    // check if we are in strobe
                {
                    if (mem_conf.program_inner_type != DMX2_INNER_STROBE_MODE)
                        mem_conf.program_inner_type = DMX2_INNER_STROBE_MODE;
                    
                    // map the speed in this mode
                    mem_conf.program_inner_type_speed = DMX2Mode_MapSpeed(data11[DMX2_SPD_CH]);
                }
                else    // we are in DMX mode with grandmaster
                {
                    if (mem_conf.program_inner_type != DMX2_INNER_DIMMER_MODE)
                        mem_conf.program_inner_type = DMX2_INNER_DIMMER_MODE;
                    
                    DMX2Mode_ChannelsDimmer(ch_val, data11);
                    dmx2_end_of_packet_update = 1;
                    resp = resp_change;
                    break;
                }

                dmx2_end_of_packet_update = 1;
            }
        }

#ifndef NO_DISPLAY_UPDATE_ON_DMX
        if (dmx2_end_of_packet_update)
        {
            dmx_menu_data_t dmx2_st;
            dmx2_st.dmx_first_chnl = &mem_conf.dmx_first_channel;
            dmx2_st.mode = DMX2_MODE;
            dmx2_st.pchannels = ch_val;
            if (dmx2_address_show)
                dmx2_st.show_addres = 1;
            else
                dmx2_st.show_addres = 0;

            resp = DMXModeMenu(&dmx2_st);
            if (resp == resp_finish)
                dmx2_end_of_packet_update = 0;
            
        }
#endif
        
        break;

    default:
        dmx2_mode_state = DMX2_MODE_INIT;
        break;
    }


    //Effects in modes
    switch (mem_conf.program_inner_type)
    {
    case DMX2_INNER_SKIPPING_MODE:
        if (!dmx2_mode_effect_timer)
        {
            resp = Colors_Fading_Pallete (ch_val);
            // if (resp == resp_finish)
            //     resp = resp_continue;

            resp = resp_change;
            dmx2_mode_effect_timer = 10 - mem_conf.program_inner_type_speed;
        }
        break;

    case DMX2_INNER_GRADUAL_MODE:
        if (!dmx2_mode_effect_timer)
        {
            resp = Colors_Fading_Shuffle_Pallete (ch_val);
            // if (resp == resp_finish)
            //     resp = resp_continue;

            resp = resp_change;
            dmx2_mode_effect_timer = 10 - mem_conf.program_inner_type_speed;
        }
        break;

    case DMX2_INNER_STROBE_MODE:
        if (!dmx2_mode_effect_timer)
        {
            resp = Colors_Strobe_Pallete (ch_val);
            // if (resp == resp_finish)
            //     resp = resp_continue;

            resp = resp_change;            
            dmx2_mode_effect_timer = 2000 - mem_conf.program_inner_type_speed * 200;
        }
        break;

    default:
        // do nothing in here
        break;
    }


    //look for a change in address if we are not changing colors
    if (resp != resp_change)
    {
        dmx_menu_address_data_t dmx2_addr_st;
        dmx2_addr_st.dmx_address = mem_conf.dmx_first_channel;
        dmx2_addr_st.dmx_channels_qtty = mem_conf.dmx_channel_quantity + 4;
        dmx2_addr_st.actions = action;
        dmx2_addr_st.timer = &timer_address;
        dmx2_addr_st.address_show = &dmx2_address_show;
        resp = DMXModeMenu_ChangeAddress(&dmx2_addr_st);

        if (resp == resp_change)
        {
            // change the DMX address
            DMX_channel_selected = dmx2_addr_st.dmx_address;
            mem_conf.dmx_first_channel = DMX_channel_selected;
        
            // force a display update
            dmx2_end_of_packet_update = 1;
        }

        if (resp == resp_finish)
        {
            //end of changing ask for a memory save
            resp = resp_need_to_save;
        
        }
    }
    return resp;
    
}
            

void DMX2Mode_ChannelsDimmer (unsigned char * ch_out, unsigned char * ch_in)
{
    unsigned short calc = 0;

    for (unsigned char i = 0; i < 6; i++)
    {
        calc = ch_in[DMX2_DIM_CH] * ch_in[DMX2_CLR_CH1 + i];
        calc >>= 8;
        *(ch_out + i) = (unsigned char) calc;
    }
}


unsigned char DMX2Mode_MapEffect (unsigned char eff)
{
    if (eff < 127)
        return 0;
    else
        return 1;
}


unsigned char DMX2Mode_MapSpeed (unsigned char speed)
{
    unsigned char out_speed = 0;
    
    if (speed > 230)
        out_speed = 9;
    else if (speed > 205)
        out_speed = 8;
    else if (speed > 180)
        out_speed = 7;
    else if (speed > 155)
        out_speed = 6;
    else if (speed > 130)
        out_speed = 5;
    else if (speed > 105)
        out_speed = 4;
    else if (speed > 80)
        out_speed = 3;
    else if (speed > 55)
        out_speed = 2;
    else if (speed > 20)
        out_speed = 1;

    return out_speed;
}

//--- end of file ---//
