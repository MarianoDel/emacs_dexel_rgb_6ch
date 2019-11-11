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
#ifdef USE_FILTER_LENGHT_16
ma16_u16_data_obj_t st_sp1
ma16_u16_data_obj_t st_sp2
ma16_u16_data_obj_t st_sp3
ma16_u16_data_obj_t st_sp4
ma16_u16_data_obj_t st_sp5
ma16_u16_data_obj_t st_sp6
#endif


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
        .max_power = 40,
        
        .volts_ch[0] = 35,
        .volts_ch[1] = 35,
        .volts_ch[2] = 35,
        .volts_ch[3] = 35,
        .volts_ch[4] = 35,
        .volts_ch[5] = 35,

        .pwm_chnls[0] = DUTY_60_PERCENT,
        .pwm_chnls[1] = DUTY_60_PERCENT,
        .pwm_chnls[2] = DUTY_60_PERCENT,        
        .pwm_chnls[3] = DUTY_60_PERCENT,
        .pwm_chnls[4] = DUTY_60_PERCENT,
        .pwm_chnls[5] = DUTY_60_PERCENT,
        
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
        .volts_ch[0] = 2,
        .volts_ch[1] = 2,
        .volts_ch[2] = 2,
        .volts_ch[3] = 2,
        .volts_ch[4] = 2,
        .volts_ch[5] = 2,
        
        .pwm_chnls[0] = 2,
        .pwm_chnls[1] = 2,
        .pwm_chnls[2] = 2,
        .pwm_chnls[3] = 2,
        .pwm_chnls[4] = 2,
        .pwm_chnls[5] = 2,
        
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
unsigned short Distance (unsigned short, unsigned short);
unsigned char CheckFiltersAndOffsets2 (void);
void UpdateFiltersTest_Reset (void);

    
// ------- del DMX -------
// extern void EXTI4_15_IRQHandler(void);

// ------- para el DMA -------
#define RCC_DMA_CLK (RCC->AHBENR & RCC_AHBENR_DMAEN)
#define RCC_DMA_CLK_ON 		RCC->AHBENR |= RCC_AHBENR_DMAEN
#define RCC_DMA_CLK_OFF 	RCC->AHBENR &= ~RCC_AHBENR_DMAEN

const char s_blank_line [] = {"                "};
unsigned short ch1_pwm = 0;
unsigned short ch2_pwm = 0;
unsigned short ch3_pwm = 0;
unsigned short ch4_pwm = 0;
unsigned short ch5_pwm = 0;
unsigned short ch6_pwm = 0;

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

    unsigned char ch_values [6] = { 0 };
    
#ifdef USE_PWM_DELTA_FUNCTION
    unsigned short delta_ch3_pwm = 0;
#endif
    
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

#if (defined USE_LED_CTRL_MODE_PWM) || (defined USE_LED_CTRL_MODE_MIXED)
    TIM_1_Init_Irq();
#elif defined USE_LED_CTRL_MODE_CONTINUOS
    TIM_1_Init_Only_PWM();
