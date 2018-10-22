//---------------------------------------------
// #### PROYECTO DEXEL 6CH BIDIRECCIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MAIN.C ################################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "hard.h"
#include "stm32f0xx.h"
#include "gpio.h"
#include "uart.h"

#include "core_cm0.h"
#include "adc.h"
#include "tim.h"

#include "comm.h"
#include "dsp.h"

#include "lcd.h"
#include "dmx_transceiver.h"
#include "menues.h"
#include "modo_slave.h"
#include "lcd_utils.h"
#include "programs_functions.h"

#include "flash_program.h"

#include <stdio.h>
#include <string.h>




//--- VARIABLES EXTERNAS ---//
// ------- Externals de la Memoria y los modos -------
parameters_typedef * pmem = (parameters_typedef *) (unsigned int *) FLASH_PAGE_FOR_BKP;	//en flash
parameters_typedef mem_conf;

// ------- Externals del ADC -------
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];
volatile unsigned char seq_ready;

// ------- Externals de los timers -------
volatile unsigned char timer_1seg = 0;
volatile unsigned char switches_timer = 0;
#ifdef USE_DELTA_FUNCTION
volatile unsigned short timer_delta_filter = 0;
#endif
// ------- Externals del USART -------
volatile unsigned char usart1_have_data;
volatile unsigned char usart2_have_data;

// ------- Externals del DMX -------
volatile unsigned char Packet_Detected_Flag;
volatile unsigned char DMX_packet_flag;
volatile unsigned char RDM_packet_flag;
volatile unsigned char dmx_receive_flag = 0;
volatile unsigned short DMX_channel_received = 0;
volatile unsigned short DMX_channel_selected = 1;
volatile unsigned char DMX_channel_quantity = 4;
volatile unsigned char dmx_timeout_timer = 0;
volatile unsigned char dmx_filters_timer = 0;

volatile unsigned char data512[SIZEOF_DMX_DATA512];
//static unsigned char data_back[10];
volatile unsigned char data7[SIZEOF_DMX_DATA7];
volatile unsigned char * pdmx;

//para el PID en hard.c
unsigned short sp1_filtered = 0;
unsigned short sp2_filtered = 0;
unsigned short sp3_filtered = 0;
unsigned short sp4_filtered = 0;
unsigned short sp5_filtered = 0;
unsigned short sp6_filtered = 0;

//--- VARIABLES GLOBALES ---//
//para pruebas mantener esto en memoria
// parameters_typedef const parameters_typedef_constant =
parameters_typedef __attribute__ ((section("memParams1"))) const parameters_typedef_constant =
    {
        .program_type = SLAVE_MODE,

        .master_enable = 1,
        
        .last_program_in_flash = 9,
        .last_program_deep_in_flash = 0,

        .dmx_channel = 1,
        .dmx_channel_quantity = 6,
        .dmx_grandmaster = 0,        
        
        .max_current_int = 2,
        .max_current_dec = 0,

        .volts_in_mains = 35,
        .volts_ch1 = 35,
        .volts_ch2 = 35,
        .volts_ch3 = 35,
        .volts_ch4 = 35,
        .volts_ch5 = 35,
        .volts_ch6 = 35,

        .max_pwm_ch1 = DUTY_90_PERCENT,
        .max_pwm_ch2 = DUTY_90_PERCENT,
        .max_pwm_ch3 = DUTY_90_PERCENT,
        .max_pwm_ch4 = DUTY_90_PERCENT,
        .max_pwm_ch5 = DUTY_90_PERCENT,
        .max_pwm_ch6 = DUTY_90_PERCENT
        
    };

//OJO!!! este es el dummy en memoria
parameters_typedef __attribute__ ((section("memParams2"))) const parameters_typedef_dummys =
    {
        .program_type = 2,    

        .master_enable = 2,
        
        .last_program_in_flash = 2,
        .last_program_deep_in_flash = 2,

        .dmx_channel = 2,
        .dmx_channel_quantity = 2,
        .dmx_grandmaster = 0,        
        
        .max_current_int = 2,
        .max_current_dec = 0,

        .volts_in_mains = 2,
        .volts_ch1 = 2,
        .volts_ch2 = 2,
        .volts_ch3 = 2,
        .volts_ch4 = 2,
        .volts_ch5 = 2,
        .volts_ch6 = 2,

        .max_pwm_ch1 = 2,
        .max_pwm_ch2 = 2,
        .max_pwm_ch3 = 2,
        .max_pwm_ch4 = 2,
        .max_pwm_ch5 = 2,
        .max_pwm_ch6 = 2
        
    };



