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
#include "flash_program.h"

#include <stdio.h>
#include <string.h>


// Private Types Constants and Macros ------------------------------------------
typedef enum {
    DMX2_MODE_INIT = 0,
    DMX2_MODE_RUNNING

} dmx2_mode_e;

typedef enum {
    DO_NOTHING = 0,
    TO_CHANGE_WAIT_FREE,
    CHANGING,
    TO_DO_NOTHING_WAIT_FREE,
    TO_CLEAN_OUT
    
} dmx2_mode_address_e;


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


// Externals -------------------------------------------------------------------
extern volatile unsigned char data11[];

//del main para dmx
extern volatile unsigned char Packet_Detected_Flag;
extern volatile unsigned short DMX_channel_selected;

extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
dmx2_mode_e dmx2_mode_state = DMX2_MODE_INIT;
unsigned char dmx2_end_of_packet_update = 0;


//-- timers del modulo --------------------
volatile unsigned short dmx2_mode_enable_menu_timer = 0;
volatile unsigned short dmx2_mode_dmx_receiving_timer = 0;


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void DMX2ModeUpdateTimer (void)
{
    if (dmx2_mode_enable_menu_timer)
        dmx2_mode_enable_menu_timer--;

    if (dmx2_mode_dmx_receiving_timer)
        dmx2_mode_dmx_receiving_timer--;

}

void DMX2ModeReset (void)
{
    dmx2_mode_state = DMX2_MODE_INIT;
}

#define TT_SHOW_ADDRESS    500
#define CNTR_TO_OUT    16
#define timer_address    dmx2_mode_enable_menu_timer
unsigned char dmx2_address_show = 0;
unsigned char dmx2_address_cntr_out = 0;
dmx2_mode_address_e dmx2_mode_address = DO_NOTHING;
void DMX2Mode (unsigned char * ch_val, sw_actions_t action)
{
    resp_t resp = resp_continue;
    
    
    switch (dmx2_mode_state)
    {
    case DMX2_MODE_INIT:
        DMXModeMenuReset();
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
            dmx2_mode_dmx_receiving_timer = TT_DMX_RECEIVING;            

            if (data11[DMX2_PKT_TYPE] == 0x00)    //dmx packet
            {
                //update the colors channels
                *(ch_val + 0) = data11[DMX2_CLR_CH1];
                *(ch_val + 1) = data11[DMX2_CLR_CH2];
                *(ch_val + 2) = data11[DMX2_CLR_CH3];
                *(ch_val + 3) = data11[DMX2_CLR_CH4];
                *(ch_val + 4) = data11[DMX2_CLR_CH5];
                *(ch_val + 5) = data11[DMX2_CLR_CH6];

                dmx2_end_of_packet_update = 1;
            }

#ifdef WITH_POWER_CONTROL
            PWM_Set_PwrCtrl(ch_val, mem_conf.dmx_channel_quantity);
#endif
        }

#ifndef NO_DISPLAY_UPDATE_ON_DMX
        if (dmx2_end_of_packet_update)
        {
            dmx_menu_data_t dmx2_st;
            dmx2_st.dmx_first_chnl = &mem_conf.dmx_first_channel;
            // dmx2_st.pchannels = ch;
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

    
    //check for a change in address
    switch (dmx2_mode_address)
    {
    case DO_NOTHING:
        if (action == selection_enter)
            dmx2_mode_address++;
        
        break;

    case TO_CHANGE_WAIT_FREE:
        if (action == do_nothing)
        {
            dmx2_address_cntr_out = CNTR_TO_OUT;
            dmx2_mode_address++;
        }
        break;
            
    case CHANGING:
        
        if (action == selection_up)
        {
            if (DMX_channel_selected < (512 - 6))
            {
                DMX_channel_selected++;
                mem_conf.dmx_first_channel = DMX_channel_selected;

                //force the display change
                dmx2_end_of_packet_update = 1;
                dmx2_address_show = 1;
                timer_address = TT_SHOW_ADDRESS;
                dmx2_address_cntr_out = CNTR_TO_OUT;

                // resp = resp_need_to_save;            
            }
        }
        
        if (action == selection_dwn)
        {
            if (DMX_channel_selected > 1)
            {
                DMX_channel_selected--;
                mem_conf.dmx_first_channel = DMX_channel_selected;            

                //force the display change
                dmx2_end_of_packet_update = 1;
                dmx2_address_show = 1;
                timer_address = TT_SHOW_ADDRESS;
                dmx2_address_cntr_out = CNTR_TO_OUT;                

                // resp = resp_need_to_save;
            }
        }

        if (action == selection_enter)
            dmx2_mode_address++;

        if (!timer_address)
        {
            if (dmx2_address_show)
                dmx2_address_show = 0;
            else
                dmx2_address_show = 1;

            if (dmx2_address_cntr_out)
                dmx2_address_cntr_out--;
            
            timer_address = TT_SHOW_ADDRESS;
        }

        if (!dmx2_address_cntr_out)
            dmx2_mode_address = TO_CLEAN_OUT;
        
        break;

    case TO_DO_NOTHING_WAIT_FREE:
        if (action == do_nothing)
            dmx2_mode_address++;
        
        break;

    case TO_CLEAN_OUT:
        dmx2_address_show = 1;
        dmx2_mode_address = DO_NOTHING;
        break;
        
    default:
        dmx2_mode_address = DO_NOTHING;
        break;            
    }
            
}
            

//--- end of file ---//
