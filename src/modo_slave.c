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
#include "modo_slave.h"
#include "hard.h"
#include "stm32f0xx.h"
#include "tim.h"
#include "dsp.h"
#include "adc.h"

#include "lcd.h"
#include "lcd_utils.h"

#include <stdio.h>
#include <string.h>


//--- VARIABLES EXTERNAS ---//
extern volatile unsigned char data7[];

//del Main
extern volatile unsigned char dmx_filters_timer;
extern volatile unsigned char Packet_Detected_Flag;
extern volatile unsigned short adc_ch [];

//--- VARIABLES GLOBALES ---//
slave_mode_t slave_mode_state = SLAVE_MODE_INIT;

//-- timers del modulo --------------------
volatile unsigned short slave_mode_enable_menu_timer = 0;
// extern volatile unsigned short standalone_timer;
// extern volatile unsigned short standalone_enable_menu_timer;
// extern volatile unsigned short minutes;
// extern volatile unsigned short scroll1_timer;




//--- Para el PID ----------
unsigned char undersampling = 0;
#define PID_UNDERSAMPLING    20

short d_ch1;
short d_ch2;
short d_ch3;
short d_ch4;
short d_ch5;
short d_ch6;
short e_z1_ch1;
short e_z1_ch2;
short e_z1_ch3;
short e_z1_ch4;
short e_z1_ch5;
short e_z1_ch6;

short e_z2_ch1;
short e_z2_ch2;
short e_z2_ch3;
short e_z2_ch4;
short e_z2_ch5;
short e_z2_ch6;

unsigned short sp1 = 0;
unsigned short sp2 = 0;
unsigned short sp3 = 0;
unsigned short sp4 = 0;
unsigned short sp5 = 0;
unsigned short sp6 = 0;

unsigned short sp1_filtered = 0;
unsigned short sp2_filtered = 0;
unsigned short sp3_filtered = 0;
unsigned short sp4_filtered = 0;
unsigned short sp5_filtered = 0;
unsigned short sp6_filtered = 0;

unsigned short v_sp1 [8];
unsigned short v_sp2 [8];
unsigned short v_sp3 [8];
unsigned short v_sp4 [8];
unsigned short v_sp5 [8];
unsigned short v_sp6 [8];

#define sequence_ready         (DMA1->ISR & DMA_ISR_TCIF1)
#define sequence_ready_reset   (DMA1->IFCR = DMA_ISR_TCIF1)

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

} slave_mode_menu_t;

static slave_mode_show_t slave_mode_show_conf = SLAVE_MODE_SHOW_CONF_0;
static slave_mode_m_manager_t slave_mode_menu_manager = MENU_ON;
static slave_mode_menu_t slave_mode_menu_state = SLAVE_MODE_MENU_INIT;

//-- Private Functions ----------
void ShowConfSlaveModeReset (void);
resp_t ShowConfSlaveMode (void);
void SlaveModeMenuManagerReset (void);
void UpdateSlaveModeMenuManager (void);
resp_t MenuSlaveMode (void);        

//--- FUNCIONES DEL MODULO ---//
void UpdateTimerSlaveMode (void)
{
    if (slave_mode_enable_menu_timer)
        slave_mode_enable_menu_timer--;
}