// ------- de los timers -------
volatile unsigned short timer_standby;
volatile unsigned short wait_ms_var = 0;
volatile unsigned char temp_sample_timer = 0;
volatile unsigned short need_to_save_timer = 0;

// ------- para la memoria -------
unsigned char need_to_save = 0;

//--- FUNCIONES DEL MODULO ---//
extern void EXTI4_15_IRQHandler(void);
void TimingDelay_Decrement(void);
void DMAConfig(void);

// ------- del DMX -------
// extern void EXTI4_15_IRQHandler(void);

// ------- para el DMA -------
#define RCC_DMA_CLK (RCC->AHBENR & RCC_AHBENR_DMAEN)
#define RCC_DMA_CLK_ON 		RCC->AHBENR |= RCC_AHBENR_DMAEN
#define RCC_DMA_CLK_OFF 	RCC->AHBENR &= ~RCC_AHBENR_DMAEN

const char s_blank_line [] = {"                "};
//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    unsigned short i = 0;
    char s_to_send [100];
    main_state_t main_state = MAIN_INIT;
    resp_t resp = resp_continue;
    unsigned char loop_count;
    
    //GPIO Configuration.
    GPIO_Config();

    //ACTIVAR SYSTICK TIMER
    if (SysTick_Config(48000))
    {
        while (1)	/* Capture error */
        {
            // if (LED)
            //     LED_OFF;
            // else
            //     LED_ON;

            for (i = 0; i < 255; i++)
            {
                asm (	"nop \n\t"
                        "nop \n\t"
                        "nop \n\t" );
            }
        }
    }


    //pruebas hard//
    USART2Config();

    TIM_1_Init();
    TIM_3_Init();

    Update_TIM1_CH1(0);
    Update_TIM1_CH2(0);    
    Update_TIM3_CH1(0);
    Update_TIM3_CH2(0);
    Update_TIM3_CH3(0);
    Update_TIM3_CH4(0);


    //-- Prueba con ADC INT ----------
    //-- ADC configuration.
    // AdcConfig();
    // ADC1->CR |= ADC_CR_ADSTART;

    // while (1)
    // {
    //     if (!timer_standby)
    //     {
    //         timer_standby = 1000;
    //         sprintf(s_to_send, "i1: %d, i2: %d, i3: %d, i4: %d, i5: %d, i6: %d, t: %d\n",
    //                 I_Channel_1,
    //                 I_Channel_2,
    //                 I_Channel_3,
    //                 I_Channel_4,
    //                 I_Channel_5,
    //                 I_Channel_6,
    //                 Temp_Channel);

    //         Usart2Send(s_to_send);
    //     }
    // }
    //-- Fin Prueba con ADC INT ----------    

    //-- Prueba con LCD ----------
    LCDInit();


    //--- Welcome code ---//
    Lcd_Command(CLEAR);
    Wait_ms(100);
    Lcd_Command(CURSOR_OFF);
    Wait_ms(100);
    Lcd_Command(BLINK_OFF);
    Wait_ms(100);
    CTRL_BKL_ON;

    LCDTransmitStr(s_blank_line);

    // LCD_1ER_RENGLON;
    // LCDTransmitStr("Dexel   ");
    // LCD_2DO_RENGLON;
    // LCDTransmitStr("Lighting");
    while (FuncShowBlink ((const char *) "Kirno 6C", (const char *) "Smrt Drv", 1, BLINK_NO) == resp_continue);
    while (FuncShowBlink ((const char *) "Dexel   ", (const char *) "Lighting", 1, BLINK_NO) == resp_continue);


    // while (1);
    //-- Fin Prueba con LCD ----------

    //--- Mensaje Bienvenida ---//
    //---- Defines from hard.h -----//
    Usart2Send("\nDexel RGB 6CH Bidireccional\n -- powered by: Kirno Technology --\n");
    Wait_ms(100);
#ifdef HARD
    Usart2Send(HARD);
    Wait_ms(100);    
#else
#error	"No Hardware defined in hard.h file"
#endif

#ifdef SOFT
    Usart2Send(SOFT);
    Wait_ms(100);    
