//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MENUES.C ###############################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "menues.h"
#include "hard.h"
#include "stm32f0xx.h"

#include "lcd.h"
#include "lcd_utils.h"

#include <stdio.h>
#include <string.h>


//--- VARIABLES EXTERNAS ---//


//--- VARIABLES GLOBALES ---//


//-- timers del modulo --------------------
volatile unsigned short main_menu_timer = 0;
// volatile unsigned short slave_mode_dmx_receiving_timer = 0;

// extern volatile unsigned short standalone_timer;
// extern volatile unsigned short standalone_enable_menu_timer;
// extern volatile unsigned short minutes;
// extern volatile unsigned short scroll1_timer;

//--- Para los menues LCD ----------


//-- Private Defines -----------------
//-- para los menues -----------------
typedef enum {
    SLAVE_MODE_SHOW_CONF_0 = 0,
    SLAVE_MODE_SHOW_CONF_1,
    SLAVE_MODE_SHOW_CONF_2,
    SLAVE_MODE_SHOW_CONF_3,
    SLAVE_MODE_SHOW_CONF_4,
    SLAVE_MODE_SHOW_CONF_5,
    SLAVE_MODE_SHOW_CONF_6    

} slave_mode_show_t;

typedef enum {
    MENU_ON = 0,
    MENU_SELECTED,
    MENU_OFF,
    MENU_WAIT_FREE

} slave_mode_m_manager_t;

typedef enum {
    SLAVE_MODE_MENU_INIT = 0,
    SLAVE_MODE_MENU_CONF_1,
    SLAVE_MODE_MENU_CONF_2,
    SLAVE_MODE_MENU_CONF_3,
    SLAVE_MODE_MENU_DMX_CHANNEL,
    SLAVE_MODE_MENU_DMX_CHANNEL_QUANTITY,
    SLAVE_MODE_MENU_GRANDMASTER

} slave_mode_menu_t;

typedef enum {
    SLAVE_MODE_MENU_RUNNING_INIT = 0,
    SLAVE_MODE_MENU_RUNNING_CHECK,
    SLAVE_MODE_MENU_RUNNING_CHANGE,
    SLAVE_MODE_MENU_RUNNING_MANUAL_CHANGE

} slave_mode_menu_running_t;

static slave_mode_show_t slave_mode_show_conf = SLAVE_MODE_SHOW_CONF_0;
static slave_mode_m_manager_t slave_mode_menu_manager = MENU_ON;
static slave_mode_menu_t slave_mode_menu_state = SLAVE_MODE_MENU_INIT;

//-- Private Functions ----------


//--- FUNCIONES DEL MODULO ---//
inline void UpdateTimerModeMenu (void)
{
    if (main_menu_timer)
        main_menu_timer--;

}

void MainMenuReset (void)
{
    slave_mode_state = SLAVE_MODE_INIT;
}

void MainMenu (void)
{
    resp_t resp = resp_continue;
    unsigned char i;
    unsigned short dummysp;

    switch (slave_mode_state)
    {
    case SLAVE_MODE_INIT:
        slave_mode_state++;
        ShowConfSlaveModeReset();
        SlaveModeMenuManagerReset();
        break;

    case SLAVE_MODE_CONF:
        resp = ShowConfSlaveMode();

        if (resp == resp_finish)
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

            //CH1
            dummysp = data7[1];
            dummysp <<= 2;
            if (dummysp > 820)
                sp1 = 820;
            else
                sp1 = dummysp;

            //CH2
            dummysp = data7[2];
            dummysp <<= 2;
            if (dummysp > 820)
                sp2 = 820;
            else
                sp2 = dummysp;

            //CH3
            dummysp = data7[3];
            dummysp <<= 2;
            if (dummysp > 820)
                sp3 = 820;
            else
                sp3 = dummysp;

            //CH4
            dummysp = data7[4];
            dummysp <<= 2;
            if (dummysp > 820)
                sp4 = 820;
            else
                sp4 = dummysp;

            //CH5
            dummysp = data7[5];
            dummysp <<= 2;
            if (dummysp > 820)
                sp5 = 820;
            else
                sp5 = dummysp;

            //CH6
            dummysp = data7[6];
            dummysp <<= 2;
            if (dummysp > 820)
                sp6 = 820;
            else
                sp6 = dummysp;

            check_dmx_lcd_pckt = 1;
            
        }

        //filters para el dmx - generalmente 8 puntos a 200Hz -
        //desde el sp al sp_filter
        if (!dmx_filters_timer)
        {
            dmx_filters_timer = 5;

            sp1_filtered = MAFilterFast (sp1, v_sp1);
            sp2_filtered = MAFilterFast (sp2, v_sp2);
            sp3_filtered = MAFilterFast (sp3, v_sp3);
            sp4_filtered = MAFilterFast (sp4, v_sp4);
            sp5_filtered = MAFilterFast (sp5, v_sp5);
            sp6_filtered = MAFilterFast (sp6, v_sp6);
        }
        
        UpdateSamplesAndPID ();

        UpdateSlaveModeMenuManager();
        
        break;

    default:
        slave_mode_state = SLAVE_MODE_INIT;
        break;
    }
}
            

