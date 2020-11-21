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
#include "ssd1306_gfx.h"
#include "screen.h"
#include "main_menu.h"
#include "dmx1_mode.h"


#include <stdio.h>
// Externals -------------------------------------------------------------------
extern volatile unsigned short adc_ch [];
extern volatile unsigned short timer_standby;
extern volatile unsigned char Packet_Detected_Flag;


// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
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


void TF_Oled_Screen (void)
{
    I2C2_Init();
    Wait_ms(100);

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


void TF_Oled_and_Main_Menu (void)
{
    // resp_t resp = resp_continue;
    sw_actions_t action = do_nothing;
    
    // resp = resp_ok;
    I2C2_Init();
    Wait_ms(100);

    MainMenu_Init();

    while (1)
    {
        action = do_nothing;

        // Check switches first
        action = CheckCW();
        // resp = MainMenu_Update(action);
        MainMenu_Update(action);        

        UpdateSwitches();
    }
}


void TF_Oled_and_DMX1_Mode (void)
{
    unsigned char ch_values [6] = { 0 };
    sw_actions_t action = do_nothing;
    
    I2C2_Init();
    Wait_ms(100);

    //primer pantalla
    SCREEN_Init();
    SCREEN_ShowText2(
        "Slave    ",
        "     Mode",
        "         ",
        "         "
        );

    DMX1ModeReset();

    while (1)
    {
        DMX1Mode(ch_values, action);

        //simulate dmx packets arrivals
        if (!timer_standby)
        {
            timer_standby = 100;
            Packet_Detected_Flag = 1;
        }
    }
}


void TF_Oled_and_Programs_Mode (void)
{
    unsigned char ch_values [6] = { 0 };
    sw_actions_t action = do_nothing;
    
    I2C2_Init();
    Wait_ms(100);

    //primer pantalla
    SCREEN_Init();
    SCREEN_ShowText2(
        "Programs ",
        "     Mode",
        "         ",
        "         "
        );

    ProgramsModeMenuReset();

    while (1)
    {
        FuncsProgramsMode(ch_values, action);
    }
}


void TF_Oled_and_Master_Mode (void)
{
    unsigned char ch_values [6] = { 0 };
    sw_actions_t action = do_nothing;
    
    I2C2_Init();
    Wait_ms(100);

    //primer pantalla
    SCREEN_Init();
    SCREEN_ShowText2(
        "Master   ",
        "     Mode",
        "         ",
        "         "
        );
    
    MasterModeMenuReset();

    while (1)
    {
        FuncsMasterMode(ch_values, action);
    }
}


//--- end of file ---//