#else    
#error "set Led control mode on hard.h"
#endif
    TIM_3_Init();

    PWMChannelsReset();


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

    //-- Prueba con DMX + PWM con tope open-loop ----------
    // // inicializo el hard que falta
    // DMX_Disa();
    // AdcConfig();

    // //-- DMA configuration.
    // DMAConfig();
    // DMA1_Channel1->CCR |= DMA_CCR_EN;

    // ADC1->CR |= ADC_CR_ADSTART;

    // //inicializo dmx si todavia no lo hice
    // TIM_14_Init();    //para detectar break en dmx
    // TIM_16_Init();    //para tx dmx OneShoot
    // USART1Config();
    
    // memcpy(&mem_conf, pmem, sizeof(parameters_typedef));
    
    // Packet_Detected_Flag = 0;
    // DMX_channel_selected = mem_conf.dmx_channel;
    // DMX_channel_quantity = mem_conf.dmx_channel_quantity;
                
    // //habilito recepcion
    // SW_RX_TX_RE_NEG;
    // DMX_Ena();    
    
    // while(1)
    // {
    //     if (DMA1->ISR & DMA_ISR_TCIF1)    //esto es sequence ready cada 16KHz
    //         DMA1->IFCR = DMA_ISR_TCIF1;
        
    //     if (!timer_standby)
    //     {
    //         timer_standby = 1000;
    //         //envio datos de los pwm
    //         sprintf(s_to_send, "c1: %d, c2: %d, c3: %d, d1: %d, d2: %d, d3: %d\n",
    //                 data7[1],
    //                 data7[2],
    //                 data7[3],
    //                 ch1_pwm,
    //                 ch2_pwm,
    //                 ch3_pwm);

    //         Usart2Send(s_to_send);

    //         // sprintf(s_to_send, "i1: %d, i2: %d, i3: %d, i4: %d, i5: %d, i6: %d\n",
    //         //         I_Channel_1,
    //         //         I_Channel_2,
    //         //         I_Channel_3,
    //         //         I_Channel_4,
    //         //         I_Channel_5,
    //         //         I_Channel_6);

    //         // Usart2Send(s_to_send);
    //     }

    //     if (UpdateFiltersTest ())
    //     {
    //         //calculo el PWM de cada canal, hardcoded
    //         ch1_pwm = PWMChannelsOffset(sp1_filtered, 598);
    //         Update_PWM1(ch1_pwm);

    //         ch2_pwm = PWMChannelsOffset(sp2_filtered, 895);
    //         Update_PWM2(ch2_pwm);

    //         ch3_pwm = PWMChannelsOffset(sp3_filtered, 839);
    //         Update_PWM3(ch3_pwm);

    //         ch4_pwm = PWMChannelsOffset(sp4_filtered, 598);            
    //         Update_PWM4(ch4_pwm);            

    //         ch5_pwm = PWMChannelsOffset(sp5_filtered, 598);            
    //         Update_PWM5(ch5_pwm);            

    //         ch6_pwm = PWMChannelsOffset(sp6_filtered, 598);            
    //         Update_PWM6(ch6_pwm);

    //     }
    // }
    //-- Fin Prueba con DMX + PWM con tope open-loop ----------

    //-- Prueba con TIM1 Irq PWM on-off + DMX ----------
    //inicializo el hard que falta
    // DMX_Disa();
    // AdcConfig();

    // //-- DMA configuration.
    // DMAConfig();
    // DMA1_Channel1->CCR |= DMA_CCR_EN;

    // ADC1->CR |= ADC_CR_ADSTART;

    // //inicializo dmx si todavia no lo hice
    // TIM_14_Init();    //para detectar break en dmx
    // TIM_16_Init();    //para tx dmx OneShoot
    // USART1Config();
    
    // memcpy(&mem_conf, pmem, sizeof(parameters_typedef));
    
    // Packet_Detected_Flag = 0;
    // DMX_channel_selected = mem_conf.dmx_channel;
    // DMX_channel_quantity = mem_conf.dmx_channel_quantity;
                
    // //habilito recepcion
    // SW_RX_TX_RE_NEG;
    // DMX_Ena();    
    
    // while(1)
    // {
    //     if (DMA1->ISR & DMA_ISR_TCIF1)    //esto es sequence ready cada 16KHz
    //         DMA1->IFCR = DMA_ISR_TCIF1;
        
    //     if (!timer_standby)
    //     {
    //         timer_standby = 500;
    //         //envio corriente y pwm de canal 1
    //         // sprintf(s_to_send, "c1: %d, c2: %d, c3: %d, c4: %d, c5: %d, c6: %d\n",
    //         //         data7[1],
    //         //         data7[2],
    //         //         data7[3],
    //         //         data7[4],
    //         //         data7[5],
    //         //         data7[6]);

    //         // Usart2Send(s_to_send);

    //         // sprintf(s_to_send, "i1: %d, i2: %d, i3: %d, i4: %d, i5: %d, i6: %d\n",
    //         //         I_Channel_1,
    //         //         I_Channel_2,
    //         //         I_Channel_3,
    //         //         I_Channel_4,
    //         //         I_Channel_5,
    //         //         I_Channel_6);

    //         // Usart2Send(s_to_send);
    //     }

    //     if (UpdateFiltersTest ())
    //     {
    //         Change_PWM1(sp1_filtered);
    //         Change_PWM2(sp2_filtered);
    //         Change_PWM3(sp3_filtered);
    //         Change_PWM4(sp4_filtered);
    //         Change_PWM5(sp5_filtered);
    //         Change_PWM6(sp6_filtered);
    //         // if (CTRL_FAN)
    //         //     CTRL_FAN_OFF;
    //         // else
    //         //     CTRL_FAN_ON;
    //     }
    // }
    //-- Fin Prueba con TIM1 Irq PWM on-off + DMX ----------

    //-- Prueba con TIM1 Irq PWM on-off ADC & DMA mido I ----------
    // //inicializo el hard que falta
    // DMX_Disa();
    // AdcConfig();

    // //-- DMA configuration.
    // DMAConfig();
    // DMA1_Channel1->CCR |= DMA_CCR_EN;

    // ADC1->CR |= ADC_CR_ADSTART;

    // unsigned char loop_count = 0;
    // i = 0;
    // while(1)
    // {
    //     if (DMA1->ISR & DMA_ISR_TCIF1)    //esto es sequence ready cada 16KHz
    //         DMA1->IFCR = DMA_ISR_TCIF1;
        
    //     if (!timer_standby)
    //     {
    //         timer_standby = 500;
    //         //envio corriente y pwm de canal 1
    //         sprintf(s_to_send, "d1: %d, i1: %d\n",
    //                 i,
    //                 I_Channel_1);

    //         Usart2Send(s_to_send);
    //         if (loop_count >= 9)
    //         {
    //             loop_count = 0;
    //             if (i > 4)
    //                 i = 0;
    //             else
    //                 i++;
    //             Change_PWM1(i);
    //         }
    //         else
    //             loop_count++;
    //     }
    // }
    //-- Fin Prueba con TIM1 Irq PWM on-off ADC & DMA mido I ----------
                
    //-- Prueba con ADC & DMA & PWM Fijo y mido I ----------
    //inicializo el hard que falta
    // DMX_Disa();
    // AdcConfig();

    //-- DMA configuration.
    // DMAConfig();
    // DMA1_Channel1->CCR |= DMA_CCR_EN;

    // ADC1->CR |= ADC_CR_ADSTART;

    // loop_count = 0;
    // i = 0;
    // while(1)
    // {
    //     if (DMA1->ISR & DMA_ISR_TCIF1)    //esto es sequence ready cada 16KHz
    //         DMA1->IFCR = DMA_ISR_TCIF1;
        
    //     if (!timer_standby)
    //     {
    //         timer_standby = 500;
    //         //envio corriente y pwm de canal 1
    //         sprintf(s_to_send, "d1: %d, i1: %d\n",
    //                 i,
    //                 I_Channel_1);

    //         Usart2Send(s_to_send);
    //         if (loop_count >= 9)
    //         {
    //             loop_count = 0;
    //             if (i > 100)
    //                 i = 10;
    //             // if (i > 4)
    //             //     i = 0;
    //             else
    //                 i++;
    //             Update_PWM1(i);
    //         }
    //         else
    //             loop_count++;
    //     }
    // }
    //-- Fin Prueba con ADC & DMA & PWM Fijo y mido I ----------

    //-- Prueba con ADC & PWM for channels settings ----------
    // AdcConfig();

    // //-- DMA configuration.
    // DMAConfig();
    // DMA1_Channel1->CCR |= DMA_CCR_EN;

    // ADC1->CR |= ADC_CR_ADSTART;
    // led_current_mode = PID_MODE;
    // PWMChannelsReset();
    
    // // Prueba ADC & DMA
    // need_to_save_timer = 10000;

    // led_current_settings_t led_curr;
    // led_curr.sp_current = 1990;
    // led_curr.channel = 1;
    
    // while(1)
    // {
    //     if (!need_to_save_timer)
    //     {
    //         resp = UpdateDutyCycle(&led_curr);

    //         if (resp == resp_error)
    //         {
    //             UpdateDutyCycleReset();
    //             need_to_save_timer = 10000;
    //             sprintf(s_to_send, "No current on CH%d\n", led_curr.channel);                
    //             Usart2Send(s_to_send);
    //         }

    //         if (resp == resp_finish)
    //         {
    //             UpdateDutyCycleReset();
    //             need_to_save_timer = 10000;
    //             sprintf(s_to_send, "More voltage needed for CH%d\n", led_curr.channel);                
    //             Usart2Send(s_to_send);
    //         }

    //         if (resp == resp_ok)
    //         {
    //             UpdateDutyCycleReset();
    //             need_to_save_timer = 10000;
    //             sprintf(s_to_send, "i: %d, d: %d, ireal: %d CH%d\n",
    //                     led_curr.filtered_current_getted,
    //                     led_curr.duty_getted,
    //                     led_curr.real_current_getted,
    //                     led_curr.channel);                
    //             Usart2Send(s_to_send);
    //         }
    //     }
    // }
    //-- Fin Prueba con ADC & PWM for channels settings ----------
    
    //-- Programa de Produccion del DMX
    //inicializo dmx si todavia no lo hice
    TIM_14_Init();    //para detectar break en dmx
    TIM_16_Init();    //para tx dmx OneShoot
    USART1Config();

    // Packet_Detected_Flag = 0;
    // DMX_channel_selected = 1;
    // DMX_channel_quantity = 6;
    
    //inicializo el hard que falta
    AdcConfig();

    //-- DMA configuration.
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    ADC1->CR |= ADC_CR_ADSTART;

    memcpy(&mem_conf, pmem, sizeof(parameters_typedef));

    unsigned short segments[SEGMENTS_QTTY];
    led_current_settings_t led_curr;


        Usart2Send(s_to_send);

        UpdateDutyCycleReset();
        while (UpdateDutyCycle(&led_curr) == resp_continue);
        if (i)
        {
            if (led_curr.duty_getted <= segments[i - 1])
                segments[i] = segments[i - 1] + 1;
            else
                segments[i] = led_curr.duty_getted;
        }
        else
            segments[i] = led_curr.duty_getted;
    }
    Usart2Send("\n");