void SlaveModeMenuManagerReset (void)
{
    slave_mode_menu_manager = MENU_ON;
}

inline void UpdateSlaveModeMenuManager (void)
{
    resp_t resp = resp_continue;
    
    //veo el menu solo si alguien toca los botones / timeout o DMX enchufado
    switch (slave_mode_menu_manager)
    {
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
            // LCDClearScreen();
            CTRL_BKL_OFF;
            slave_mode_menu_manager = MENU_OFF;
        }
        break;

    case MENU_SELECTED:
        //estado algo seleccionado espero update
        resp = FuncShowBlink ((const char *) "Something Select", (const char *) "Updating Values", 1, BLINK_NO);

        if (resp == resp_finish)
        {
            // slave_mode_state = SLAVE_MODE_UPDATE;
            slave_mode_menu_manager = MENU_ON;
        }
        break;

    case MENU_OFF:
        //estado menu apagado
        if ((CheckS1() > S_NO) || (CheckS2() > S_NO) || (slave_mode_dmx_receiving_timer))
        {
            slave_mode_enable_menu_timer = TT_MENU_TIMEOUT;    //vuelvo a mostrar
            slave_mode_menu_manager = MENU_ON;
            CTRL_BKL_ON;
        }
        break;

    case MENU_WAIT_FREE:
        if ((CheckS1() == S_NO) && (CheckS2() == S_NO))
        {
            slave_mode_menu_manager = MENU_ON;
            //voy a activar el Menu
            //me fijo en ue parte del Menu estaba
            //TODO ES UNA CHANCHADA, PERO BUENO...
            // if (slave_mode_menu_state <= STAND_ALONE_MENU_RAMP_ON_DIMMING)
            // {
            //     //salgo directo
            //     LCD_2DO_RENGLON;
            //     LCDTransmitStr((const char *) "Cont.     Select");
            // }
            // else
            // {
            //     if (slave_mode_menu_state <= STAND_ALONE_MENU_MOV_SENS_SELECTED_2)
            //     {
            //         slave_mode_menu_state = STAND_ALONE_MENU_MOV_SENS;
            //     }
            //     else if (slave_mode_menu_state <= STAND_ALONE_MENU_LDR_SELECTED_5)
            //     {
            //         slave_mode_menu_state = STAND_ALONE_MENU_LDR;
            //     }
            //     else if (slave_mode_menu_state <= STAND_ALONE_MENU_MAX_DIMMING_SELECTED_1)
            //     {
            //         slave_mode_menu_state = STAND_ALONE_MENU_MAX_DIMMING;
            //     }
            //     else if (slave_mode_menu_state <= STAND_ALONE_MENU_MIN_DIMMING_SELECTED_1)
            //     {
            //         slave_mode_menu_state = STAND_ALONE_MENU_MIN_DIMMING;
            //     }
            //     else if (slave_mode_menu_state <= STAND_ALONE_MENU_RAMP_ON_START_SELECTED_1)
            //     {
            //         slave_mode_menu_state =STAND_ALONE_MENU_RAMP_ON_START;
            //     }
            //     else if (slave_mode_menu_state <= STAND_ALONE_MENU_RAMP_ON_DIMMING_SELECTED_1)
            //     {
            //         slave_mode_menu_state = STAND_ALONE_MENU_RAMP_ON_DIMMING;
            //     }
            //     FuncOptionsReset ();
            //     FuncShowSelectv2Reset ();
            //     FuncChangeReset ();
            // }
        }
        break;

    default:
        slave_mode_menu_manager = 0;
        break;
    }

    if (CheckS1() > S_HALF)
        resp = resp_change_all_up;

}


