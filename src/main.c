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
volatile unsigned short timer_signals = 0;
volatile unsigned short timer_signals_gen = 0;
volatile unsigned short timer_led = 0;
volatile unsigned short timer_buzzer = 0;
volatile unsigned char switches_timer = 0;

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
parameters_typedef __attribute__ ((section("memParams"))) const parameters_typedef_constant =
    {
        .program_type = 1,
        .last_program_in_flash = 9,
        .last_program_deep_in_flash = 0,

        .dmx_channel = 1,
        .dmx_channel_quantity = 6,
        .dmx_grandmaster = 0,        
        
        .max_current_ch1 = 10,
        .max_current_ch2 = 0,
        .max_current_ch3 = 0,
        .max_current_ch4 = 0,
        .max_current_ch5 = 0,
        .max_current_ch6 = 0,                

        .max_pwm_ch1 = 10,
        .max_pwm_ch2 = 0,
        .max_pwm_ch3 = 0,
        .max_pwm_ch4 = 0,
        .max_pwm_ch5 = 0,
        .max_pwm_ch6 = 0
        
    };



// ------- de los timers -------
volatile unsigned short timer_standby;
volatile unsigned short wait_ms_var = 0;

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
    unsigned char size = 0;
    main_state_t main_state = MAIN_INIT;
    resp_t resp = resp_continue;


    unsigned char check_s1 = 0, check_s2 = 0;
    
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

    LCD_1ER_RENGLON;
    LCDTransmitStr("MAXI");
    LCD_2DO_RENGLON;
    LCDTransmitStr("Gagliardi");

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
    TIM_14_Init();
    USART1Config();

    Packet_Detected_Flag = 0;
    DMX_channel_selected = 1;
    DMX_channel_quantity = 6;

    SW_RX_TX_OFF;
    DMX_Ena();
    
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
            sprintf(s_to_send, "prog type: %d\n", mem_conf.program_type);
            Usart2Send(s_to_send);
            Wait_ms(100);

            sprintf(s_to_send, "last prg: %d\n", mem_conf.last_program_in_flash);
            Usart2Send(s_to_send);
            Wait_ms(100);

            sprintf(s_to_send, "last prog deep: %d\n", mem_conf.last_program_deep_in_flash);
            Usart2Send(s_to_send);
            Wait_ms(100);
            
            main_state++;
            break;

        case MAIN_HARDWARE_INIT:
            // Func_PX_Reset();    //programs no necesita reset
            FuncSlaveModeReset();
            //master mode reset()
            main_state++;            
            break;

        case MAIN_GET_CONF:
            if (mem_conf.program_type == MASTER_MODE)
                main_state = MAIN_IN_MASTER_MODE;

            if (mem_conf.program_type == SLAVE_MODE)
                main_state = MAIN_IN_SLAVE_MODE;

            if (mem_conf.program_type == PROGRAMS_MODE)
                main_state = MAIN_IN_PROGRAMS_MODE;

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
            
            break;
            
        case MAIN_IN_SLAVE_MODE:
            FuncSlaveMode();
            if (!timer_standby)
            {
                timer_standby = 1000;
                //envio corrientes
                sprintf(s_to_send, "i1: %d, i2: %d, i3: %d, i4: %d, i5: %d, i6: %d, t: %d\n",
                        I_Channel_1,
                        I_Channel_2,
                        I_Channel_3,
                        I_Channel_4,
                        I_Channel_5,
                        I_Channel_6,
                        Temp_Channel);

                Usart2Send(s_to_send);

                //envio canales dmx
                sprintf(s_to_send, "c0: %d, c1: %d, c2: %d, c3: %d, c4: %d, c5: %d, c6: %d\n",
                        data7[0],
                        data7[1],
                        data7[2],
                        data7[3],
                        data7[4],
                        data7[5],
                        data7[6]);
                
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

            break;

        case MAIN_IN_OVERTEMP:
            break;

        case MAIN_ENTERING_MAIN_MENU:
            resp = MainMenu();

            if (resp == resp_ok)    //ver resp_finish
            {
                

            }
            break;
            
        default:
            main_state = MAIN_INIT;
            break;
        }

        //cuestiones generales
        
        UpdateSwitches();
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

    //Tamaño del buffer a transmitir
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

    // if (timer_led)
    //     timer_led--;

    // if (timer_buzzer)
    //     timer_buzzer--;

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

