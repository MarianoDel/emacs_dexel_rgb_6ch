//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX1_MODE.C ###############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "dmx1_mode.h"
#include "dmx_menu.h"
#include "flash_program.h"

#include <stdio.h>
#include <string.h>


// Private Types Constants and Macros ------------------------------------------
typedef enum {
    DMX1_MODE_INIT = 0,
    DMX1_MODE_RUNNING

} dmx1_mode_e;


#define DMX1_PKT_TYPE    0
#define DMX1_CLR_CH1    1
#define DMX1_CLR_CH2    2
#define DMX1_CLR_CH3    3
#define DMX1_CLR_CH4    4
#define DMX1_CLR_CH5    5
#define DMX1_CLR_CH6    6


#define TT_MENU_TIMEOUT    30000
#define TT_DMX_RECEIVING    1000

// variables re-use
#define dmx1_mode_state    mode_state
#define dmx1_mode_enable_menu_timer    mode_effect_timer


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
unsigned char dmx1_end_of_packet_update = 0;


//-- timers del modulo --------------------
// volatile unsigned short dmx1_mode_enable_menu_timer = 0;
// volatile unsigned short dmx1_mode_dmx_receiving_timer = 0;


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void DMX1Mode_UpdateTimers (void)
{
    if (dmx1_mode_enable_menu_timer)
        dmx1_mode_enable_menu_timer--;

    // if (dmx1_mode_dmx_receiving_timer)
    //     dmx1_mode_dmx_receiving_timer--;

}

void DMX1ModeReset (void)
{
    dmx1_mode_state = DMX1_MODE_INIT;
}


#define timer_address    dmx1_mode_enable_menu_timer
unsigned char dmx1_address_show = 0;
resp_t DMX1Mode (unsigned char * ch_val, sw_actions_t action)
{
    resp_t resp = resp_continue;
    
    switch (dmx1_mode_state)
    {
    case DMX1_MODE_INIT:
        DMXModeMenuReset();
        DMXModeMenu_ChangeAddressReset();
        dmx1_address_show = 1;
        dmx1_mode_state++;
        break;

    case DMX1_MODE_RUNNING:

        //update del dmx - generalmente a 40Hz -
        //desde el dmx al sp
        if (Packet_Detected_Flag)
        {
            Packet_Detected_Flag = 0;

            //le aviso al menu que se estan recibiendo paquetes dmx
            // dmx1_mode_dmx_receiving_timer = TT_DMX_RECEIVING;            

            if (data11[DMX1_PKT_TYPE] == 0x00)    //dmx packet
            {
                //update the colors channels
                *(ch_val + 0) = data11[DMX1_CLR_CH1];
                *(ch_val + 1) = data11[DMX1_CLR_CH2];
                *(ch_val + 2) = data11[DMX1_CLR_CH3];
                *(ch_val + 3) = data11[DMX1_CLR_CH4];
                *(ch_val + 4) = data11[DMX1_CLR_CH5];
                *(ch_val + 5) = data11[DMX1_CLR_CH6];

                dmx1_end_of_packet_update = 1;
                resp = resp_change;
                break;
            }
        }

        if (dmx1_end_of_packet_update)            
        {
            dmx_menu_data_t dmx1_st;
            dmx1_st.dmx_first_chnl = &mem_conf.dmx_first_channel;
            dmx1_st.mode = DMX1_MODE;
            dmx1_st.pchannels = ch_val;
            dmx1_st.chnls_qtty = mem_conf.dmx_channel_quantity;
            if (dmx1_address_show)
                dmx1_st.show_addres = 1;
            else
                dmx1_st.show_addres = 0;

            resp = DMXModeMenu(&dmx1_st);
            if (resp == resp_finish)
                dmx1_end_of_packet_update = 0;
            
        }
        break;

    default:
        dmx1_mode_state = DMX1_MODE_INIT;
        break;
    }

    
    //look for a change in address if we are not changing colors
    if (resp != resp_change)
    {
        dmx_menu_address_data_t dmx1_addr_st;
        dmx1_addr_st.dmx_address = mem_conf.dmx_first_channel;
        dmx1_addr_st.dmx_channels_qtty = mem_conf.dmx_channel_quantity;
        dmx1_addr_st.actions = action;
        dmx1_addr_st.timer = (unsigned short *) &timer_address;
        dmx1_addr_st.address_show = &dmx1_address_show;
        resp = DMXModeMenu_ChangeAddress(&dmx1_addr_st);

        if (resp == resp_change)
        {
            // change the DMX address
            DMX_channel_selected = dmx1_addr_st.dmx_address;
            mem_conf.dmx_first_channel = DMX_channel_selected;
        
            // force a display update
            dmx1_end_of_packet_update = 1;
        }

        if (resp == resp_finish)
        {
            //end of changing ask for a memory save
            resp = resp_need_to_save;
        
        }
    }
    return resp;
            
}
            

//--- end of file ---//