inline resp_t MenuSlaveModeRunning (void)
{
    resp_t resp = resp_continue;
    unsigned short dummy = 0;
    char s_lcd1 [10];
    char s_lcd2 [10];
    short one_int = 0, one_dec = 0;
    

    switch (slave_mode_menu_state)
    {
    case SLAVE_MODE_MENU_RUNNING_INIT:
        //empiezo con las selecciones

        //fuerzo cambio en ch1
        last_ch1 = ~data7[1];

        last_ch2 = data7[2];
        last_ch3 = data7[3];
        last_ch4 = data7[4];
        last_ch5 = data7[5];
        last_ch6 = data7[6];

        slave_mode_menu_state++;
        
        break;

    case SLAVE_MODE_MENU_RUNNING_CHECK:

        if (check_dmx_lcd_pckt)    //TODO: despues sacar esta traba
        {
            check_dmx_lcd_pckt = 0;
            if (last_ch1 != data7[1])
            {
                last_ch1 = data7[1];
                dmx_local_channel = mem_conf.dmx_channel;
                dmx_local_value = data7[1];
                slave_mode_menu_state++;
                break;
            }

            if (last_ch2 != data7[2])
            {
                last_ch2 = data7[2];            
                dmx_local_channel = mem_conf.dmx_channel + 1;
                dmx_local_value = data7[2];
                slave_mode_menu_state++;
                break;
            }

            if (last_ch3 != data7[3])
            {
                last_ch3 = data7[3];
                dmx_local_channel = mem_conf.dmx_channel + 2;
                dmx_local_value = data7[3];
                slave_mode_menu_state++;
                break;
            }

            if (last_ch4 != data7[4])
            {
                last_ch4 = data7[4];
                dmx_local_channel = mem_conf.dmx_channel + 3;
                dmx_local_value = data7[4];
                slave_mode_menu_state++;
                break;
            }

            if (last_ch5 != data7[5])
            {
                last_ch5 = data7[5];
                dmx_local_channel = mem_conf.dmx_channel + 4;
                dmx_local_value = data7[5];
                slave_mode_menu_state++;
                break;
            }

            if (last_ch6 != data7[6])
            {
                last_ch6 = data7[6];
                dmx_local_channel = mem_conf.dmx_channel + 5;
                dmx_local_value = data7[6];
                slave_mode_menu_state++;
                break;
            }
        }

        if ((CheckS1() > S_NO) || (CheckS2() > S_NO))
            slave_mode_menu_state = SLAVE_MODE_MENU_RUNNING_MANUAL_CHANGE;

        break;

    case SLAVE_MODE_MENU_RUNNING_CHANGE:
        
        fcalc = dmx_local_value;
        fcalc = fcalc * K_100P;
        one_int = (short) fcalc;
        fcalc = fcalc - one_int;
        fcalc = fcalc * 10;
        one_dec = (short) fcalc;

        sprintf(s_lcd2, "%3d.%01d", one_int, one_dec);
        strcat(s_lcd2, "%  ");

        sprintf(s_lcd1, "ch:%3d D", dmx_local_channel);

        resp = FuncShowBlink ((const char *) s_lcd1, (const char *) s_lcd2, 0, BLINK_NO);

        if (resp == resp_finish)
        {
            resp = resp_continue;
            slave_mode_menu_state = SLAVE_MODE_MENU_RUNNING_CHECK;
        }
        break;

    case SLAVE_MODE_MENU_RUNNING_MANUAL_CHANGE:
        if (!CheckS1() && !CheckS2())
        {
            if (dmx_local_channel < (mem_conf.dmx_channel + mem_conf.dmx_channel_quantity - 1))
            {
                dmx_local_channel++;
                dmx_local_value = data7[dmx_local_channel];
            }
            else
            {
                dmx_local_channel = 1;
                dmx_local_value = data7[1];
            }
            slave_mode_menu_state = SLAVE_MODE_MENU_RUNNING_CHANGE;
        }
        break;
        
    default:
        slave_mode_menu_state = SLAVE_MODE_MENU_RUNNING_INIT;
        break;
    }
    return resp;
}