#else
#error	"No Soft Version defined in hard.h file"
#endif


    //---- End of Defines from hard.h -----//

    //-- Prueba de Switches S1 y S2 ----------
    // unsigned char check_s1 = 0, check_s2 = 0;
    // while (1)
    // {
    //     if ((CheckS1()) && (check_s1 == 0))
    //     {
    //         check_s1 = 1;
    //         Usart2Send("S1\n");
    //     }
    //     else if ((!CheckS1()) && (check_s1))
    //     {
    //         check_s1 = 0;
    //         Usart2Send("not S1\n");
    //     }
                            
    //     if ((CheckS2()) && (check_s2 == 0))
    //     {
    //         check_s2 = 1;
    //         Usart2Send("S2\n");
    //     }
    //     else if ((!CheckS2()) && (check_s2))
    //     {
    //         check_s2 = 0;
    //         Usart2Send("not S2\n");
    //     }

    //     UpdateSwitches();
    // }
    //-- Fin Prueba de Switches S1 y S2 ----------


    //-- Prueba de Switch RX-TX DMX512 ----------
    // USART1Config();
    
    // while (1)
    // {
    //     SW_RX_TX_DE;
    //     Wait_ms(4);
    //     SW_RX_TX_RE_NEG;
    //     Wait_ms(2);
    // }
    //-- Fin Prueba de Switch RX-TX DMX512 ----------

    //-- Prueba de Switch USART1 DMX512 PIN TX ----------
    // TIM_16_Init();
    // USART1Config();
    // SW_RX_TX_DE;
    
    // while (1)
    // {
    //     if (!timer_standby)
    //     {
    //         timer_standby = 40;
    //         data512[0] = 0;
    //         data512[1] = 2;
    //         data512[2] = 0;
    //         data512[511] = 0x55;
    //         SendDMXPacket (PCKT_INIT);
    //     }
    // }
    //-- Fin Prueba de Switch USART1 DMX512 PIN TX ----------    

    //-- Prueba con DMX512 ----------
    // TIM_14_Init();
    // USART1Config();

    // Packet_Detected_Flag = 0;
    // DMX_channel_selected = 1;
    // DMX_channel_quantity = 6;

    // for (i = 0; i < 8; i++)
    // {
    //     v_sp1[i] = 0;
    //     v_sp2[i] = 0;
    //     v_sp3[i] = 0;
    //     v_sp4[i] = 0;
    //     v_sp5[i] = 0;
    //     v_sp6[i] = 0;
    // }
        
    // SW_RX_TX_OFF;
    // DMX_Ena();

    // while (1)
    // {
    //     if (!timer_standby)
    //     {
    //         timer_standby = 1000;
    //         sprintf(s_to_send, "c0: %d, c1: %d, c2: %d, c3: %d, c4: %d, c5: %d, c6: %d\n",
    //                 data7[0],
    //                 data7[1],
    //                 data7[2],
    //                 data7[3],
    //                 data7[4],
    //                 data7[5],
    //                 data7[6]);

    //         Usart2Send(s_to_send);
    //     }

    //     if (dmx_receive_flag)
    //         CTRL_FAN_ON;
    //     else
    //         CTRL_FAN_OFF;
    // }    
    //-- Fin Prueba con DMX512 ----------

    //-- Prueba con ADC & DMA & PWM Fijo y mido I ----------
    //inicializo el hard que falta
    DMX_Disa();
    AdcConfig();

    //-- DMA configuration.
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    ADC1->CR |= ADC_CR_ADSTART;

    loop_count = 0;
    i = 0;
    while(1)
    {
        if (DMA1->ISR & DMA_ISR_TCIF1)    //esto es sequence ready cada 16KHz
            DMA1->IFCR = DMA_ISR_TCIF1;
        
        if (!timer_standby)
        {
            timer_standby = 500;
            //envio corriente y pwm de canal 1
            sprintf(s_to_send, "d1: %d, i1: %d\n",
                    i,
                    I_Channel_1);

            Usart2Send(s_to_send);
            if (loop_count >= 9)
            {
                loop_count = 0;
                if (i > 100)
                    i = 10;
                // if (i > 4)
                //     i = 0;
                else
                    i++;
                Update_PWM1(i);
            }
            else
                loop_count++;
        }
    }
    //-- Fin Prueba con ADC & DMA & PWM Fijo y mido I ----------
    
    //-- Prueba con ADC & DMA ----------
    //-- ADC configuration.