#endif
    
    
    
        
        
    while (1)
    {
        switch (main_state)
        {
        case MAIN_INIT:
            // memcpy(&mem_conf, pmem, sizeof(parameters_typedef));
            main_state++;
            break;

        case MAIN_HARDWARE_INIT:

            //reseteo hardware
            //DMX en RX
            SW_RX_TX_RE_NEG;
            DMX_Disa();

            //reseteo canales
            PWMChannelsReset();
                        
            //reseteo menues
            MasterModeMenuReset();
            FuncSlaveModeReset();

            sprintf(s_to_send, "prog type: %d\n", mem_conf.program_type);
            Usart2Send(s_to_send);
            Wait_ms(100);
            sprintf(s_to_send, "Max pwm channels: %d %d %d %d %d %d\n",
                    mem_conf.pwm_chnls[0],
                    mem_conf.pwm_chnls[1],
                    mem_conf.pwm_chnls[2],
                    mem_conf.pwm_chnls[3],
                    mem_conf.pwm_chnls[4],
                    mem_conf.pwm_chnls[5]);
            
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
            if (UpdateFiltersTest ())
            {
#ifdef USE_LED_CTRL_MODE_MIXED
                //calculo el PWM de cada canal, solo si tengo leds en los canales
                if (mem_conf.pwm_chnls[0])
                {
                    if (sp1_filtered <= TIM_CNTR_FOR_DMX_MODE_CHANGE)
                    {
                        ch1_pwm = PWMChannelsOffset(sp1_filtered, mem_conf.pwm_chnls[0]);
                        mem_conf.pwm_base_chnls[0] = ch1_pwm;
                        Update_PWM1(ch1_pwm);                        
                    }
                }
                
                if (mem_conf.pwm_chnls[1])
                {
                    if (sp2_filtered <= TIM_CNTR_FOR_DMX_MODE_CHANGE)
                    {
                        ch2_pwm = PWMChannelsOffset(sp2_filtered, mem_conf.pwm_chnls[1]);
                        mem_conf.pwm_base_chnls[1] = ch2_pwm;
                        Update_PWM2(ch2_pwm);
                    }
                }

                if (mem_conf.pwm_chnls[2])
                {
                    if (sp3_filtered <= TIM_CNTR_FOR_DMX_MODE_CHANGE)
                    {
                        ch3_pwm = PWMChannelsOffset(sp3_filtered, mem_conf.pwm_chnls[2]);
                        mem_conf.pwm_base_chnls[2] = ch3_pwm;
                        // Update_PWM3(ch3_pwm);
#ifdef USE_PWM_DELTA_FUNCTION
                        if (!delta_index)
                        {
                            delta_index = 5;
                            if (delta_ch3_pwm < ch3_pwm)
                                delta_ch3_pwm++;
                            else if (delta_ch3_pwm > ch3_pwm)
                                delta_ch3_pwm--;
                    
                            Update_PWM3(delta_ch3_pwm);
                        }
                        else
                            delta_index--;
#else
                        Update_PWM3(ch3_pwm);
#endif                        
                    }
                }

                if (mem_conf.pwm_chnls[3])
                {
                    if (sp4_filtered <= TIM_CNTR_FOR_DMX_MODE_CHANGE)
                    {
                        ch4_pwm = PWMChannelsOffset(sp4_filtered, mem_conf.pwm_chnls[3]);
                        mem_conf.pwm_base_chnls[3] = ch4_pwm;
                        Update_PWM4(ch4_pwm);
                    }
                }

                if (mem_conf.pwm_chnls[4])
                {
                    if (sp5_filtered <= TIM_CNTR_FOR_DMX_MODE_CHANGE)
                    {
                        ch5_pwm = PWMChannelsOffset(sp5_filtered, mem_conf.pwm_chnls[4]);
                        mem_conf.pwm_base_chnls[4] = ch5_pwm;
                        Update_PWM5(ch5_pwm);
                    }
                }

                if (mem_conf.pwm_chnls[5])
                {
                    if (sp6_filtered <= TIM_CNTR_FOR_DMX_MODE_CHANGE)
                    {
                        ch6_pwm = PWMChannelsOffset(sp6_filtered, mem_conf.pwm_chnls[5]);
                        mem_conf.pwm_base_chnls[5] = ch6_pwm;
                        Update_PWM6(ch6_pwm);
                    }
                }
#endif
#ifdef USE_LED_CTRL_MODE_CONTINUOS
                //calculo el PWM de cada canal, solo si tengo leds en los canales
                if (mem_conf.pwm_chnls[0])
                {
                    ch1_pwm = PWMChannelsOffset(sp1_filtered, mem_conf.pwm_chnls[0]);
                    Update_PWM1(ch1_pwm);                        
                }
                
                if (mem_conf.pwm_chnls[1])
                {
                    ch2_pwm = PWMChannelsOffset(sp2_filtered, mem_conf.pwm_chnls[1]);
                    Update_PWM2(ch2_pwm);
                }

//                 if (mem_conf.pwm_chnls[2])
//                 {
//                     ch3_pwm = PWMChannelsOffset(sp3_filtered, mem_conf.pwm_chnls[2]);
//                     // ch3_pwm = PWMChannelsOffset(DMXMapping(sp3_filtered), mem_conf.pwm_chnls[2]);
// #ifdef USE_PWM_DELTA_FUNCTION
//                     // if (!delta_index)
//                     // {
//                     //     delta_index = 5;
//                         if (delta_ch3_pwm < ch3_pwm)
//                             delta_ch3_pwm++;
//                         else if (delta_ch3_pwm > ch3_pwm)
//                             delta_ch3_pwm--;
                    
//                         Update_PWM3(delta_ch3_pwm);
//                     // }
//                     // else
//                     //     delta_index--;
// #else
//                     Update_PWM3(ch3_pwm);
// #endif
//                 }

                if (mem_conf.pwm_chnls[3])
                {
                    ch4_pwm = PWMChannelsOffset(sp4_filtered, mem_conf.pwm_chnls[3]);
                    Update_PWM4(ch4_pwm);
                }

                if (mem_conf.pwm_chnls[4])
                {
                    ch5_pwm = PWMChannelsOffset(sp5_filtered, mem_conf.pwm_chnls[4]);
                    Update_PWM5(ch5_pwm);
                }

                if (mem_conf.pwm_chnls[5])
                {
                    ch6_pwm = PWMChannelsOffset(sp6_filtered, mem_conf.pwm_chnls[5]);
                    Update_PWM6(ch6_pwm);
                }
#endif
            }    //end of filters

            if (mem_conf.pwm_chnls[2])
            {
                if (!delta_timer)
                {
                    unsigned char new_segment = 0;
                    unsigned short dummy = 0;
                    // delta_timer = 5;

                    //mapeo los segmentos
                    new_segment = GetProcessedSegment(sp3_filtered, const_segments, SEGMENTS_QTTY);

#if (defined LINEAR_SEGMENT_8) || (defined LINEAR_SEGMENT_16) || (defined LINEAR_SEGMENT_32)                    
                    if (new_segment)
                    {
                        dummy = sp3_filtered - new_segment * SEGMENTS_VALUE;
                        dummy = dummy * (segments[new_segment] - segments[new_segment - 1]);
                        dummy /= SEGMENTS_VALUE;
                        ch3_pwm = dummy + segments[new_segment - 1];
                    }
                    else
                    {
                        dummy = sp3_filtered * segments[0];
                        dummy /= SEGMENTS_VALUE;
                        ch3_pwm = dummy;
                    }
#endif

#if (defined FIBONACCI_12) || (defined FIBONACCI_8)
                    //tengo que mapear el dmx a la corriente
                    //ch3_pwm es el valor de pwm que necesito para la corriente mapeada
                    delta_timer = 5;

                    if (new_segment)
                    {
                        //traslado a 0 el segmento
                        dummy = sp3_filtered - const_segments[new_segment - 1];
                        dummy = dummy * (segments[new_segment] - segments[new_segment - 1]);
                        dummy /= const_segments[new_segment] - const_segments[new_segment - 1];
                        //traslado el delta al segmento elegido
                        ch3_pwm = dummy + segments[new_segment - 1];
                    }
                    else
                    {
                        dummy = sp3_filtered * segments[0];
                        dummy /= const_segments[0];
                        ch3_pwm = dummy;
                    }
#endif
                    
#ifdef USE_PWM_DELTA_FUNCTION

#if (SEGMENTS_QTTY == 8)
                    // siempre hago funcion delta, pero segun el segmento donde estoy le muevo la
                    // velocidad                    
                    new_segment = GetProcessedSegment(delta_ch3_pwm, segments, SEGMENTS_QTTY);
                    if (new_segment < slow_segment)    //segmento bajo, tengo muchos puntos, voy mas rapido
                        delta_timer = 1;
                    else if (new_segment == slow_segment)    //segmento de cambio de modo voy bien lento
                        delta_timer = 2;
                    else
                        delta_timer = 5;
#endif

#if (SEGMENTS_QTTY == 16)
                    // siempre hago funcion delta, pero segun el segmento donde estoy le muevo la
                    // velocidad                    
                    new_segment = GetProcessedSegment(delta_ch3_pwm, segments, SEGMENTS_QTTY);
                    if (new_segment < slow_segment)    //segmento bajo, tengo muchos puntos, voy mas rapido
                        delta_timer = 1;
                    else if (new_segment == slow_segment)    //segmento de cambio de modo voy bien lento
                        delta_timer = 2;
                    else
                        delta_timer = 5;
#endif

#if (SEGMENTS_QTTY == 32)
                    // siempre hago funcion delta, pero segun el segmento donde estoy le muevo la
                    // velocidad                    
                    new_segment = GetProcessedSegment(delta_ch3_pwm, segments, SEGMENTS_QTTY);
                    if (new_segment < slow_segment)    //segmento bajo, tengo muchos puntos, voy mas rapido
                        delta_timer = 1;
                    else if (new_segment == slow_segment)    //segmento de cambio de modo voy bien lento
                        delta_timer = 2;
                    else if (new_segment == (slow_segment + 1))    //empiezo a acelerar
                        delta_timer = 12;
                    else
                        delta_timer = 5;
#endif
                    
                    if (ch3_pwm > delta_ch3_pwm)
                        delta_ch3_pwm++;
                    else if (ch3_pwm < delta_ch3_pwm)
                        delta_ch3_pwm--;

                    Update_PWM3(delta_ch3_pwm);
#else
                    Update_PWM3(ch3_pwm);
#endif
                }
            }


            if (!timer_standby)
            {
                timer_standby = 1000;

                sprintf(s_to_send, "c1: %d, c2: %d, c3: %d, c4: %d, c5: %d, c6: %d\n",
                    sp1_filtered,
                    sp2_filtered,
                    sp3_filtered,
                    sp4_filtered,
                    sp5_filtered,
                    sp6_filtered);
                Usart2Send(s_to_send);
                
                sprintf(s_to_send, "d1: %d, d2: %d, d3: %d, d4: %d, d5: %d, d6: %d\n",
                    ch1_pwm,
                    ch2_pwm,
                    ch3_pwm,
                    ch4_pwm,
                    ch5_pwm,
                    ch6_pwm);
                Usart2Send(s_to_send);                
            }

            if (CheckS2() > S_HALF)
                main_state = MAIN_ENTERING_MAIN_MENU;

            break;

        case MAIN_IN_PROGRAMS_MODE:
            Func_PX(mem_conf.last_program_in_flash, mem_conf.last_program_deep_in_flash);
            // UpdateSamplesAndPID();

            if (CheckS2() > S_HALF)
                main_state = MAIN_ENTERING_MAIN_MENU;

            ProgramsModeMenu();
            
            break;

        case MAIN_IN_OVERTEMP:
            CTRL_FAN_ON;
            PWMChannelsReset();
            
            Change_PWM1(0);
            Change_PWM2(0);
            Change_PWM3(0);
            Change_PWM4(0);
            Change_PWM5(0);
            Change_PWM6(0);

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
            PWMChannelsReset();

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
#ifdef USE_OVERTEMP_PROT
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
#endif

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

    if (delta_timer)
        delta_timer--;
    
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

unsigned short Distance (unsigned short a, unsigned short b)
{
    if (a < b)
        return 0;

    return (a - b);
}

unsigned char GetProcessedSegment (unsigned short check_segment_by_value,
                                   unsigned short * s,
                                   unsigned char seg_qtty)
{
    // char * s_to_send[100];
    unsigned char i;
    
    for (i = seg_qtty; i > 0; i--)
    {
        if (check_segment_by_value > *(s + i - 1))
            return i;
    }

    return 0;
}


//--- end of file ---//