inline resp_t MenuSlaveMode (void)
{
    resp_t resp = resp_continue;
//     unsigned short dummy = 0;

//     switch (slave_mode_menu_state)
//     {
//     case SLAVE_MODE_MENU_INIT:
//         //empiezo con las selecciones
//         resp = FuncShowBlink ((const char *) "Slv Mode", (const char *) "Selects ", 1, BLINK_NO);

//         if (resp == resp_finish)
//         {
//             slave_mode_menu_state++;
//             resp = resp_continue;
//         }
//         break;

//     case SLAVE_MODE_MENU_CONF_1:
//         resp = FuncShowSelectv2 ((const char * ) "DMX chnl");

//         if (resp == resp_change)	//cambio de menu
//             slave_mode_menu_state = SLAVE_MODE_MENU_CONF_2;

//         if (resp == resp_selected)	//se eligio el menu
//             slave_mode_menu_state = SLAVE_MODE_MENU_DMX_CHANNEL;

//         if (resp != resp_continue)
//             resp = resp_working;

//         break;

//     case SLAVE_MODE_MENU_CONF_2:
//         resp = FuncShowSelectv2 ((const char * ) "Chn qnty");

//         if (resp == resp_change)	//cambio de menu
//             slave_mode_menu_state = SLAVE_MODE_MENU_CONF_3;


//         if (resp == resp_selected)	//se eligio el menu
//             slave_mode_menu_state = SLAVE_MODE_MENU_DMX_CHANNEL_QUANTITY;

//         if (resp != resp_continue)
//             resp = resp_working;

//         break;

//     case SLAVE_MODE_MENU_CONF_3:
//         resp = FuncShowSelectv2 ((const char * ) "W Grandm");

//         if (resp == resp_change)	//cambio de menu
//             slave_mode_menu_state = SLAVE_MODE_MENU_CONF_1;

//         if (resp == resp_selected)	//se eligio el menu
//             slave_mode_menu_state = SLAVE_MODE_MENU_GRANDMASTER;

//         if (resp != resp_continue)
//             resp = resp_working;

//         break;

//     // case SLAVE_MODE_MENU_CONF_4:
//     //     resp = FuncShowSelectv2 ((const char * ) "Done!   ");

//     //     if (resp == resp_change)	//cambio de menu
//     //         slave_mode_menu_state = SLAVE_MODE_MENU_CONF_1;

//     //     if (resp == resp_selected)	//se eligio el menu
//     //     {
//     //         slave_mode_menu_state = SLAVE_MODE_MENU_INIT;
//     //     }

//     //     if (resp != resp_continue)
//     //         resp = resp_working;

//     //     break;
        
//         // -------- DESDE ACA CONFIGURO ---------//
        
//     case SLAVE_MODE_MENU_DMX_CHANNEL:
//         resp = FuncChangeChannels (&dmx_channel);

//         if (resp == resp_finish)
//         {
//             slave_mode_menu_state = SLAVE_MODE_MENU_CONF_1;
//             // unsigned short local;
//             // // local = Slave_ModeStruct_local.max_dimmer_value_percent * 255;
//             // // local = local / 100;
//             // // Slave_ModeStruct_local.max_dimmer_value_dmx = local;

//             // // slave_mode_menu_state = SLAVE_MODE_MENU_MAX_DIMMING_SELECTED_1;
//             // // LCD_1ER_RENGLON;
//             // // LCDTransmitStr((const char *) "wait to free    ");
//             resp = resp_working;
//         }
//         break;

//     case SLAVE_MODE_MENU_DMX_CHANNEL_QUANTITY:
//         resp = FuncChangeChannelsQuantity (&dmx_channel);

//         if (resp == resp_finish)
//         {
//             slave_mode_menu_state = SLAVE_MODE_MENU_CONF_2;
//             // unsigned short local;
//             // // local = Slave_ModeStruct_local.max_dimmer_value_percent * 255;
//             // // local = local / 100;
//             // // Slave_ModeStruct_local.max_dimmer_value_dmx = local;

//             // // slave_mode_menu_state = SLAVE_MODE_MENU_MAX_DIMMING_SELECTED_1;
//             // // LCD_1ER_RENGLON;
//             // // LCDTransmitStr((const char *) "wait to free    ");
//             resp = resp_working;
//         }
//         break;

//     case SLAVE_MODE_MENU_GRANDMASTER:        
//         resp = FuncChangeOnOff (&grandmaster_value);

//         if (resp == resp_finish)
//         {
//             slave_mode_menu_state = SLAVE_MODE_MENU_CONF_3;
//             // unsigned short local;
//             // // local = Slave_ModeStruct_local.max_dimmer_value_percent * 255;
//             // // local = local / 100;
//             // // Slave_ModeStruct_local.max_dimmer_value_dmx = local;

//             // // slave_mode_menu_state = SLAVE_MODE_MENU_MAX_DIMMING_SELECTED_1;
//             // // LCD_1ER_RENGLON;
//             // // LCDTransmitStr((const char *) "wait to free    ");
//             resp = resp_working;
//         }
//         break;
        

//     // case SLAVE_MODE_MENU_MOV_SENS_SELECTED:
//     //     if (Slave_ModeStruct_local.move_sensor_enable)
//     //         resp = 0x80;
//     //     else
//     //         resp = 0x81;

//     //     FuncOptions ((const char *) "on   off   back ",(const char *) s_blank_line, (unsigned char *)s_sel, 3, resp);
//     //     slave_mode_menu_state++;
//     //     break;

//     // case SLAVE_MODE_MENU_MOV_SENS_SELECTED_1:
//     //     resp = FuncOptions ((const char *) "on   off   back ",(const char *) s_blank_line, (unsigned char *)s_sel, 3, 0);

//     //     if ((resp & 0x0f) == resp_selected)
//     //     {
//     //         resp = resp & 0xf0;
//     //         resp >>= 4;
//     //         if (resp == 0)
//     //         {
//     //             Slave_ModeStruct_local.move_sensor_enable = 1;
//     //             slave_mode_menu_state++;
//     //             LCD_1ER_RENGLON;
//     //             LCDTransmitStr((const char *) "wait to free    ");
//     //             resp = resp_working;
//     //         }

//     //         if (resp == 1)
//     //         {
//     //             Slave_ModeStruct_local.move_sensor_enable = 0;
//     //             resp = resp_selected;
//     //             slave_mode_menu_state = SLAVE_MODE_MENU_MOV_SENS;
//     //         }

//     //         if (resp == 2)
//     //         {
//     //             resp = resp_working;
//     //             slave_mode_menu_state = SLAVE_MODE_MENU_MOV_SENS_SELECTED_4;
//     //             LCD_1ER_RENGLON;
//     //             LCDTransmitStr((const char *) "wait to free    ");
//     //         }
//     //     }
//     //     break;

//     // case SLAVE_MODE_MENU_MOV_SENS_SELECTED_2:
//     //     if (CheckS2() == S_NO)
//     //         slave_mode_menu_state++;

//     //     resp = resp_working;
//     //     break;

//     // case SLAVE_MODE_MENU_MOV_SENS_SELECTED_3:
//     //     resp = FuncChangeSecsMove (&Slave_ModeStruct_local.move_sensor_secs);

//     //     if (resp == resp_finish)
//     //     {
//     //         slave_mode_menu_state++;
//     //         LCD_1ER_RENGLON;
//     //         LCDTransmitStr((const char *) "wait to free    ");
//     //     }

//     //     resp = resp_working;
//     //     break;

//     // case SLAVE_MODE_MENU_MOV_SENS_SELECTED_4:
//     //     if (CheckS2() == S_NO)
//     //         slave_mode_menu_state = SLAVE_MODE_MENU_MOV_SENS;

//     //     resp = resp_working;
//     //     break;

//     // case SLAVE_MODE_MENU_LDR_SELECTED:
//     //     if (Slave_ModeStruct_local.ldr_enable)
//     //         resp = 0x80;
//     //     else
//     //         resp = 0x81;

//     //     FuncOptions ((const char *) "on   off   back ",(const char *) s_blank_line, (unsigned char *)s_sel, 3, resp);
//     //     slave_mode_menu_state++;
//     //     break;

//     // case SLAVE_MODE_MENU_LDR_SELECTED_1:
//     //     resp = FuncOptions ((const char *) "on   off   back ",(const char *) s_blank_line,(unsigned char *) s_sel, 3, 0);

//     //     if ((resp & 0x0f) == resp_selected)
//     //     {
//     //         resp = resp & 0xf0;
//     //         resp >>= 4;
//     //         if (resp == 0)
//     //         {
//     //             Slave_ModeStruct_local.ldr_enable = 1;
//     //             slave_mode_menu_state++;
//     //             LCD_1ER_RENGLON;
//     //             LCDTransmitStr((const char *) "wait to free    ");
//     //         }

//     //         if (resp == 1)
//     //         {
//     //             Slave_ModeStruct_local.ldr_enable = 0;
//     //             resp = resp_selected;
//     //             slave_mode_menu_state = SLAVE_MODE_MENU_LDR;
//     //         }

//     //         if (resp == 2)
//     //         {
//     //             resp = resp_working;
//     //             slave_mode_menu_state = SLAVE_MODE_MENU_LDR_SELECTED_4;
//     //             LCD_1ER_RENGLON;
//     //             LCDTransmitStr((const char *) "wait to free    ");
//     //         }
//     //     }
//     //     break;

//     // case SLAVE_MODE_MENU_LDR_SELECTED_2:
//     //     if (CheckS2() == S_NO)
//     //         slave_mode_menu_state++;

//     //     resp = resp_working;
//     //     break;

//     // case SLAVE_MODE_MENU_LDR_SELECTED_3:
//     //     //resp = FuncChange (&Slave_ModeStruct_local.ldr_value);
//     //     resp = FuncChangePercent (&Slave_ModeStruct_local.ldr_value);

//     //     if (resp == resp_finish)
//     //         slave_mode_menu_state = SLAVE_MODE_MENU_LDR_SELECTED_5;

//     //     resp = resp_working;
//     //     break;

//     // case SLAVE_MODE_MENU_LDR_SELECTED_4:
//     //     if (CheckS2() == S_NO)
//     //         slave_mode_menu_state = SLAVE_MODE_MENU_LDR;

//     //     resp = resp_working;
//     //     break;

//     // case SLAVE_MODE_MENU_LDR_SELECTED_5:
//     //     resp = resp_working;
//     //     if (CheckS2() == S_NO)
//     //     {
//     //         slave_mode_menu_state = SLAVE_MODE_MENU_LDR;
//     //         resp = resp_selected;
//     //     }
//     //     break;

//     // case SLAVE_MODE_MENU_MAX_DIMMING_SELECTED:
//     //     resp = FuncChangePercent (&Slave_ModeStruct_local.max_dimmer_value_percent);

//     //     if (resp == resp_finish)
//     //     {
//     //         unsigned short local;
//     //         local = Slave_ModeStruct_local.max_dimmer_value_percent * 255;
//     //         local = local / 100;
//     //         Slave_ModeStruct_local.max_dimmer_value_dmx = local;

//     //         slave_mode_menu_state = SLAVE_MODE_MENU_MAX_DIMMING_SELECTED_1;
//     //         LCD_1ER_RENGLON;
//     //         LCDTransmitStr((const char *) "wait to free    ");
//     //     }

//     //     resp = resp_working;
//     //     break;

//     // case SLAVE_MODE_MENU_MAX_DIMMING_SELECTED_1:
//     //     resp = resp_working;
//     //     if (CheckS2() == S_NO)
//     //     {
//     //         //hago el update si corresponde
//     //         if (slave_mode_ii > Slave_ModeStruct_local.max_dimmer_value_dmx)
//     //         {
//     //             slave_mode_ii = Slave_ModeStruct_local.max_dimmer_value_dmx;
//     //             Update_TIM3_CH1 (slave_mode_ii);
//     //             slave_mode_dimming_last_slope = DIM_DOWN;
//     //         }
//     //         slave_mode_menu_state = SLAVE_MODE_MENU_MAX_DIMMING;
//     //         resp = resp_selected;
//     //     }
//     //     break;

//     // case SLAVE_MODE_MENU_MIN_DIMMING_SELECTED:
//     //     resp = FuncChangePercent (&Slave_ModeStruct_local.min_dimmer_value_percent);

//     //     if (resp == resp_finish)
//     //     {
//     //         unsigned short local;
//     //         local = Slave_ModeStruct_local.min_dimmer_value_percent * 255;
//     //         local = local / 100;
//     //         Slave_ModeStruct_local.min_dimmer_value_dmx = local;

//     //         slave_mode_menu_state = SLAVE_MODE_MENU_MIN_DIMMING_SELECTED_1;
//     //         LCD_1ER_RENGLON;
//     //         LCDTransmitStr((const char *) "wait to free    ");
//     //     }

//     //     resp = resp_working;
//     //     break;

//     // case SLAVE_MODE_MENU_MIN_DIMMING_SELECTED_1:
//     //     resp = resp_working;
//     //     if (CheckS2() == S_NO)
//     //     {
//     //         //hago el update si corresponde
//     //         if (slave_mode_ii < Slave_ModeStruct_local.min_dimmer_value_dmx)
//     //         {
//     //             slave_mode_ii = Slave_ModeStruct_local.min_dimmer_value_dmx;
//     //             Update_TIM3_CH1 (slave_mode_ii);
//     //             slave_mode_dimming_last_slope = DIM_UP;
//     //         }

//     //         slave_mode_menu_state = SLAVE_MODE_MENU_MIN_DIMMING;
//     //         resp = resp_selected;
//     //     }
//     //     break;

//     // case SLAVE_MODE_MENU_RAMP_ON_START_SELECTED:
//     //     //ajusto el timer
//     //     dummy = Slave_ModeStruct_local.power_up_timer_value / 1000;
//     //     resp = FuncChangeSecs (&dummy);

//     //     if (resp == resp_finish)
//     //     {
//     //         Slave_ModeStruct_local.power_up_timer_value = dummy * 1000;
//     //         slave_mode_menu_state = SLAVE_MODE_MENU_RAMP_ON_START_SELECTED_1;
//     //         LCD_1ER_RENGLON;
//     //         LCDTransmitStr((const char *) "wait to free    ");
//     //     }

//     //     resp = resp_working;
//     //     break;

//     // case SLAVE_MODE_MENU_RAMP_ON_START_SELECTED_1:
//     //     resp = resp_working;
//     //     if (CheckS2() == S_NO)
//     //     {
//     //         slave_mode_menu_state = SLAVE_MODE_MENU_RAMP_ON_START;
//     //         resp = resp_selected;
//     //     }
//     //     break;

//     // case SLAVE_MODE_MENU_RAMP_ON_DIMMING_SELECTED:
//     //     //ajusto el timer
//     //     dummy = Slave_ModeStruct_local.dimming_up_timer_value / 1000;
//     //     resp = FuncChangeSecs (&dummy);

//     //     if (resp == resp_finish)
//     //     {
//     //         Slave_ModeStruct_local.dimming_up_timer_value = dummy * 1000;
//     //         slave_mode_menu_state = SLAVE_MODE_MENU_RAMP_ON_DIMMING_SELECTED_1;
//     //         LCD_1ER_RENGLON;
//     //         LCDTransmitStr((const char *) "wait to free    ");
//     //     }

//     //     resp = resp_working;
//     //     break;

//     // case SLAVE_MODE_MENU_RAMP_ON_DIMMING_SELECTED_1:
//     //     resp = resp_working;
//     //     if (CheckS2() == S_NO)
//     //     {
//     //         slave_mode_menu_state = SLAVE_MODE_MENU_RAMP_ON_DIMMING;
//     //         resp = resp_selected;
//     //     }
//     //     break;

//     default:
//         slave_mode_menu_state = SLAVE_MODE_MENU_INIT;
//         break;
//     }
    return resp;
}