// #ifdef ADC_WITH_DMA
//     AdcConfig();

//     //-- DMA configuration.
//     DMAConfig();
//     DMA1_Channel1->CCR |= DMA_CCR_EN;

//     ADC1->CR |= ADC_CR_ADSTART;
    
//     // Prueba ADC & DMA
//     while(1)
//     {
//         if (DMA1->ISR & DMA_ISR_TCIF1)    //esto es sequence ready
//         {
//             // Clear DMA TC flag
//             DMA1->IFCR = DMA_ISR_TCIF1;

//             if (undersampling < (PID_UNDERSAMPLING - 1))
//             {
//                 undersampling++;
//             }
//             else
//             {
//                 undersampling = 0;
//                 if (CTRL_FAN)
//                     CTRL_FAN_OFF;
//                 else
//                     CTRL_FAN_ON;

//                 //PID CH1
//                 if (!sp1_filtered)
//                     Update_PWM1(0);
//                 else
//                 {
//                     d_ch1 = PID_roof (sp1_filtered, I_Channel_1, d_ch1, &e_z1_ch1, &e_z2_ch1);

//                     if (d_ch1 < 0)
//                         d_ch1 = 0;
//                     else
//                     {
//                         if (d_ch1 > DUTY_90_PERCENT)
//                             d_ch1 = DUTY_90_PERCENT;
                    
//                         Update_PWM1(d_ch1);
//                     }
//                 }

//                 //PID CH2
//                 if (!sp2_filtered)
//                     Update_PWM2(0);
//                 else
//                 {                
//                     d_ch2 = PID_roof (sp2_filtered, I_Channel_2, d_ch2, &e_z1_ch2, &e_z2_ch2);

//                     if (d_ch2 < 0)
//                         d_ch2 = 0;
//                     else
//                     {
//                         if (d_ch2 > DUTY_90_PERCENT)
//                             d_ch2 = DUTY_90_PERCENT;
                    
//                         Update_PWM2(d_ch2);
//                     }
//                 }

//                 //PID CH3
//                 if (!sp3_filtered)
//                     Update_PWM3(0);
//                 else
//                 {                                
//                     d_ch3 = PID_roof (sp3_filtered, I_Channel_3, d_ch3, &e_z1_ch3, &e_z2_ch3);

//                     if (d_ch3 < 0)
//                         d_ch3 = 0;
//                     else
//                     {
//                         if (d_ch3 > DUTY_90_PERCENT)
//                             d_ch3 = DUTY_90_PERCENT;
                    
//                         Update_PWM3(d_ch3);
//                     }
//                 }

//                 //PID CH4
//                 if (!sp4_filtered)
//                     Update_PWM4(0);
//                 else
//                 {
//                     d_ch4 = PID_roof (sp4_filtered, I_Channel_4, d_ch4, &e_z1_ch4, &e_z2_ch4);

//                     if (d_ch4 < 0)
//                         d_ch4 = 0;
//                     else
//                     {
//                         if (d_ch4 > DUTY_90_PERCENT)
//                             d_ch4 = DUTY_90_PERCENT;
                    
//                         Update_PWM4(d_ch4);
//                     }
//                 }

//                 //PID CH5
//                 if (!sp5_filtered)
//                     Update_PWM5(0);
//                 else
//                 {                
//                     d_ch5 = PID_roof (sp5_filtered, I_Channel_5, d_ch5, &e_z1_ch5, &e_z2_ch5);

//                     if (d_ch5 < 0)
//                         d_ch5 = 0;
//                     else
//                     {
//                         if (d_ch5 > DUTY_90_PERCENT)
//                             d_ch5 = DUTY_90_PERCENT;
                    
//                         Update_PWM5(d_ch5);
//                     }
//                 }

//                 //PID CH6
//                 if (!sp6_filtered)
//                     Update_PWM6(0);
//                 else
//                 {                                
//                     d_ch6 = PID_roof (sp6_filtered, I_Channel_6, d_ch6, &e_z1_ch6, &e_z2_ch6);

//                     if (d_ch6 < 0)
//                         d_ch6 = 0;
//                     else
//                     {
//                         if (d_ch6 > DUTY_90_PERCENT)
//                             d_ch6 = DUTY_90_PERCENT;
                    
//                         Update_PWM6(d_ch6);
//                     }
//                 }               
//             }
//         }

