//----------------------------------------------------------
// #### PROYECTO DEXEL 6CH BIDIRECCIONAL - Custom Board ####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.C ###################################
//----------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "test_functions.h"
#include "hard.h"
#include "adc.h"
#include "uart.h"
#include "dma.h"
#include "tim.h"

#include "i2c.h"
#include "screen.h"

#include "dmx_transceiver.h"


#include <stdio.h>
// Externals -------------------------------------------------------------------
extern volatile unsigned short adc_ch [];
extern volatile unsigned short timer_standby;
extern volatile unsigned char Packet_Detected_Flag;
extern volatile unsigned char data512[];
extern volatile unsigned char * pdmx;


// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void TF_Usart1_Tx_Single (void)
{
    USART1Config();

    while (1)
    {
        if (!timer_standby)
        {
            timer_standby = 100;
            USART1->TDR = 'M';
        }
    }
}


void TF_Usart1_Tx_Int (void)
{
    USART1Config();
    pdmx = &data512[0];
    
    while (1)
    {
        if (!timer_standby)
        {
            timer_standby = 100;
            USART1->CR1 |= USART_CR1_TXEIE;
        }
    }
}


void TF_Usart1_Tx_Dmx (void)
{
    TIM_16_Init();    //para tx dmx OneShoot
    USART1Config();

    while (1)
    {
        if (!timer_standby)
        {
            timer_standby = 100;
            SendDMXPacket (PCKT_INIT);            
        }
    }
}


void TF_Voltage_Temperature (void)
{
    char s_to_send [100];
    unsigned short seq_cnt = 0;
    float volts = 0.0;
    unsigned char volts_int = 0;
    unsigned char volts_dec = 0;
    while (1)
    {
        if (sequence_ready)
        {
            sequence_ready_reset;
            seq_cnt++;
            if (seq_cnt > 2400)
            {
                seq_cnt = 0;
                volts = V_Sense_48V * 3.3 * 34;
                volts = volts / 4095;
                volts_int = (unsigned char) volts;
                volts = volts - volts_int;
                volts = volts * 100;
                volts_dec = (unsigned char) volts;
                sprintf(s_to_send, "temp: %d vdig: %d voltage: %d.%02dV\n",
                        Temp_Channel,
                        V_Sense_48V,
                        volts_int,
                        volts_dec);
                Usart2Send(s_to_send);
            }
        }
    }
}


void TF_Control_Fan (void)
{
    while (1)
    {
        CTRL_FAN_ON;
        Wait_ms(1000);
        CTRL_FAN_OFF;
        Wait_ms(2000);
    }
}


extern void display_update_int_state_machine (void);
void TF_Oled_Screen (void)
{
    // OLED Init
    Wait_ms(500);    //for supply stability
    I2C2_Init();
    Wait_ms(10);

    //primer pantalla
    SCREEN_Init();

    unsigned char a = 0;
    while (1)
    {
        if (!timer_standby)
        {
            CTRL_FAN_ON;
            timer_standby = 1000;
            if (a)
            {
                SCREEN_ShowText2(
                    "Primera  ",
                    " Pantalla",
                    "         ",
                    "         "
                    );
                a = 0;
            }
            else
            {
                SCREEN_ShowText2(
                    "         ",
                    "         ",
                    "Segunda  ",
                    " Pantalla"
                    );
                a = 1;
            }
            CTRL_FAN_OFF;
        }
        display_update_int_state_machine();

        //chequeo de jitter
        // if (!dmx_timeout_timer)
        // {
        //     dmx_timeout_timer = 5;
        //     if (CTRL_FAN)
        //         CTRL_FAN_OFF;
        //     else
        //         CTRL_FAN_ON;
            
        // }
    }
}


extern resp_t Colors_Strobe (unsigned char * ch_val, unsigned char strobe_ch);
void TF_Oled_Screen_And_Strobe2s (void)
{
    // OLED Init
    Wait_ms(500);    //for supply stability
    I2C2_Init();
    Wait_ms(10);

    //primer pantalla
    SCREEN_Init();

    SCREEN_ShowText2(
        "Testeo de",
        " Placas  ",
        "  RGB6CH ",
        "  KIRNO  "
        );

    timer_standby = 1300;
    
    while (timer_standby)
        display_update_int_state_machine();

    //two seconds strobe
    resp_t resp = resp_continue;
    unsigned char which_color = 1;

    TIM_16_Init();    //para tx dmx OneShoot
    USART1Config();
    
    while (1)
    {
        if (!timer_standby)
        {
            unsigned char ch_val [6] = { 0 };
            resp = Colors_Strobe (ch_val, which_color);

            if (resp == resp_finish)    //end of seted color sequence
            {
                if (which_color & 0x20)
                    which_color = 1;
                else
                    which_color <<= 1;
            }

            data512[0] = 0;
            data512[1] = *(ch_val + 0);
            data512[2] = *(ch_val + 1);
            data512[3] = *(ch_val + 2);
            data512[4] = *(ch_val + 3);
            data512[5] = *(ch_val + 4);
            data512[6] = *(ch_val + 5);
            
            SendDMXPacket (PCKT_INIT);
            timer_standby = 2000;
        }
    }
}


//--- end of file ---//