void FuncSlaveMode (void)
{
    resp_t resp = resp_continue;
    unsigned char i;
    unsigned short dummysp;

    switch (slave_mode_state)
    {
    case SLAVE_MODE_INIT:
        for (i = 0; i < 8; i++)
        {
            v_sp1[i] = 0;
            v_sp2[i] = 0;
            v_sp3[i] = 0;
            v_sp4[i] = 0;
            v_sp5[i] = 0;
            v_sp6[i] = 0;
        }

        slave_mode_state++;
        ShowConfSlaveModeReset();
        break;

    case SLAVE_MODE_CONF:
        resp = ShowConfSlaveMode();

        if (resp == resp_finish)
            slave_mode_state++;

        break;

    case SLAVE_MODE_RUNNING:

        //Update de muestras y lazos PID
        //con sp_filter
        if (sequence_ready)
        {
            // Clear DMA TC flag
            sequence_ready_reset;

            if (undersampling < (PID_UNDERSAMPLING - 1))
            {
                undersampling++;
            }
            else
            {
                undersampling = 0;

                //PID CH1
                if (!sp1_filtered)
                    Update_PWM1(0);
                else
                {
                    d_ch1 = PID_roof (sp1_filtered, I_Channel_1, d_ch1, &e_z1_ch1, &e_z2_ch1);

                    if (d_ch1 < 0)
                        d_ch1 = 0;
                    else
                    {
                        if (d_ch1 > DUTY_90_PERCENT)
                            d_ch1 = DUTY_90_PERCENT;
                    
                        Update_PWM1(d_ch1);
                    }
                }

                //PID CH2
                if (!sp2_filtered)
                    Update_PWM2(0);
                else
                {                
                    d_ch2 = PID_roof (sp2_filtered, I_Channel_2, d_ch2, &e_z1_ch2, &e_z2_ch2);

                    if (d_ch2 < 0)
                        d_ch2 = 0;
                    else
                    {
                        if (d_ch2 > DUTY_90_PERCENT)
                            d_ch2 = DUTY_90_PERCENT;
                    
                        Update_PWM2(d_ch2);
                    }
                }

                //PID CH3
                if (!sp3_filtered)
                    Update_PWM3(0);
                else
                {                                
                    d_ch3 = PID_roof (sp3_filtered, I_Channel_3, d_ch3, &e_z1_ch3, &e_z2_ch3);

                    if (d_ch3 < 0)
                        d_ch3 = 0;
                    else
                    {
                        if (d_ch3 > DUTY_90_PERCENT)
                            d_ch3 = DUTY_90_PERCENT;
                    
                        Update_PWM3(d_ch3);
                    }
                }

                //PID CH4
                if (!sp4_filtered)
                    Update_PWM4(0);
                else
                {
                    d_ch4 = PID_roof (sp4_filtered, I_Channel_4, d_ch4, &e_z1_ch4, &e_z2_ch4);

                    if (d_ch4 < 0)
                        d_ch4 = 0;
                    else
                    {
                        if (d_ch4 > DUTY_90_PERCENT)
                            d_ch4 = DUTY_90_PERCENT;
                    
                        Update_PWM4(d_ch4);
                    }
                }

                //PID CH5
                if (!sp5_filtered)
                    Update_PWM5(0);
                else
                {                
                    d_ch5 = PID_roof (sp5_filtered, I_Channel_5, d_ch5, &e_z1_ch5, &e_z2_ch5);

                    if (d_ch5 < 0)
                        d_ch5 = 0;
                    else
                    {
                        if (d_ch5 > DUTY_90_PERCENT)
                            d_ch5 = DUTY_90_PERCENT;
                    
                        Update_PWM5(d_ch5);
                    }
                }

                //PID CH6
                if (!sp6_filtered)
                    Update_PWM6(0);
                else
                {                                
                    d_ch6 = PID_roof (sp6_filtered, I_Channel_6, d_ch6, &e_z1_ch6, &e_z2_ch6);

                    if (d_ch6 < 0)
                        d_ch6 = 0;
                    else
                    {
                        if (d_ch6 > DUTY_90_PERCENT)
                            d_ch6 = DUTY_90_PERCENT;
                    
                        Update_PWM6(d_ch6);
                    }
                }               
            }
        }

        //update del dmx - generalmente a 40Hz -
        //desde el dmx al sp
        if (Packet_Detected_Flag)
        {
            Packet_Detected_Flag = 0;

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
    
    //veo el menu solo si alguien toca los botones o timeout
    switch (slave_mode_menu_manager)
    {
    case MENU_ON:
        //estado normal

        resp = MenuSlaveMode();

        if (resp == resp_working)	//alguien esta tratando de seleccionar algo, le doy tiempo
            slave_mode_enable_menu_timer = TT_MENU_TIMEOUT;

        if (resp == resp_selected)	//se selecciono algo
        {
            slave_mode_enable_menu_timer = TT_MENU_TIMEOUT;
            //slave_mode_state = STAND_ALONE_INIT;
            // if (RELAY)
            //     slave_mode_state = STAND_ALONE_ON;
            // else
            //     slave_mode_state = STAND_ALONE_OFF;
            slave_mode_menu_manager++;
        }

        if (!slave_mode_enable_menu_timer)	//ya mostre el menu mucho tiempo, lo apago
        {
//				LCD_1ER_RENGLON;
//				LCDTransmitStr((const char *)s_blank_line);
//				LCD_2DO_RENGLON;
//				LCDTransmitStr((const char *)s_blank_line);
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
        if ((CheckS1() > S_NO) || (CheckS2() > S_NO))
        {
            slave_mode_enable_menu_timer = TT_MENU_TIMEOUT;			//vuelvo a mostrar
            LCD_1ER_RENGLON;
            LCDTransmitStr((const char *) "wait to free    ");

            slave_mode_menu_manager++;

            CTRL_BKL_ON;
//				if (slave_mode_state == STAND_ALONE_SHOW_CONF)
//					ShowConfSlave_ModeResetEnd();
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


inline resp_t MenuSlaveMode (void)
{
    resp_t resp = resp_continue;
    unsigned char dummy = 0;

    switch (slave_mode_menu_state)
    {
    case SLAVE_MODE_MENU_INIT:
        //empiezo con las selecciones
        resp = FuncShowBlink ((const char *) "Starting Slave_Mode", (const char *) "Selections", 1, BLINK_NO);

        if (resp == resp_finish)
        {
            slave_mode_menu_state++;
            resp = resp_continue;
        }
        break;

    case SLAVE_MODE_MENU_CONF_1:
        resp = FuncShowSelectv2 ((const char * ) "Move Sensor Conf");

        if (resp == resp_change)	//cambio de menu
            slave_mode_menu_state = SLAVE_MODE_MENU_CONF_2;

        if (resp == resp_selected)	//se eligio el menu
            slave_mode_menu_state = SLAVE_MODE_MENU_CONF_3;

        if (resp != resp_continue)
            resp = resp_working;

        break;

    case SLAVE_MODE_MENU_CONF_2:
        resp = FuncShowSelectv2 ((const char * ) "Energy Harv Conf");

        if (resp == resp_change)	//cambio de menu
            slave_mode_menu_state = SLAVE_MODE_MENU_CONF_3;


        if (resp == resp_selected)	//se eligio el menu
            slave_mode_menu_state = SLAVE_MODE_MENU_CONF_2;

        if (resp != resp_continue)
            resp = resp_working;

        break;

    case SLAVE_MODE_MENU_CONF_3:
        resp = FuncShowSelectv2 ((const char * ) "Max Dimming Conf");

        if (resp == resp_change)	//cambio de menu
            slave_mode_menu_state = SLAVE_MODE_MENU_CONF_2;

        if (resp == resp_selected)	//se eligio el menu
            slave_mode_menu_state = SLAVE_MODE_MENU_CONF_2;

        if (resp != resp_continue)
            resp = resp_working;

        break;

    // case SLAVE_MODE_MENU_MIN_DIMMING:
    //     resp = FuncShowSelectv2 ((const char * ) "Min Dimming Conf");

    //     if (resp == resp_change)	//cambio de menu
    //         slave_mode_menu_state = SLAVE_MODE_MENU_RAMP_ON_START;

    //     if (resp == resp_selected)	//se eligio el menu
    //         slave_mode_menu_state = SLAVE_MODE_MENU_MIN_DIMMING_SELECTED;

    //     if (resp != resp_continue)
    //         resp = resp_working;

    //     break;

    // case SLAVE_MODE_MENU_RAMP_ON_START:
    //     resp = FuncShowSelectv2 ((const char * ) "Ramp ON/OFF Conf");

    //     if (resp == resp_change)	//cambio de menu
    //         slave_mode_menu_state = SLAVE_MODE_MENU_RAMP_ON_DIMMING;

    //     if (resp == resp_selected)	//se eligio el menu
    //         slave_mode_menu_state = SLAVE_MODE_MENU_RAMP_ON_START_SELECTED;

    //     if (resp != resp_continue)
    //         resp = resp_working;

    //     break;

    // case SLAVE_MODE_MENU_RAMP_ON_DIMMING:
    //     resp = FuncShowSelectv2 ((const char * ) "Ramp on Dim Conf");

    //     if (resp == resp_change)	//cambio de menu
    //         slave_mode_menu_state = SLAVE_MODE_MENU_MOV_SENS;

    //     if (resp == resp_selected)	//se eligio el menu
    //         slave_mode_menu_state = SLAVE_MODE_MENU_RAMP_ON_DIMMING_SELECTED;

    //     if (resp != resp_continue)
    //         resp = resp_working;

    //     break;

    // case SLAVE_MODE_MENU_MOV_SENS_SELECTED:
    //     if (Slave_ModeStruct_local.move_sensor_enable)
    //         resp = 0x80;
    //     else
    //         resp = 0x81;

    //     FuncOptions ((const char *) "on   off   back ",(const char *) s_blank_line, (unsigned char *)s_sel, 3, resp);
    //     slave_mode_menu_state++;
    //     break;

    // case SLAVE_MODE_MENU_MOV_SENS_SELECTED_1:
    //     resp = FuncOptions ((const char *) "on   off   back ",(const char *) s_blank_line, (unsigned char *)s_sel, 3, 0);

    //     if ((resp & 0x0f) == resp_selected)
    //     {
    //         resp = resp & 0xf0;
    //         resp >>= 4;
    //         if (resp == 0)
    //         {
    //             Slave_ModeStruct_local.move_sensor_enable = 1;
    //             slave_mode_menu_state++;
    //             LCD_1ER_RENGLON;
    //             LCDTransmitStr((const char *) "wait to free    ");
    //             resp = resp_working;
    //         }

    //         if (resp == 1)
    //         {
    //             Slave_ModeStruct_local.move_sensor_enable = 0;
    //             resp = resp_selected;
    //             slave_mode_menu_state = SLAVE_MODE_MENU_MOV_SENS;
    //         }

    //         if (resp == 2)
    //         {
    //             resp = resp_working;
    //             slave_mode_menu_state = SLAVE_MODE_MENU_MOV_SENS_SELECTED_4;
    //             LCD_1ER_RENGLON;
    //             LCDTransmitStr((const char *) "wait to free    ");
    //         }
    //     }
    //     break;

    // case SLAVE_MODE_MENU_MOV_SENS_SELECTED_2:
    //     if (CheckS2() == S_NO)
    //         slave_mode_menu_state++;

    //     resp = resp_working;
    //     break;

    // case SLAVE_MODE_MENU_MOV_SENS_SELECTED_3:
    //     resp = FuncChangeSecsMove (&Slave_ModeStruct_local.move_sensor_secs);

    //     if (resp == resp_finish)
    //     {
    //         slave_mode_menu_state++;
    //         LCD_1ER_RENGLON;
    //         LCDTransmitStr((const char *) "wait to free    ");
    //     }

    //     resp = resp_working;
    //     break;

    // case SLAVE_MODE_MENU_MOV_SENS_SELECTED_4:
    //     if (CheckS2() == S_NO)
    //         slave_mode_menu_state = SLAVE_MODE_MENU_MOV_SENS;

    //     resp = resp_working;
    //     break;

    // case SLAVE_MODE_MENU_LDR_SELECTED:
    //     if (Slave_ModeStruct_local.ldr_enable)
    //         resp = 0x80;
    //     else
    //         resp = 0x81;

    //     FuncOptions ((const char *) "on   off   back ",(const char *) s_blank_line, (unsigned char *)s_sel, 3, resp);
    //     slave_mode_menu_state++;
    //     break;

    // case SLAVE_MODE_MENU_LDR_SELECTED_1:
    //     resp = FuncOptions ((const char *) "on   off   back ",(const char *) s_blank_line,(unsigned char *) s_sel, 3, 0);

    //     if ((resp & 0x0f) == resp_selected)
    //     {
    //         resp = resp & 0xf0;
    //         resp >>= 4;
    //         if (resp == 0)
    //         {
    //             Slave_ModeStruct_local.ldr_enable = 1;
    //             slave_mode_menu_state++;
    //             LCD_1ER_RENGLON;
    //             LCDTransmitStr((const char *) "wait to free    ");
    //         }

    //         if (resp == 1)
    //         {
    //             Slave_ModeStruct_local.ldr_enable = 0;
    //             resp = resp_selected;
    //             slave_mode_menu_state = SLAVE_MODE_MENU_LDR;
    //         }

    //         if (resp == 2)
    //         {
    //             resp = resp_working;
    //             slave_mode_menu_state = SLAVE_MODE_MENU_LDR_SELECTED_4;
    //             LCD_1ER_RENGLON;
    //             LCDTransmitStr((const char *) "wait to free    ");
    //         }
    //     }
    //     break;

    // case SLAVE_MODE_MENU_LDR_SELECTED_2:
    //     if (CheckS2() == S_NO)
    //         slave_mode_menu_state++;

    //     resp = resp_working;
    //     break;

    // case SLAVE_MODE_MENU_LDR_SELECTED_3:
    //     //resp = FuncChange (&Slave_ModeStruct_local.ldr_value);
    //     resp = FuncChangePercent (&Slave_ModeStruct_local.ldr_value);

    //     if (resp == resp_finish)
    //         slave_mode_menu_state = SLAVE_MODE_MENU_LDR_SELECTED_5;

    //     resp = resp_working;
    //     break;

    // case SLAVE_MODE_MENU_LDR_SELECTED_4:
    //     if (CheckS2() == S_NO)
    //         slave_mode_menu_state = SLAVE_MODE_MENU_LDR;

    //     resp = resp_working;
    //     break;

    // case SLAVE_MODE_MENU_LDR_SELECTED_5:
    //     resp = resp_working;
    //     if (CheckS2() == S_NO)
    //     {
    //         slave_mode_menu_state = SLAVE_MODE_MENU_LDR;
    //         resp = resp_selected;
    //     }
    //     break;

    // case SLAVE_MODE_MENU_MAX_DIMMING_SELECTED:
    //     resp = FuncChangePercent (&Slave_ModeStruct_local.max_dimmer_value_percent);

    //     if (resp == resp_finish)
    //     {
    //         unsigned short local;
    //         local = Slave_ModeStruct_local.max_dimmer_value_percent * 255;
    //         local = local / 100;
    //         Slave_ModeStruct_local.max_dimmer_value_dmx = local;

    //         slave_mode_menu_state = SLAVE_MODE_MENU_MAX_DIMMING_SELECTED_1;
    //         LCD_1ER_RENGLON;
    //         LCDTransmitStr((const char *) "wait to free    ");
    //     }

    //     resp = resp_working;
    //     break;

    // case SLAVE_MODE_MENU_MAX_DIMMING_SELECTED_1:
    //     resp = resp_working;
    //     if (CheckS2() == S_NO)
    //     {
    //         //hago el update si corresponde
    //         if (slave_mode_ii > Slave_ModeStruct_local.max_dimmer_value_dmx)
    //         {
    //             slave_mode_ii = Slave_ModeStruct_local.max_dimmer_value_dmx;
    //             Update_TIM3_CH1 (slave_mode_ii);
    //             slave_mode_dimming_last_slope = DIM_DOWN;
    //         }
    //         slave_mode_menu_state = SLAVE_MODE_MENU_MAX_DIMMING;
    //         resp = resp_selected;
    //     }
    //     break;

    // case SLAVE_MODE_MENU_MIN_DIMMING_SELECTED:
    //     resp = FuncChangePercent (&Slave_ModeStruct_local.min_dimmer_value_percent);

    //     if (resp == resp_finish)
    //     {
    //         unsigned short local;
    //         local = Slave_ModeStruct_local.min_dimmer_value_percent * 255;
    //         local = local / 100;
    //         Slave_ModeStruct_local.min_dimmer_value_dmx = local;

    //         slave_mode_menu_state = SLAVE_MODE_MENU_MIN_DIMMING_SELECTED_1;
    //         LCD_1ER_RENGLON;
    //         LCDTransmitStr((const char *) "wait to free    ");
    //     }

    //     resp = resp_working;
    //     break;

    // case SLAVE_MODE_MENU_MIN_DIMMING_SELECTED_1:
    //     resp = resp_working;
    //     if (CheckS2() == S_NO)
    //     {
    //         //hago el update si corresponde
    //         if (slave_mode_ii < Slave_ModeStruct_local.min_dimmer_value_dmx)
    //         {
    //             slave_mode_ii = Slave_ModeStruct_local.min_dimmer_value_dmx;
    //             Update_TIM3_CH1 (slave_mode_ii);
    //             slave_mode_dimming_last_slope = DIM_UP;
    //         }

    //         slave_mode_menu_state = SLAVE_MODE_MENU_MIN_DIMMING;
    //         resp = resp_selected;
    //     }
    //     break;

    // case SLAVE_MODE_MENU_RAMP_ON_START_SELECTED:
    //     //ajusto el timer
    //     dummy = Slave_ModeStruct_local.power_up_timer_value / 1000;
    //     resp = FuncChangeSecs (&dummy);

    //     if (resp == resp_finish)
    //     {
    //         Slave_ModeStruct_local.power_up_timer_value = dummy * 1000;
    //         slave_mode_menu_state = SLAVE_MODE_MENU_RAMP_ON_START_SELECTED_1;
    //         LCD_1ER_RENGLON;
    //         LCDTransmitStr((const char *) "wait to free    ");
    //     }

    //     resp = resp_working;
    //     break;

    // case SLAVE_MODE_MENU_RAMP_ON_START_SELECTED_1:
    //     resp = resp_working;
    //     if (CheckS2() == S_NO)
    //     {
    //         slave_mode_menu_state = SLAVE_MODE_MENU_RAMP_ON_START;
    //         resp = resp_selected;
    //     }
    //     break;

    // case SLAVE_MODE_MENU_RAMP_ON_DIMMING_SELECTED:
    //     //ajusto el timer
    //     dummy = Slave_ModeStruct_local.dimming_up_timer_value / 1000;
    //     resp = FuncChangeSecs (&dummy);

    //     if (resp == resp_finish)
    //     {
    //         Slave_ModeStruct_local.dimming_up_timer_value = dummy * 1000;
    //         slave_mode_menu_state = SLAVE_MODE_MENU_RAMP_ON_DIMMING_SELECTED_1;
    //         LCD_1ER_RENGLON;
    //         LCDTransmitStr((const char *) "wait to free    ");
    //     }

    //     resp = resp_working;
    //     break;

    // case SLAVE_MODE_MENU_RAMP_ON_DIMMING_SELECTED_1:
    //     resp = resp_working;
    //     if (CheckS2() == S_NO)
    //     {
    //         slave_mode_menu_state = SLAVE_MODE_MENU_RAMP_ON_DIMMING;
    //         resp = resp_selected;
    //     }
    //     break;

    default:
        slave_mode_menu_state = SLAVE_MODE_MENU_INIT;
        break;
    }
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
        LCD_1ER_RENGLON;
        //TODO: cambiar a sin const para ver que pasa con la flash
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