//         //me fijo si hubo overrun
//         if (ADC1->ISR & ADC_IT_OVR)
//         {
//             ADC1->ISR |= ADC_IT_EOC | ADC_IT_EOSEQ | ADC_IT_OVR;
//             Usart2Send("over\n");
//         }

//         if (!timer_standby)
//         {
//             timer_standby = 1000;
//             //envio corrientes
//             sprintf(s_to_send, "i1: %d, i2: %d, i3: %d, i4: %d, i5: %d, i6: %d, t: %d\n",
//                     I_Channel_1,
//                     I_Channel_2,
//                     I_Channel_3,
//                     I_Channel_4,
//                     I_Channel_5,
//                     I_Channel_6,
//                     Temp_Channel);

//             Usart2Send(s_to_send);

//             //envio canales dmx
//             sprintf(s_to_send, "c0: %d, c1: %d, c2: %d, c3: %d, c4: %d, c5: %d, c6: %d\n",
//                     data7[0],
//                     data7[1],
//                     data7[2],
//                     data7[3],
//                     data7[4],
//                     data7[5],
//                     data7[6]);

//             Usart2Send(s_to_send);
            
//         }

//         //update del dmx
//         if (Packet_Detected_Flag)
//         {
//             Packet_Detected_Flag = 0;

//             //CH1
//             dummysp = data7[1];
//             dummysp <<= 2;
//             if (dummysp > 820)
//                 sp1 = 820;
//             else
//                 sp1 = dummysp;

//             //CH2
//             dummysp = data7[2];
//             dummysp <<= 2;
//             if (dummysp > 820)
//                 sp2 = 820;
//             else
//                 sp2 = dummysp;

//             //CH3
//             dummysp = data7[3];
//             dummysp <<= 2;
//             if (dummysp > 820)
//                 sp3 = 820;
//             else
//                 sp3 = dummysp;

//             //CH4
//             dummysp = data7[4];
//             dummysp <<= 2;
//             if (dummysp > 820)
//                 sp4 = 820;
//             else
//                 sp4 = dummysp;

//             //CH5
//             dummysp = data7[5];
//             dummysp <<= 2;
//             if (dummysp > 820)
//                 sp5 = 820;
//             else
//                 sp5 = dummysp;

//             //CH6
//             dummysp = data7[6];
//             dummysp <<= 2;
//             if (dummysp > 820)
//                 sp6 = 820;
//             else
//                 sp6 = dummysp;            
            
//         }

//         //filters para el dmx
//         if (!dmx_filters_timer)
//         {
//             dmx_filters_timer = 5;

//             sp1_filtered = MAFilterFast (sp1, v_sp1);
//             sp2_filtered = MAFilterFast (sp2, v_sp2);
//             sp3_filtered = MAFilterFast (sp3, v_sp3);
//             sp4_filtered = MAFilterFast (sp4, v_sp4);
//             sp5_filtered = MAFilterFast (sp5, v_sp5);
//             sp6_filtered = MAFilterFast (sp6, v_sp6);
//         }


                
        

//         // UpdateSwitches();

//         // if (CheckS1() && (!check_s1))
//         // {
//         //     check_s1 = 1;
//         //     Usart2Send("S1\n");
//         // }
        
//         // if ((CheckS2()) && (check_s1))
//         // {
//         //     check_s1 = 0;
//         //     Usart2Send("not S1\n");
//         // }
        
//     }
// #endif
    //-- Prueba con ADC & DMA ----------

    //-- Programa de Produccion del DMX
    //inicializo dmx si todavia no lo hice
    TIM_14_Init();    //para detectar break en dmx
    TIM_16_Init();    //para tx dmx OneShoot
    USART1Config();

    // Packet_Detected_Flag = 0;
    // DMX_channel_selected = 1;
    // DMX_channel_quantity = 6;
#ifdef USE_DELTA_FUNCTION
    TIM_17_Init();
