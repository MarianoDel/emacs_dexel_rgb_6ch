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
#include "signals.h"

#include "lcd.h"
#include "dmx_transceiver.h"

#include <stdio.h>
//#include <string.h>




//--- VARIABLES EXTERNAS ---//
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

volatile unsigned char data512[SIZEOF_DMX_DATA512];
//static unsigned char data_back[10];
volatile unsigned char data7[SIZEOF_DMX_DATA7];


//--- VARIABLES GLOBALES ---//


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
#ifdef ADC_WITH_DMA
    unsigned char onsync = 0;
#endif

    unsigned char check_s1 = 0, check_s2 = 0;
    
    // unsigned char bytes_readed = 0;

    //GPIO Configuration.
    GPIO_Config();

    //TIM Configuration.
    // TIM_3_Init();
    // TIM_14_Init();

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

    // while (1)
    // {
    //     if (usart2_have_data)
    //     {
    //         usart2_have_data = 0;
    //         size = ReadUsart2Buffer(s_to_send, sizeof(s_to_send));
    //         s_to_send[size - 1] = '\n';
    //         s_to_send[size] = '\0';
    //         Usart2Send(s_to_send);
    //     }
    // }


    // while (1)
    // {                
    //     Wait_ms(1000);
    //     Usart2Send("Hola\n");
    // }

    // while (1)
    // {
    //     Wait_ms(1);
    //     TIM_CH1_ON;
    //     TIM_CH2_ON;
    //     Wait_ms(1);
    //     TIM_CH1_OFF;
    //     TIM_CH2_OFF;        
    // }
    TIM_1_Init();
    TIM_3_Init();


    Update_TIM1_CH1(100);
    Update_TIM1_CH2(100);    
    Update_TIM3_CH1(100);
    Update_TIM3_CH2(100);
    Update_TIM3_CH3(100);
    Update_TIM3_CH4(100);

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

    //-- Prueba con DMX512 ----------
    // TIM_14_Init();
    // USART1Config();

    // Packet_Detected_Flag = 0;
    // DMX_channel_selected = 1;
    // DMX_channel_quantity = 6;

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
#ifdef ADC_WITH_DMA
    AdcConfig();
    ADC1->CR |= ADC_CR_ADSTART;

    //-- DMA configuration.
    DMAConfig();

    // Prueba ADC & DMA
    while(1)
    {
        //busco sync con DMA
        while (!onsync)
        {
            if (ADC1->ISR & ADC_IT_EOC)
            {
                DMA1_Channel1->CCR |= DMA_CCR_EN;
                ADC1->ISR |= ADC_IT_EOC;
                onsync = 1;
            }
        }

        if (DMA1->ISR & DMA_ISR_TCIF1)    //esto es sequence ready
        {
            // Clear DMA TC flag
            DMA1->IFCR = DMA_ISR_TCIF1;

        }

        //me fijo si hubo overrun
        if (ADC1->ISR & ADC_IT_OVR)
        {
            ADC1->ISR |= ADC_IT_EOC | ADC_IT_EOSEQ | ADC_IT_OVR;
            Usart2Send("over\n");
        }

        if (!timer_standby)
        {
            timer_standby = 1000;
            sprintf(s_to_send, "i1: %d, i2: %d, i3: %d, i4: %d, i5: %d, i6: %d, t: %d\n",
                    I_Channel_1,
                    I_Channel_2,
                    I_Channel_3,
                    I_Channel_4,
                    I_Channel_5,
                    I_Channel_6,
                    Temp_Channel);

            Usart2Send(s_to_send);
        }            
    }
#endif
    //-- Prueba con ADC & DMA ----------




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

        
    



    //prueba modulo signals.c comm.c tim.c adc.c

    Update_TIM3_CH1(0);
    Update_TIM3_CH2(0);
    Update_TIM3_CH3(0);
    Update_TIM3_CH4(0);

    AdcConfig();
    ADC1->CR |= ADC_CR_ADSTART;
    
    // TIM_14_Init();
    // UpdateLaserCh1(0);
    // UpdateLaserCh2(0);
    // UpdateLaserCh3(0);
    // UpdateLaserCh4(0);

    // USART1Config();

    //--- Mensaje Bienvenida ---//
    //---- Defines from hard.h -----//
#ifdef FIRST_POWER_BOARD
    Wait_ms(1000);
#endif

#ifdef SECOND_POWER_BOARD
    Wait_ms(2000);
#endif
    Usart1Send("\nLipoLaser -- powered by: Kirno Technology\n");
    Wait_ms(100);
#ifdef HARD
    Usart1Send(HARD);
    Wait_ms(100);    
#else
#error	"No Hardware defined in hard.h file"
#endif

#ifdef SOFT
    Usart1Send(SOFT);
    Wait_ms(100);    
#else
#error	"No Soft Version defined in hard.h file"
#endif
    Usart1Send("Features:\n");
