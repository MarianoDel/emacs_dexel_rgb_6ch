//---------------------------------------------
// #### PROYECTO DEXEL 6CH BIDIRECCIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MODO_SLAVE.C #############################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "slave_mode.h"
#include "hard.h"
#include "stm32f0xx.h"
#include "dsp.h"

#include "ssd1306.h"
#include "mainmenu.h"
#include "flash_program.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------
extern volatile unsigned char data7[];

//del Main para timers
extern volatile unsigned char dmx_filters_timer;

//del main para dmx
extern volatile unsigned char Packet_Detected_Flag;

extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
slave_mode_t slave_mode_state = SLAVE_MODE_INIT;
unsigned char dmx_end_of_packet_update = 0;

//-- timers del modulo --------------------
volatile unsigned short slave_mode_enable_menu_timer = 0;
volatile unsigned short slave_mode_dmx_receiving_timer = 0;

// extern volatile unsigned short standalone_timer;
// extern volatile unsigned short standalone_enable_menu_timer;
// extern volatile unsigned short minutes;
// extern volatile unsigned short scroll1_timer;


//--- Para los menues LCD ----------
unsigned char last_ch1;
unsigned char last_ch2;
unsigned char last_ch3;
unsigned char last_ch4;
unsigned char last_ch5;
unsigned char last_ch6;

//-- Private Defines -----------------
//-- para los menues -----------------


typedef enum {
    MENU_INIT = 0,
    MENU_ON,
    MENU_OFF

} slave_mode_m_manager_t;


typedef enum {
    SLAVE_MODE_MENU_RUNNING_INIT = 0,
    SLAVE_MODE_MENU_RUNNING_WAIT_NEW_PACKET,
    SLAVE_MODE_MENU_RUNNING_CHECK_CH1,
    SLAVE_MODE_MENU_RUNNING_CHECK_CH2,
    SLAVE_MODE_MENU_RUNNING_CHECK_CH3,
    SLAVE_MODE_MENU_RUNNING_CHECK_CH4,
    SLAVE_MODE_MENU_RUNNING_CHECK_CH5,
    SLAVE_MODE_MENU_RUNNING_CHECK_CH6,
    SLAVE_MODE_MENU_RUNNING_UPDATE_DISPLAY

} slave_mode_menu_running_t;

static slave_mode_m_manager_t slave_mode_menu_manager = MENU_ON;
static slave_mode_menu_running_t slave_mode_menu_state = SLAVE_MODE_MENU_RUNNING_INIT;


// Module Private Functions ----------------------------------------------------
void SlaveModeMenuManagerReset (void);
void UpdateSlaveModeMenuManager (void);
resp_t MenuSlaveModeRunning (void);
void Percentage (unsigned char, unsigned char *, unsigned char *);


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

void FuncSlaveMode (unsigned char * ch_val)
{
    resp_t resp = resp_continue;
    unsigned short dummy_16;
    
    switch (slave_mode_state)
    {
    case SLAVE_MODE_INIT:
        SlaveModeMenuManagerReset();
        slave_mode_state++;
        break;

    case SLAVE_MODE_CONF:
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

            //update de valores recibidos
            *(ch_val + 0) = data7[1];
            *(ch_val + 1) = data7[2];
            *(ch_val + 2) = data7[3];
            *(ch_val + 3) = data7[4];
            *(ch_val + 4) = data7[5];
            *(ch_val + 5) = data7[6];
            
            dmx_end_of_packet_update = 1;
        }

        UpdateSlaveModeMenuManager();
        
        break;

    default:
        slave_mode_state = SLAVE_MODE_INIT;
        break;
    }
}
            

void SlaveModeMenuManagerReset (void)
{
    slave_mode_menu_manager = MENU_INIT;
}

inline void UpdateSlaveModeMenuManager (void)
{
    resp_t resp = resp_continue;
    
    //veo el menu solo si alguien toca los botones / timeout o DMX enchufado
    switch (slave_mode_menu_manager)
    {
    case MENU_INIT:
        gfx_setTextSize(1);
        gfx_setTextBg(0);
        gfx_setTextColor(1);
        display_clear();
        slave_mode_menu_manager++;
        break;
        
    case MENU_ON:
        //estado normal

        resp = MenuSlaveModeRunning();

        if (resp == resp_working)	//alguien esta tratando de seleccionar algo, le doy tiempo
            slave_mode_enable_menu_timer = TT_MENU_TIMEOUT;

        if (resp == resp_selected)	//se selecciono algo
        {
            slave_mode_enable_menu_timer = TT_MENU_TIMEOUT;
            slave_mode_menu_manager++;
        }

        //ya mostre el menu mucho tiempo, lo apago, si no estoy con dmx
        if ((!slave_mode_dmx_receiving_timer) && (!slave_mode_enable_menu_timer))
        {
            display_contrast(0x0F);
            slave_mode_menu_manager = MENU_OFF;
        }
        break;

    case MENU_OFF:
        //estado menu apagado
        if ((CheckS1() > S_NO) || (CheckS2() > S_NO) || (slave_mode_dmx_receiving_timer))
        {
            slave_mode_enable_menu_timer = TT_MENU_TIMEOUT;    //vuelvo a mostrar
            slave_mode_menu_manager = MENU_ON;
            display_contrast(0xCF);
        }
        break;

    default:
        slave_mode_menu_manager = 0;
        break;
    }

    if (CheckS1() > S_HALF)
        resp = resp_change_all_up;

}