#endif

    
    //inicializo el hard que falta
    AdcConfig();

    //-- DMA configuration.
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    ADC1->CR |= ADC_CR_ADSTART;


    while (1)
    {
        switch (main_state)
        {
        case MAIN_INIT:
            memcpy(&mem_conf, pmem, sizeof(parameters_typedef));

            main_state++;
            break;

        case MAIN_HARDWARE_INIT:

            //reseteo hardware
            //DMX en RX
            SW_RX_TX_RE_NEG;
            DMX_Disa();

            //reseteo canales
            Update_PWM1(0);
            Update_PWM2(0);    
            Update_PWM3(0);
            Update_PWM4(0);
            Update_PWM5(0);
            Update_PWM6(0);
                        
            //reseteo menues
            MasterModeMenuReset();
            FuncSlaveModeReset();

            sprintf(s_to_send, "prog type: %d\n", mem_conf.program_type);
            Usart2Send(s_to_send);
            Wait_ms(100);
            sprintf(s_to_send, "Max pwm channels: %d %d %d %d %d %d\n",
                    mem_conf.max_pwm_ch1,
                    mem_conf.max_pwm_ch2,
                    mem_conf.max_pwm_ch3,
                    mem_conf.max_pwm_ch4,
                    mem_conf.max_pwm_ch5,
                    mem_conf.max_pwm_ch6);
            
            Usart2Send(s_to_send);
            Wait_ms(100);
            
            main_state++;            
            break;

        case MAIN_GET_CONF:
            if (mem_conf.program_type == MASTER_MODE)
            {
                //habilito transmisiones
                SW_RX_TX_DE;
                DMX_Ena();                    
                main_state = MAIN_IN_MASTER_MODE;             
            }                

            if (mem_conf.program_type == SLAVE_MODE)
            {
                //variables de recepcion
                Packet_Detected_Flag = 0;
                DMX_channel_selected = mem_conf.dmx_channel;
                DMX_channel_quantity = mem_conf.dmx_channel_quantity;

                //habilito recepcion
                SW_RX_TX_RE_NEG;
                DMX_Ena();    
                main_state = MAIN_IN_SLAVE_MODE;
            }

            if (mem_conf.program_type == PROGRAMS_MODE)
            {
                //me aseguro no cargar la linea
                SW_RX_TX_RE_NEG;
                main_state = MAIN_IN_PROGRAMS_MODE;
            }

            //default state no debiera estar nunca aca!
            if (main_state == MAIN_GET_CONF)
            {
                mem_conf.program_type = SLAVE_MODE;
                main_state = MAIN_IN_SLAVE_MODE;
            }                
            break;

        case MAIN_IN_MASTER_MODE:    //por ahora programs mode
            Func_PX(mem_conf.last_program_in_flash, mem_conf.last_program_deep_in_flash);
            UpdateSamplesAndPID();
            if (CheckS2() > S_HALF)
                main_state = MAIN_ENTERING_MAIN_MENU;

            MasterModeMenu();
            if (!timer_standby)
            {
                timer_standby = 40;
                SendDMXPacket (PCKT_INIT);
            }
            break;
            
        case MAIN_IN_SLAVE_MODE:
            FuncSlaveMode();
            UpdateSamplesAndPID ();
            if (!timer_standby)
            {
                timer_standby = 1000;
                //envio corrientes
                sprintf(s_to_send, "i1: %d, c1: %d, sp1: %d\n",
                        I_Channel_1,
                        data7[1],
                        sp1_filtered);
                Usart2Send(s_to_send);
            }

            if (CheckS2() > S_HALF)
                main_state = MAIN_ENTERING_MAIN_MENU;

            break;

        case MAIN_IN_PROGRAMS_MODE:
            Func_PX(mem_conf.last_program_in_flash, mem_conf.last_program_deep_in_flash);
            UpdateSamplesAndPID();

            if (CheckS2() > S_HALF)
                main_state = MAIN_ENTERING_MAIN_MENU;

            ProgramsModeMenu();
            
            break;

        case MAIN_IN_OVERTEMP:
            CTRL_FAN_ON;
            Update_PWM1(0);
            Update_PWM2(0);
            Update_PWM3(0);
            Update_PWM4(0);
            Update_PWM5(0);
            Update_PWM6(0);

            LCD_1ER_RENGLON;
            LCDTransmitStr("OVERTEMP");
            LCD_2DO_RENGLON;
            LCDTransmitStr(s_blank_line);

            sprintf(s_to_send, "overtemp: %d\n", Temp_Channel);
            Usart2Send(s_to_send);

            main_state = MAIN_IN_OVERTEMP_B;
            break;

        case MAIN_IN_OVERTEMP_B:
            if (Temp_Channel < TEMP_IN_50)
            {
                //reconecto
                main_state = MAIN_HARDWARE_INIT;
            }
            
            break;
            
        case MAIN_ENTERING_MAIN_MENU:
            //deshabilitar salidas hardware
            SW_RX_TX_RE_NEG;
            DMX_Disa();

            //reseteo canales
            Update_TIM1_CH1(0);
            Update_TIM1_CH2(0);    
            Update_TIM3_CH1(0);
            Update_TIM3_CH2(0);
            Update_TIM3_CH3(0);
            Update_TIM3_CH4(0);

            MainMenuReset();
            main_state++;

            break;

        case MAIN_IN_MAIN_MENU:
            resp = MainMenu();

            if (resp == resp_need_to_save)
            {
                need_to_save = 1;
                need_to_save_timer = 10000;
                main_state = MAIN_HARDWARE_INIT;
            }
            
            if (resp == resp_finish)
                main_state = MAIN_HARDWARE_INIT;

            break;
            
        default:
            main_state = MAIN_INIT;
            break;
        }

        //cuestiones generales
        
        UpdateSwitches();

        //sensado de temperatura
        if (!temp_sample_timer)
        {
            temp_sample_timer = 10;	//tomo muestra cada 10ms

            if ((main_state != MAIN_IN_OVERTEMP) && (main_state != MAIN_IN_OVERTEMP_B))
            {
                if (Temp_Channel > TEMP_IN_65)
                {
                    //corto los leds	ver si habia DMX cortar y poner nuevamente
                    main_state = MAIN_IN_OVERTEMP;
                }
                else if (Temp_Channel > TEMP_IN_35)
                    CTRL_FAN_ON;
                else if (Temp_Channel < TEMP_IN_30)
                    CTRL_FAN_OFF;
            }
        }

        //grabado de memoria luego de configuracion
        if ((need_to_save) && (!need_to_save_timer))
        {
            
            need_to_save = WriteConfigurations();

            if (need_to_save)
                Usart2Send((char *) "Memory Saved OK!\n");
            else
                Usart2Send((char *) "Memory problems\n");

            need_to_save = 0;
            //update de memoria RAM
            // memcpy(&mem_conf, pmem, sizeof(parameters_typedef));
        }
        
    }    //end of while 1
    
    return 0;
}
//--- End of Main ---//