#ifdef LED_AND_LASER_SAME_POWER
    Usart1Send((const char *)" Led and Lasers use same power\n");
#endif
#ifdef LED_AND_LASER_DIFFERENT_POWER
    Usart1Send((const char *)" Led and Lasers use differents power\n");
#endif

//---- End of Defines from hard.h -----//
    
    while (1)
    {        
        TreatmentManager();
        UpdateCommunications();
        UpdateLed();
        UpdateBuzzer();
    }
    //fin prueba modulo signals.c comm.c tim.c adc.c

    //prueba modulo adc.c tim.c e int adc
    // TIM_3_Init();
    // Update_TIM3_CH1(511);
    // Update_TIM3_CH2(0);
    // Update_TIM3_CH3(0);
    // Update_TIM3_CH4(0);

    // AdcConfig();
    // ADC1->CR |= ADC_CR_ADSTART;
    
    // while (1)
    // {
    //     if (seq_ready)
    //     {
    //         seq_ready = 0;
    //         if (LED)
    //             LED_OFF;
    //         else
    //             LED_ON;
    //     }
    // }               
    //fin prueba modulo adc.c tim.c e int adc

    //prueba modulo comm.c
    // USART1Config();
    // while (1)
    // {
    //     UpdateCommunications();
    // }
    // fin prueba modulo comm.c
        
    // //prueba PWM con TIM3
    // TIM_3_Init();

    // Update_TIM3_CH1(0);
    // Update_TIM3_CH2(0);
    // Update_TIM3_CH3(0);
    // Update_TIM3_CH4(0);    

    // while (1)
    // {
    //     for (i = 0; i < 1023; i++)
    //     {
    //         Update_TIM3_CH1(i);
    //         Wait_ms(10);
    //     }
    // }
    // // fin prueba int timer 14 y SOFT_PWM


    // Update_TIM3_CH1(511);
    // Update_TIM3_CH2(511);
    // Update_TIM3_CH3(511);
    // Update_TIM3_CH4(511);
    // while (1);
    
    // //prueba int timer 14 y SOFT_PWM
    // TIM_14_Init();

    // // UpdateLaserCh1(127);
    // // UpdateLaserCh2(127);
    // // UpdateLaserCh3(127);
    // // UpdateLaserCh4(127);

    // // while (1);


    // while (1)
    // {
    //     for (i = 0; i < 255; i++)
    //     {
    //         UpdateLaserCh1(i);
    //         UpdateLaserCh2(i);
    //         UpdateLaserCh3(i);
    //         UpdateLaserCh4(i);

    //         Wait_ms(100);
    //     }
    // }
    // fin prueba int timer 14 y SOFT_PWM

    //prueba loop (Tx - Rx) en usart1
    // USART1Config();
    // while (1)
    // {
    //     if (usart1_have_data)
    //     {
    //         usart1_have_data = 0;
    //         bytes_readed = ReadUsart1Buffer((unsigned char *) s_to_senda, sizeof(s_to_senda));

    //         if ((bytes_readed + 1) < sizeof(s_to_senda))
    //         {
    //             *(s_to_senda + bytes_readed - 1) = '\n';
    //             *(s_to_senda + bytes_readed) = '\0';
    //             timer_standby = 1000;
    //         }

    //         if (LED)
    //             LED_OFF;
    //         else
    //             LED_ON;
    //     }

    //     if ((!timer_standby) && (bytes_readed > 0))
    //     {
    //         bytes_readed = 0;
    //         Usart1Send(s_to_senda);
    //     }
    // }
    //fin prueba loop (Tx - Rx) en usart1

    //Prueba USART TX
    // USART1Config();

    // while (1)
    // {
    //     LED_ON;
    //     Usart1Send("prueba\n");
    //     Wait_ms(100);
    //     LED_OFF;
    //     Wait_ms(1900);
    // }
    //Fin Prueba USART TX

    //PRUEBA LED Y BUZZER
    // while (1)
    // {
    //     if (BUZZER)
    //     {
    //         LED_OFF;
    //         BUZZER_OFF;
    //     }
    //     else
    //     {
    //         LED_ON;
    //         BUZZER_ON;
    //     }
    //     Wait_ms(200);

    //     // LED_ON;
    //     // BUZZER_ON;
    //     // Wait_ms(150);
    //     // LED_OFF;
    //     // BUZZER_OFF;
    //     // Wait_ms(2000);
    // }
    //FIN PRUEBA LED Y BUZZER


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

    if (timer_signals)
        timer_signals--;

    if (timer_signals_gen)
        timer_signals_gen--;

    if (timer_led)
        timer_led--;

    if (timer_buzzer)
        timer_buzzer--;

    if (switches_timer)
        switches_timer--;

    if (dmx_timeout_timer)
        dmx_timeout_timer--;
    else
        EXTIOn();    //dejo 20ms del paquete sin INT

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