unsigned char change_values = 0;
inline resp_t MenuSlaveModeRunning (void)
{
    resp_t resp = resp_continue;
    char s_temp[18];
    unsigned char one_int = 0;
    unsigned char one_dec = 0;

    switch (slave_mode_menu_state)
    {
    case SLAVE_MODE_MENU_RUNNING_INIT:
        //empiezo con las selecciones

        //fuerzo cambio
        last_ch1 = ~data7[1];
        last_ch2 = ~data7[2];
        last_ch3 = ~data7[3];
        last_ch4 = ~data7[4];
        last_ch5 = ~data7[5];
        last_ch6 = ~data7[6];

        slave_mode_menu_state++;
        break;

    case SLAVE_MODE_MENU_RUNNING_WAIT_NEW_PACKET:
        if (dmx_end_of_packet_update)
        {
            dmx_end_of_packet_update = 0;
            slave_mode_menu_state++;
        }
        break;
        
    case SLAVE_MODE_MENU_RUNNING_CHECK_CH1:
        if (last_ch1 != data7[1])
        {
            last_ch1 = data7[1];

            Percentage(last_ch1, &one_int, &one_dec);
            sprintf(s_temp, "ch%3d: %3d.%01d%%",
                    mem_conf.dmx_channel,
                    one_int,
                    one_dec);

            MainMenu_SetLine1(s_temp);
            change_values = 1;
        }

        slave_mode_menu_state++;
        break;

    case SLAVE_MODE_MENU_RUNNING_CHECK_CH2:        
        if (last_ch2 != data7[2])
        {
            last_ch2 = data7[2];            

            Percentage(last_ch2, &one_int, &one_dec);
            sprintf(s_temp, "ch%3d: %3d.%01d%%",
                    mem_conf.dmx_channel + 1,
                    one_int,
                    one_dec);

            MainMenu_SetLine2(s_temp);
            change_values = 1;
        }
        
        slave_mode_menu_state++;
        break;

    case SLAVE_MODE_MENU_RUNNING_CHECK_CH3:
        if (last_ch3 != data7[3])
        {
            last_ch3 = data7[3];

            Percentage(last_ch3, &one_int, &one_dec);
            sprintf(s_temp, "ch%3d: %3d.%01d%%",
                    mem_conf.dmx_channel + 2,
                    one_int,
                    one_dec);

            MainMenu_SetLine3(s_temp);
            change_values = 1;
        }

        slave_mode_menu_state++;
        break;

    case SLAVE_MODE_MENU_RUNNING_CHECK_CH4:
        if (last_ch4 != data7[4])
        {
            last_ch4 = data7[4];

            Percentage(last_ch4, &one_int, &one_dec);
            sprintf(s_temp, "ch%3d: %3d.%01d%%",
                    mem_conf.dmx_channel + 3,
                    one_int,
                    one_dec);

            MainMenu_SetLine4(s_temp);
            change_values = 1;
        }

        slave_mode_menu_state++;
        break;

    case SLAVE_MODE_MENU_RUNNING_CHECK_CH5:
        if (last_ch5 != data7[5])
        {
            last_ch5 = data7[5];

            Percentage(last_ch5, &one_int, &one_dec);
            sprintf(s_temp, "ch%3d: %3d.%01d%%",
                    mem_conf.dmx_channel + 4,
                    one_int,
                    one_dec);

            MainMenu_SetLine5(s_temp);
            change_values = 1;
        }

        slave_mode_menu_state++;
        break;

    case SLAVE_MODE_MENU_RUNNING_CHECK_CH6:
        if (last_ch6 != data7[6])
        {
            last_ch6 = data7[6];

            Percentage(last_ch6, &one_int, &one_dec);
            sprintf(s_temp, "ch%3d: %3d.%01d%%",
                    mem_conf.dmx_channel + 5,
                    one_int,
                    one_dec);

            MainMenu_SetLine6(s_temp);
            change_values = 1;
        }

        slave_mode_menu_state++;
        break;

    case SLAVE_MODE_MENU_RUNNING_UPDATE_DISPLAY:
        if (change_values)
        {
            change_values = 0;
            MainMenu_BlankOptions();
            MainMenu_SetOptions(0);
            display_update();
        }
        
        slave_mode_menu_state = SLAVE_MODE_MENU_RUNNING_WAIT_NEW_PACKET;
        break;
        
    default:
        slave_mode_menu_state = SLAVE_MODE_MENU_RUNNING_INIT;
        break;
    }
    
    return resp;
}


void Percentage (unsigned char dmx_value, unsigned char * val_int, unsigned char * val_dec)
{
    unsigned int calc = 0;
    
    if (dmx_value == 0)
    {
        *val_int = 0;
        *val_dec = 0;
    }
    else if (dmx_value == 255)
    {
        *val_int = 100;
        *val_dec = 0;
    }
    else
    {
        calc = dmx_value * 1000;
        calc = calc / 255;        
        *val_int = calc / 10;
        *val_dec = calc - *val_int * 10;
    }
}


//--- end of file ---//