void ShowConfSlaveModeReset (void)
{
    slave_mode_show_conf = SLAVE_MODE_SHOW_CONF_0;
}

resp_t ShowConfSlaveMode (void)
{
    resp_t resp = resp_continue;
    char s_current [20];	//usar s_conf[]

    switch (slave_mode_show_conf)
    {
    case SLAVE_MODE_SHOW_CONF_0:
         //TODO: cambiar a func blink
        LCD_1ER_RENGLON;
        LCDTransmitStr((const char *) "Slv Mode");
        LCD_2DO_RENGLON;
        LCDTransmitStr((const char *) "Config  ");        
        slave_mode_show_conf++;
        break;

    case SLAVE_MODE_SHOW_CONF_1:
        // if (params_struct.grandmaster == 0)
        //     resp = FuncScroll2((const char *)"Without Grandmaster");
        // else
        //     resp = FuncScroll2((const char *)"Grandmaster Enabled");

        resp = FuncScroll2((const char *)"Without Grandmaster");
        
        if (resp == resp_finish)
        {
            resp = resp_continue;
            slave_mode_show_conf++;
        }
        break;

    case SLAVE_MODE_SHOW_CONF_2:
        memset(s_current, ' ', sizeof(s_current));
        // sprintf(s_current, "Dmx Ch. On %3d", params_struct.dmx_ch);
        sprintf(s_current, "Dmx Ch. On %3d", 1);
        resp = FuncScroll2((const char *) s_current);

        if (resp == resp_finish)
        {
            resp = resp_continue;
            slave_mode_show_conf++;
        }
        break;

    case SLAVE_MODE_SHOW_CONF_3:
        // memset(s_current, ' ', sizeof(s_current));
        // sprintf(s_current, "Max Dimm at %3d", Slave_ModeStruct_local.max_dimmer_value_percent);
        // strcat(s_current, (const char*)"%");
        // resp = FuncScroll2((const char *) s_current);

        // if (resp == resp_finish)
            slave_mode_show_conf++;

        break;

    case SLAVE_MODE_SHOW_CONF_4:
        // memset(s_current, ' ', sizeof(s_current));
        // sprintf(s_current, "Min Dimm at %3d", Slave_ModeStruct_local.min_dimmer_value_percent);
        // strcat(s_current, (const char*)"%");
        // resp = FuncScroll2((const char *) s_current);

        // if (resp == resp_finish)
            slave_mode_show_conf++;

        break;

    case SLAVE_MODE_SHOW_CONF_5:
        // memset(s_current, ' ', sizeof(s_current));
        // sprintf(s_current, "Ramp ON Start %2d", (Slave_ModeStruct_local.power_up_timer_value / 1000));
        // strcat(s_current, (const char*)"s");
        // resp = FuncScroll2((const char *) s_current);

        // if (resp == resp_finish)
            slave_mode_show_conf++;

        break;

    case SLAVE_MODE_SHOW_CONF_6:
        // memset(s_current, ' ', sizeof(s_current));
        // sprintf(s_current, "Ramp Dimming %2d", (Slave_ModeStruct_local.dimming_up_timer_value / 1000));
        // strcat(s_current, (const char*)"s");
        // resp = FuncScroll2((const char *) s_current);

        // if (resp == resp_finish)
        // {
        //     slave_mode_show_conf = SLAVE_MODE_SHOW_CONF_0;
        //     resp = resp_finish;
        // }
        slave_mode_show_conf = SLAVE_MODE_SHOW_CONF_0;
        resp = resp_finish;        
        break;

    default:
        slave_mode_show_conf = SLAVE_MODE_SHOW_CONF_0;
        resp = resp_finish;
        break;
    }
    return resp;
}

//--- end of file ---//