void DMAConfig(void)
{
    /* DMA1 clock enable */
    if (!RCC_DMA_CLK)
        RCC_DMA_CLK_ON;

    //Configuro el control del DMA CH1
    DMA1_Channel1->CCR = 0;
    //priority very high
    //memory halfword
    //peripheral halfword
    //increment memory
    DMA1_Channel1->CCR |= DMA_CCR_PL | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_MINC;
    //DMA1_Channel1->CCR |= DMA_Mode_Circular | DMA_CCR_TCIE;
    //cicular mode
    DMA1_Channel1->CCR |= DMA_CCR_CIRC;

    //Tama�o del buffer a transmitir
    DMA1_Channel1->CNDTR = ADC_CHANNEL_QUANTITY;

    //Address del periferico
    DMA1_Channel1->CPAR = (uint32_t) &ADC1->DR;

    //Address en memoria
    DMA1_Channel1->CMAR = (uint32_t) &adc_ch[0];

    //Enable
    //DMA1_Channel1->CCR |= DMA_CCR_EN;
}

void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    // if (timer_signals)
    //     timer_signals--;

    // if (timer_signals_gen)
    //     timer_signals_gen--;

    if (need_to_save_timer)
        need_to_save_timer--;

    if (temp_sample_timer)
        temp_sample_timer--;

    if (switches_timer)
        switches_timer--;

    if (dmx_filters_timer)
        dmx_filters_timer--;

    if (dmx_timeout_timer)
        dmx_timeout_timer--;
    else
        EXTIOn();    //dejo 20ms del paquete sin INT

    //para lcd_utils
    UpdateTimerLCD ();

    //para modo_slave
    UpdateTimerSlaveMode();

    //para programas
    UpdateProgTimers ();

    //para main menu
    UpdateTimerModeMenu ();
}

void EXTI4_15_IRQHandler (void)    //nueva detecta el primer 0 en usart Consola PHILIPS
{
    if(EXTI->PR & 0x0100)	//Line8
    {
        DmxInt_Break_Handler();
        EXTI->PR |= 0x0100;
    }
}

//--- end of file ---//

