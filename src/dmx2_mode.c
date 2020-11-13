//-------------------------------------------------
// #### DEXEL 6CH BIDIRECCIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX2_MODE.C ###############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "dmx2_mode.h"
#include "dmx1_menu.h"
#include "flash_program.h"

#include <stdio.h>
#include <string.h>


// Private Types Constants and Macros ------------------------------------------
typedef enum {
    SLAVE_MODE_INIT = 0,
    SLAVE_MODE_RUNNING

} slave_mode_e;

typedef enum {
    DO_NOTHING = 0,
    TO_CHANGE_WAIT_FREE,
    CHANGING,
    TO_DO_NOTHING_WAIT_FREE,
    TO_CLEAN_OUT
    
} slave_mode_address_e;

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

// Externals -------------------------------------------------------------------
extern volatile unsigned char data11[];

//del main para dmx
extern volatile unsigned char Packet_Detected_Flag;
extern volatile unsigned short DMX_channel_selected;

extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
slave_mode_e slave_mode_state = SLAVE_MODE_INIT;
unsigned char dmx_end_of_packet_update = 0;


//-- timers del modulo --------------------
volatile unsigned short slave_mode_enable_menu_timer = 0;
volatile unsigned short slave_mode_dmx_receiving_timer = 0;


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void UpdateTimerSlaveMode (void)
{
    if (slave_mode_enable_menu_timer)
        slave_mode_enable_menu_timer--;

    if (slave_mode_dmx_receiving_timer)
        slave_mode_dmx_receiving_timer--;

}

void FuncSlaveModeReset (void)
{
    slave_mode_state = SLAVE_MODE_INIT;
}

#define TT_SHOW_ADDRESS    500
#define CNTR_TO_OUT    16
#define timer_address    slave_mode_enable_menu_timer
unsigned char address_show = 0;
unsigned char address_cntr_out = 0;
slave_mode_address_e slave_mode_address = DO_NOTHING;
void FuncSlaveMode (unsigned char * ch_val, sw_actions_t action)
{
    resp_t resp = resp_continue;
    
    
    switch (slave_mode_state)
    {
    case SLAVE_MODE_INIT:
        DMX1ModeMenuReset();
        address_show = 1;
        slave_mode_state++;
        break;

    case SLAVE_MODE_RUNNING:

        //update del dmx - generalmente a 40Hz -
        //desde el dmx al sp
        if (Packet_Detected_Flag)
        {
            Packet_Detected_Flag = 0;

            //le aviso al menu que se estan recibiendo paquetes dmx
            slave_mode_dmx_receiving_timer = TT_DMX_RECEIVING;            

            //ajuste por grandmaster
            if (mem_conf.dmx_grandmaster)
            {
                unsigned short dummy_16;
                
                dummy_16 = data7[0] * data7[1];
                dummy_16 >>= 8;
                data7[1] = (unsigned char) dummy_16;

                dummy_16 = data7[0] * data7[2];
                dummy_16 >>= 8;
                data7[2] = (unsigned char) dummy_16;

                dummy_16 = data7[0] * data7[3];
                dummy_16 >>= 8;
                data7[3] = (unsigned char) dummy_16;

                dummy_16 = data7[0] * data7[4];
                dummy_16 >>= 8;
                data7[4] = (unsigned char) dummy_16;

                dummy_16 = data7[0] * data7[5];
                dummy_16 >>= 8;
                data7[5] = (unsigned char) dummy_16;

                dummy_16 = data7[0] * data7[6];
                dummy_16 >>= 8;
                data7[6] = (unsigned char) dummy_16;
            }

            // if (!dmx_end_of_packet_update)
            // {
                //update de valores recibidos
                *(ch_val + 0) = data7[1];
                *(ch_val + 1) = data7[2];
                *(ch_val + 2) = data7[3];
                *(ch_val + 3) = data7[4];
                *(ch_val + 4) = data7[5];
                *(ch_val + 5) = data7[6];

                dmx_end_of_packet_update = 1;
            // }
            

#ifdef WITH_POWER_CONTROL
            PWM_Set_PwrCtrl(ch_val, mem_conf.dmx_channel_quantity);
#endif
        }

#ifndef NO_DISPLAY_UPDATE_ON_DMX
        if (dmx_end_of_packet_update)
        {
            // unsigned char ch[6];
            // ch[0] = *(ch_val + 0);
            // ch[1] = *(ch_val + 1);
            // ch[2] = *(ch_val + 2);
            // ch[3] = *(ch_val + 3);
            // ch[4] = *(ch_val + 4);
            // ch[5] = *(ch_val + 5);
            
            dmx1_menu_data_t dmx1_st;
            dmx1_st.dmx_first_chnl = &mem_conf.dmx_first_channel;
            // dmx1_st.pchannels = ch;
            dmx1_st.pchannels = ch_val;
            if (address_show)
                dmx1_st.show_addres = 1;
            else
                dmx1_st.show_addres = 0;

            resp = DMX1ModeMenu(&dmx1_st);
            if (resp == resp_finish)
                dmx_end_of_packet_update = 0;
            
        }
#endif
        
        break;

    default:
        slave_mode_state = SLAVE_MODE_INIT;
        break;
    }

    
    //check for a change in address
    switch (slave_mode_address)
    {
    case DO_NOTHING:
        if (action == selection_enter)
            slave_mode_address++;
        
        break;

    case TO_CHANGE_WAIT_FREE:
        if (action == do_nothing)
        {
            address_cntr_out = CNTR_TO_OUT;
            slave_mode_address++;
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
                dmx_end_of_packet_update = 1;
                address_show = 1;
                timer_address = TT_SHOW_ADDRESS;
                address_cntr_out = CNTR_TO_OUT;

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
                dmx_end_of_packet_update = 1;
                address_show = 1;
                timer_address = TT_SHOW_ADDRESS;
                address_cntr_out = CNTR_TO_OUT;                

                // resp = resp_need_to_save;
            }
        }

        if (action == selection_enter)
            slave_mode_address++;

        if (!timer_address)
        {
            if (address_show)
                address_show = 0;
            else
                address_show = 1;

            if (address_cntr_out)
                address_cntr_out--;
            
            timer_address = TT_SHOW_ADDRESS;
        }

        if (!address_cntr_out)
            slave_mode_address = TO_CLEAN_OUT;
        
        break;

    case TO_DO_NOTHING_WAIT_FREE:
        if (action == do_nothing)
            slave_mode_address++;
        
        break;

    case TO_CLEAN_OUT:
        address_show = 1;
        slave_mode_address = DO_NOTHING;
        break;
        
    default:
        slave_mode_address = DO_NOTHING;
        break;            
    }
            
}
            

//--- end of file ---//
