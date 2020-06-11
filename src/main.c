//----------------------------------------------------------
// #### PROYECTO DEXEL 6CH BIDIRECCIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MAIN.C #############################################
//----------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "hard.h"
#include "stm32f0xx.h"
#include "gpio.h"
#include "uart.h"

#include "core_cm0.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"

#include "comm.h"
#include "dsp.h"

#include "dmx_transceiver.h"
#include "slave_mode.h"
#include "master_mode.h"
#include "programs_mode.h"
#include "programs_functions.h"
#include "test_functions.h"

#include "flash_program.h"
#include "i2c.h"
#include "mainmenu.h"
#include "screen.h"
#include "ssd1306.h"

#include <stdio.h>
#include <string.h>




// Externals -------------------------------------------------------------------
// --------- Externals de la Memoria y los modos -------
parameters_typedef * pmem = (parameters_typedef *) (unsigned int *) FLASH_PAGE_FOR_BKP;	//en flash
parameters_typedef mem_conf;

// --------- Externals del ADC ---------
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];
volatile unsigned char seq_ready;

// --------- Externals de los timers ---------
volatile unsigned char timer_1seg = 0;
volatile unsigned char switches_timer = 0;

#ifdef USE_PWM_DELTA_INT_TIMER_FAST
volatile unsigned short dmx_timer_hundreds_us_ch1 = 0;
volatile unsigned short dmx_timer_hundreds_us_ch2 = 0;
volatile unsigned short dmx_timer_hundreds_us_ch3 = 0;
volatile unsigned short dmx_timer_hundreds_us_ch4 = 0;
volatile unsigned short dmx_timer_hundreds_us_ch5 = 0;
volatile unsigned short dmx_timer_hundreds_us_ch6 = 0;
#endif

unsigned short ch1_pwm = 0;
unsigned short ch2_pwm = 0;
unsigned short ch3_pwm = 0;
unsigned short ch4_pwm = 0;
unsigned short ch5_pwm = 0;
unsigned short ch6_pwm = 0;

// --------- Externals del USART -------
volatile unsigned char usart1_have_data;
volatile unsigned char usart2_have_data;

// --------- Externals del DMX -------
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

// resultados de los filtros aplicados
unsigned short sp1_filtered = 0;
unsigned short sp2_filtered = 0;
unsigned short sp3_filtered = 0;
unsigned short sp4_filtered = 0;
unsigned short sp5_filtered = 0;
unsigned short sp6_filtered = 0;

unsigned char ch_mode_change_segment [6] = { 0 };    


#ifdef USE_FILTER_LENGHT_16
ma16_u16_data_obj_t st_sp1;
ma16_u16_data_obj_t st_sp2;
ma16_u16_data_obj_t st_sp3;
ma16_u16_data_obj_t st_sp4;
ma16_u16_data_obj_t st_sp5;
ma16_u16_data_obj_t st_sp6;
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
        
        // .max_current_int = 2,
        // .max_current_dec = 0,
        .max_current_ma = 2000,

        .volts_in_mains = 35,
        .max_power = 40,
        
        .volts_ch[0] = 35,
        .volts_ch[1] = 35,
        .volts_ch[2] = 35,
        .volts_ch[3] = 35,
        .volts_ch[4] = 35,
        .volts_ch[5] = 35,

        .pwm_chnls[0] = DUTY_70_PERCENT,
        .pwm_chnls[1] = DUTY_60_PERCENT,
        .pwm_chnls[2] = DUTY_60_PERCENT,        
        .pwm_chnls[3] = DUTY_87_PERCENT,    //Green
        // .pwm_chnls[3] = DUTY_75_PERCENT,    //Red        
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
        
        // .max_current_int = 2,
        // .max_current_dec = 0,
        .max_current_ma = 2000,

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
unsigned short Distance (unsigned short, unsigned short);
void CheckFiltersAndOffsets2 (unsigned char *, unsigned char *);
void UpdateFiltersTest_Reset (void);


//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    char s_to_send [100];
    main_state_t main_state = MAIN_INIT;
    resp_t resp = resp_continue;
    sw_actions_t action = do_nothing;

    unsigned char ch_values [6] = { 0 };

#ifdef HARD_TEST_MODE_DO_NOTHING
    while (1);
#endif

    //GPIO Configuration.
    GPIO_Config();

#ifdef HARD_TEST_MODE_DO_NOTHING_AFTER_GPIOS
    while (1);
#endif
    
    //ACTIVAR SYSTICK TIMER
    if (SysTick_Config(48000))
    {
        while (1)	/* Capture error */
        {
            // if (LED)
            //     LED_OFF;
            // else
            //     LED_ON;

            for (unsigned char i = 0; i < 255; i++)
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
#ifdef USE_PWM_DELTA_INT_TIMER_FAST
    TIM_17_Init();
#endif

    // EnablePreload_CH1;
    // EnablePreload_CH2;
    // EnablePreload_CH3;
    // EnablePreload_CH4;
    // EnablePreload_CH5;
    // EnablePreload_CH6;

    // DisablePreload_CH1;
    // DisablePreload_CH2;
    // DisablePreload_CH3;
    // DisablePreload_CH4;
    // DisablePreload_CH5;
    // DisablePreload_CH6;

    PWMChannelsReset();

#ifdef HARD_TEST_MODE_FAN
    while (1)
    {
        CTRL_FAN_ON;
        Wait_ms(1000);
        CTRL_FAN_OFF;
        Wait_ms(2000);
    }
#endif

#ifdef HARD_TEST_MODE_ONLY_OLED_SCREENS
    resp = resp_ok;
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
#endif

    
#ifdef HARD_TEST_MODE_ONLY_OLED_MAIN_MENU
    resp = resp_ok;
    I2C2_Init();
    Wait_ms(100);

    MainMenu_Init();

    while (1)
    {
        action = do_nothing;

        // Check switches first
        action = CheckSW();        
        resp = MainMenu_Update(action);

        UpdateSwitches();
    }
#endif

    
#ifdef HARD_TEST_MODE_ONLY_OLED_SLAVE_MODE
    resp = resp_ok;
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

    FuncSlaveModeReset();

    while (1)
    {
        FuncSlaveMode(ch_values);

        //simulate dmx packets arrivals
        if (!timer_standby)
        {
            timer_standby = 100;
            Packet_Detected_Flag = 1;
        }
    }

#endif
    
#ifdef HARD_TEST_MODE_ONLY_OLED_PROGRAMS_MODE
    resp = resp_ok;
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
        FuncsProgramsMode(ch_values);
    }
#endif


#ifdef HARD_TEST_MODE_ONLY_OLED_MASTER_MODE
    resp = resp_ok;
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
        FuncsMasterMode(ch_values);
    }
#endif

    ///////////////////////////////////
    // Inicio del Programa Principal //
    ///////////////////////////////////
    // OLED Init
    I2C2_Init();
    Wait_ms(10);

    //first screen
    SCREEN_Init();

    SCREEN_ShowText2(
        "Kirno    ",
        "   Tech  ",
        "Smart    ",
        "   Driver"
        );
    timer_standby = 1300;
    
    while (timer_standby)
        display_update_int_state_machine();
        

    //second screen
    SCREEN_ShowText2(
        "         ",
        "Dexel    ",
        "Lighting ",
        "         "
        );
    timer_standby = 1300;
    
    while (timer_standby)
        display_update_int_state_machine();


    //--- Mensaje Bienvenida ---//
    //---- Defines from hard.h -----//
#ifdef USART2_DEBUG_MODE
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
#endif    //USART2_DEBUG_MODE

    //---- End of Defines from hard.h -----//
    
    //-- Programa de Produccion del DMX
    //inicializo dmx si todavia no lo hice
    TIM_14_Init();    //para detectar break en dmx
    TIM_16_Init();    //para tx dmx OneShoot
    USART1Config();

    //inicializo el hard que falta
    AdcConfig();

    //-- DMA configuration.
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    ADC1->CR |= ADC_CR_ADSTART;

    memcpy(&mem_conf, pmem, sizeof(parameters_typedef));

    //-- Para Debug Test inicial de corriente
// #ifdef ALWAYS_CHECK_CURRENT_ON_INIT
//     unsigned short * p_seg = &mem_conf.segments[0][0];
//     led_current_settings_t led_curr;

//     HARD_Find_Current_Segments(&led_curr, p_seg);

//     for (unsigned char j = 0; j < 6; j++)
//     {        
//         sprintf(s_to_send, "segments[%d]: ", j);
//         Usart2Send(s_to_send);
//         // for (unsigned char i = 0; i < SEGMENTS_QTTY; i++)
//         for (unsigned char i = 0; i < 16; i++)            
//         {
//             // sprintf(s_to_send, "%d ", segments[j][i]);
//             sprintf(s_to_send, "%d ", mem_conf.segments[j][i]);
//             Usart2Send(s_to_send);
//             Wait_ms(10);
//         }
//         Usart2Send("\n");
//     }
    
// #endif
    
//     HARD_Find_Slow_Segments (ch_mode_change_segment);    //muestra las cuentas del vector de corriente

// #ifdef HARCODED_CURRENT_ON_INIT
// #ifdef DITHER_8
//     unsigned short v_curr_ch0 [16] = {939, 1383, 1735, 2048, 2388, 2676, 2938, 3157,
//                                       3389, 3662, 3746, 3827, 3905, 3983, 4057, 4132};
//     unsigned short v_curr_ch1 [16] = {1461, 2148, 2722, 3241, 3676, 4093, 4503, 4898,
//                                       5327, 5400, 5467, 5532, 5593, 5653, 5710, 5766};
//     unsigned short v_curr_ch2 [16] = {1296, 1859, 2328, 2692, 3042, 3364, 3645, 3918,
//                                       4207, 4503, 4548, 4591, 4632, 4674, 4713, 4753};
//     unsigned short v_curr_ch3 [16] = {1403, 1800, 2269, 2609, 2963, 3300, 3608, 3899,
//                                       4202, 4510, 4555, 4599, 4642, 4685, 4725, 4766};

//     // unsigned short v_curr_ch3 [16] = {1403, 1800, 2269, 2609, 2963, 3300, 3608, 3899,
//     //                                   4202, 4202, 4202, 4202, 4202, 4202, 4202, 4202};
    
//     memcpy(&mem_conf.segments[0][0], v_curr_ch0, sizeof(v_curr_ch0));
//     memcpy(&mem_conf.segments[1][0], v_curr_ch1, sizeof(v_curr_ch1));
//     memcpy(&mem_conf.segments[2][0], v_curr_ch2, sizeof(v_curr_ch2));
//     memcpy(&mem_conf.segments[3][0], v_curr_ch3, sizeof(v_curr_ch3));
//     memcpy(&mem_conf.segments[4][0], v_curr_ch3, sizeof(v_curr_ch3));    
//     // memset(&mem_conf.segments[4][0], '\0', sizeof(v_curr_ch0));
//     memset(&mem_conf.segments[5][0], '\0', sizeof(v_curr_ch0));
// #endif
//     //segmento anterior al primero en modo CCM    
//     ch_mode_change_segment[0] = 10;
//     ch_mode_change_segment[1] = 8;
//     ch_mode_change_segment[2] = 9;
//     ch_mode_change_segment[3] = 9;
//     ch_mode_change_segment[4] = 9;    
//     // ch_mode_change_segment[3] = 15;    //para funcionar junto con el vector de que no cambie el modo
//     // ch_mode_change_segment[4] = 8;
//     // ch_mode_change_segment[5] = 8;

//     mem_conf.pwm_chnls[0] = 4132;
//     mem_conf.pwm_chnls[1] = 5766;
//     mem_conf.pwm_chnls[2] = 4753;
//     mem_conf.pwm_chnls[3] = 4202;
//     mem_conf.pwm_chnls[4] = 4202;
//     mem_conf.pwm_chnls[5] = 0;
// #endif
    //-- FIN Para Debug Test inicial de corriente    

    //mando info al puerto
// #ifdef USART2_DEBUG_MODE

// #ifdef USE_SLOW_SEGMENT_LAST_BUT_ONE
//     Wait_ms(100);
//     sprintf(s_to_send, "slow_segment_last_but_one: ");
//     Usart2Send(s_to_send);

//     for (unsigned char j = 0; j < 6; j++)
//     {
//         sprintf(s_to_send, "%d ",
//                 mem_conf.segments[j][(ch_mode_change_segment[j] - 1)]);
//         Usart2Send(s_to_send);
//         Wait_ms(10);
//     }
//     Usart2Send("\n");   
// #else
//     Wait_ms(100);
//     sprintf(s_to_send, "slow_segment: ");
//     Usart2Send(s_to_send);

//     for (unsigned char j = 0; j < 6; j++)
//     {
//         sprintf(s_to_send, "%d ",
//                 mem_conf.segments[j][ch_mode_change_segment[j]]);
//         Usart2Send(s_to_send);
//         Wait_ms(10);
//     }
//     Usart2Send("\n");    
// #endif

// #endif    //endif USART_DEBUG

    
    ///////////////////////////////////
    // Pruebo el Blanco desde el DMX //
    ///////////////////////////////////
    // TEST_White_Dmx_Dimming_By_Segments(25, 1, 255);
    // TEST_White_Dmx_Dimming_By_Change_Mode(25, 5, 255);
    
    
    ////////////////////////////////////////
    // Pruebo el Blanco directo en el PWM //
    ////////////////////////////////////////
    // 3600 puede ser un minimo con el led apenas prendido
    // 4800 serian 27.64V
    // 3400 serian 20.64V
    // pwm_dimming_t dimmer = {
    //     .time_step_ms = 25,
    //     .pwm_min = 0,
    //     .pwm_max = 4800,
    //     .pwm_increment = 12,
    //     .channel = CH4_VAL_OFFSET
    // };
        
    // TEST_Pwm_Dimming(&dimmer);    //DCM -> CCM

    // pwm_dimming_t dimmer = {
    //     .time_step_ms = 5,
    //     .pwm_min = 0,
    //     .pwm_max = 4766,
    //     .pwm_increment = 12,
    //     .channel = CH3_VAL_OFFSET
    // };
        
    // TEST_Pwm_Dimming(&dimmer);    //DCM -> CCM
    

    

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
                        
#ifdef USART2_DEBUG_MODE            
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
#endif

            //limpio los filtros
            UpdateFiltersTest_Reset();

            main_state++;            
            break;

        case MAIN_GET_CONF:
            if (mem_conf.program_type == MASTER_MODE)
            {
                //habilito transmisiones
                SW_RX_TX_DE;
                DMX_Ena();
                
                MasterModeMenuReset();
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
                
                FuncSlaveModeReset();
                main_state = MAIN_IN_SLAVE_MODE;
            }

            if (mem_conf.program_type == PROGRAMS_MODE)
            {
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
            FuncsMasterMode(ch_values);
            CheckFiltersAndOffsets2 (ch_values, ch_mode_change_segment);

            if ((SW_ENTER() > S_HALF) || (SW_BACK() > S_HALF))
                main_state = MAIN_ENTERING_MAIN_MENU;

            if (!timer_standby)
            {
                timer_standby = 40;
                SendDMXPacket (PCKT_INIT);
            }
            break;
            
        case MAIN_IN_SLAVE_MODE:
            FuncSlaveMode (ch_values);
            CheckFiltersAndOffsets2 (ch_values, ch_mode_change_segment);

#ifdef USART2_DEBUG_MODE
            if (!timer_standby)
            {
                timer_standby = 1000;

                sprintf(s_to_send, "c1: %d, c2: %d, c3: %d, c4: %d, c5: %d, c6: %d\n",
                        *(ch_values + 0),
                        *(ch_values + 1),
                        *(ch_values + 2),
                        *(ch_values + 3),
                        *(ch_values + 4),
                        *(ch_values + 5));
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
#endif

            if ((SW_ENTER() > S_HALF) || (SW_BACK() > S_HALF))
                main_state = MAIN_ENTERING_MAIN_MENU;

            break;

        case MAIN_IN_PROGRAMS_MODE:
            FuncsProgramsMode(ch_values);
            CheckFiltersAndOffsets2 (ch_values, ch_mode_change_segment);

            if ((SW_ENTER() > S_HALF) || (SW_BACK() > S_HALF))
                main_state = MAIN_ENTERING_MAIN_MENU;

            break;

        case MAIN_IN_OVERTEMP:
            CTRL_FAN_ON;
            PWMChannelsReset();
            
            SCREEN_ShowText2(
                "LEDs     ",
                "Overtemp ",
                "         ",
                "         "
                );

#ifdef USART2_DEBUG_MODE
            sprintf(s_to_send, "overtemp: %d\n", Temp_Channel);
            Usart2Send(s_to_send);
#endif

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

            MainMenu_Init();
            main_state++;
            break;

        case MAIN_IN_MAIN_MENU:
            action = do_nothing;

            // Check switches first
            action = CheckSW();        
            resp = MainMenu_Update(action);

            if (resp == resp_need_to_save)
            {
#ifdef SAVE_FLASH_IMMEDIATE
                need_to_save_timer = 0;
#endif
#ifdef SAVE_FLASH_WITH_TIMEOUT
                need_to_save_timer = 10000;
#endif
                need_to_save = 1;
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

        // update de LCD
        // display_update_int_state_machine();
        

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
            __disable_irq();
            need_to_save = WriteConfigurations();
            __enable_irq();

#ifdef USART2_DEBUG_MODE
            if (need_to_save)
                Usart2Send((char *) "Memory Saved OK!\n");
            else
                Usart2Send((char *) "Memory problems\n");
#endif

            need_to_save = 0;
        }
        
    }    //end of while 1
    
    return 0;
}
//--- End of Main ---//


void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

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

    //para modo_slave
    UpdateTimerSlaveMode();

    //para programas
    UpdateProgTimers ();

    //para main menu
    // UpdateTimerModeMenu ();
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
        return (b - a);
    else
        return (a - b);
}


#ifdef USE_PWM_WITH_DELTA
unsigned short last_ch1_pwm = 0;
unsigned short last_ch2_pwm = 0;
unsigned short last_ch3_pwm = 0;
unsigned short last_ch4_pwm = 0;
unsigned short last_ch5_pwm = 0;
unsigned short last_ch6_pwm = 0;
#endif

IIR_first_order_data_obj_t ch2_iir;

unsigned short ch_minimun_value[6] = {46, 150, 150, 140, 140, 140};

//aca filtro los offsets del pwm en vez del valor del canal
//cada 5ms
void CheckFiltersAndOffsets2 (unsigned char * ch_val, unsigned char * slow_segment)
{
    //filters para el dmx - generalmente 8 puntos a 200Hz -
    //desde el sp al sp_filter
#ifdef USE_PWM_DELTA_INT_TIMER_FAST
    //cada 5ms mapeo los pwm y los cargo por interrupcion
    if (!dmx_filters_timer)
    {
        dmx_filters_timer = DMX_UPDATE_TIMER;

        if (mem_conf.pwm_chnls[CH1_VAL_OFFSET])
        {
            unsigned char slow_sgm = slow_segment[CH1_VAL_OFFSET];
            
            ch1_pwm = HARD_Map_New_DMX_Data(
                mem_conf.segments[CH1_VAL_OFFSET],
                *(ch_val + CH1_VAL_OFFSET),
                slow_sgm);
            
        }

        if (mem_conf.pwm_chnls[CH2_VAL_OFFSET])
        {
            unsigned char slow_sgm = slow_segment[CH2_VAL_OFFSET];

            ch2_pwm = HARD_Map_New_DMX_Data(
                mem_conf.segments[CH2_VAL_OFFSET],
                *(ch_val + CH2_VAL_OFFSET),
                slow_sgm);
            
        }

        if (mem_conf.pwm_chnls[CH3_VAL_OFFSET])
        {
            unsigned char slow_sgm = slow_segment[CH3_VAL_OFFSET];

            ch3_pwm = HARD_Map_New_DMX_Data(
                mem_conf.segments[CH3_VAL_OFFSET],
                *(ch_val + CH3_VAL_OFFSET),
                slow_sgm);
            
        }
                
        if (mem_conf.pwm_chnls[CH4_VAL_OFFSET])
        {
            unsigned char slow_sgm = slow_segment[CH4_VAL_OFFSET];

            ch4_pwm = HARD_Map_New_DMX_Data(
                mem_conf.segments[CH4_VAL_OFFSET],
                *(ch_val + CH4_VAL_OFFSET),
                slow_sgm);
            
        }

        if (mem_conf.pwm_chnls[CH5_VAL_OFFSET])
        {
            unsigned char slow_sgm = slow_segment[CH5_VAL_OFFSET];

            ch5_pwm = HARD_Map_New_DMX_Data(
                mem_conf.segments[CH5_VAL_OFFSET],
                *(ch_val + CH5_VAL_OFFSET),
                slow_sgm);
            
        }

        if (mem_conf.pwm_chnls[CH6_VAL_OFFSET])
        {
            unsigned char slow_sgm = slow_segment[CH6_VAL_OFFSET];

            ch6_pwm = HARD_Map_New_DMX_Data(
                mem_conf.segments[CH6_VAL_OFFSET],
                *(ch_val + CH6_VAL_OFFSET),
                slow_sgm);
            
        }
    }
    
#endif    //USE_PWM_DELTA_INT_TIMER_FAST

#ifdef USE_PWM_DIRECT_OR_DELTA
    if (!dmx_filters_timer)
    {
        dmx_filters_timer = DMX_UPDATE_TIMER;
        
//         if (mem_conf.pwm_chnls[CH1_VAL_OFFSET])
//         {
//             unsigned char slow_sgm = slow_segment[CH1_VAL_OFFSET];
// #ifdef MAP_CURRENT_WITH_SLOW_SEGMENT      
//             ch1_pwm = HARD_Map_New_DMX_Data(
//                 mem_conf.segments[CH1_VAL_OFFSET],
//                 *(ch_val + CH1_VAL_OFFSET),
//                 slow_sgm,
//                 ch_minimun_value[CH1_VAL_OFFSET]);
// #endif
// #ifdef MAP_CURRENT_DIRECT
//             ch1_pwm = HARD_Process_New_PWM_Data(
//                 mem_conf.segments[CH1_VAL_OFFSET],
//                 *(ch_val + CH1_VAL_OFFSET));
// #endif
            
// #ifdef USE_PWM_WITH_DELTA
//             last_ch1_pwm = CalcNewDelta (last_ch1_pwm, ch1_pwm);
// #ifdef USE_PWM_WITH_DITHER
//             TIM_LoadDitherSequences(0, last_ch1_pwm);
// #else
//             Update_PWM1(last_ch1_pwm);
// #endif
// #endif

// #ifdef USE_PWM_DIRECT
//             ch1_pwm = MA16_U16Circular (&st_sp1, ch1_pwm);
// #ifdef USE_PWM_WITH_DITHER
//             TIM_LoadDitherSequences(0, ch1_pwm);
// #else
//             Update_PWM1(ch1_pwm);
// #endif
// #endif
//         }
        
//         if (mem_conf.pwm_chnls[CH2_VAL_OFFSET])
//         {
//             unsigned char slow_sgm = slow_segment[CH2_VAL_OFFSET];

// #ifdef MAP_CURRENT_WITH_SLOW_SEGMENT                  
//             ch2_pwm = HARD_Map_New_DMX_Data(
//                 mem_conf.segments[CH2_VAL_OFFSET],
//                 *(ch_val + CH2_VAL_OFFSET),
//                 slow_sgm,
//                 ch_minimun_value[CH2_VAL_OFFSET]);                
// #endif
// #ifdef MAP_CURRENT_DIRECT            
//             ch2_pwm = HARD_Process_New_PWM_Data(
//                 mem_conf.segments[CH2_VAL_OFFSET],
//                 *(ch_val + CH2_VAL_OFFSET));
// #endif
            
// #ifdef USE_PWM_WITH_DELTA
//             last_ch2_pwm = CalcNewDelta (last_ch2_pwm, ch2_pwm);
//             Update_PWM2(last_ch2_pwm);
// #else
//             ch2_pwm = IIR_first_order(&ch2_iir, ch2_pwm);
//             // ch2_pwm = MA16_U16Circular (&st_sp2, ch2_pwm);
//             if (ch2_pwm > DUTY_MAX_ALLOWED_WITH_DITHER)
//                 ch2_pwm = DUTY_MAX_ALLOWED_WITH_DITHER;
            
// #ifdef USE_PWM_WITH_DITHER
//             TIM_LoadDitherSequences(1, ch2_pwm);
// #else
//             Update_PWM2(ch2_pwm);
// #endif
// #endif
//         }
        
//         if (mem_conf.pwm_chnls[CH3_VAL_OFFSET])
//         {
//             unsigned char slow_sgm = slow_segment[CH3_VAL_OFFSET];
            
// #ifdef MAP_CURRENT_WITH_SLOW_SEGMENT      
//             ch3_pwm = HARD_Map_New_DMX_Data(
//                 mem_conf.segments[CH3_VAL_OFFSET],
//                 *(ch_val + CH3_VAL_OFFSET),
//                 slow_sgm,
//                 ch_minimun_value[CH3_VAL_OFFSET]);                
// #endif
// #ifdef MAP_CURRENT_DIRECT
//             ch3_pwm = HARD_Process_New_PWM_Data(
//                 mem_conf.segments[CH3_VAL_OFFSET],
//                 *(ch_val + CH3_VAL_OFFSET));
// #endif
            
// #ifdef USE_PWM_WITH_DELTA
//             last_ch3_pwm = CalcNewDelta (last_ch3_pwm, ch3_pwm);
//             Update_PWM3(last_ch3_pwm);
// #else
//             ch3_pwm = MA16_U16Circular (&st_sp3, ch3_pwm);
// #ifdef USE_PWM_WITH_DITHER
//             TIM_LoadDitherSequences(2, ch3_pwm);
// #else
//             Update_PWM3(ch3_pwm);
// #endif
// #endif
//         }
        
//         if (mem_conf.pwm_chnls[CH4_VAL_OFFSET])
//         {
//             unsigned char slow_sgm = slow_segment[CH4_VAL_OFFSET];

// #ifdef MAP_CURRENT_WITH_SLOW_SEGMENT                  
//             ch4_pwm = HARD_Map_New_DMX_Data(
//                 mem_conf.segments[CH4_VAL_OFFSET],
//                 *(ch_val + CH4_VAL_OFFSET),
//                     slow_sgm,
//                     ch_minimun_value[CH4_VAL_OFFSET]);                    
// #endif
// #ifdef MAP_CURRENT_DIRECT            
//             ch4_pwm = HARD_Process_New_PWM_Data(
//                 mem_conf.segments[CH4_VAL_OFFSET],
//                 *(ch_val + CH4_VAL_OFFSET));
// #endif
            
// #ifdef USE_PWM_WITH_DELTA
//             last_ch4_pwm = CalcNewDelta (last_ch4_pwm, ch4_pwm);
// #ifdef USE_PWM_WITH_DITHER
//             TIM_LoadDitherSequences(3, last_ch4_pwm);
// #else
//             Update_PWM4(last_ch4_pwm);
// #endif
// #endif

// #ifdef USE_PWM_DIRECT
//             ch4_pwm = MA16_U16Circular (&st_sp4, ch4_pwm);
//             // ch4_pwm = IIR_first_order(ch4_pwm);
//             if (ch4_pwm > DUTY_MAX_ALLOWED_WITH_DITHER)
//                 ch4_pwm = DUTY_MAX_ALLOWED_WITH_DITHER;
            
// #ifdef USE_PWM_WITH_DITHER
//             // TIM_LoadDitherSequences(3, ch6_pwm);
//             TIM_LoadDitherSequences(3, ch4_pwm);            
// #else
//             Update_PWM4(ch4_pwm);
// #endif
// #endif
//         }
        
//         if (mem_conf.pwm_chnls[CH5_VAL_OFFSET])
//         {
//             unsigned char slow_sgm = slow_segment[CH5_VAL_OFFSET];
            
//             ch5_pwm = HARD_Map_New_DMX_Data(
//                 mem_conf.segments[CH5_VAL_OFFSET],
//                 *(ch_val + CH5_VAL_OFFSET),
//                     slow_sgm,
//                 ch_minimun_value[CH5_VAL_OFFSET]);                    

// #ifdef USE_PWM_WITH_DELTA
//             last_ch5_pwm = CalcNewDelta (last_ch5_pwm, ch5_pwm);
//             Update_PWM5(last_ch5_pwm);
// #else
//             ch5_pwm = MA16_U16Circular (&st_sp5, ch5_pwm);
// #ifdef USE_PWM_WITH_DITHER
//             TIM_LoadDitherSequences(4, ch5_pwm);
// #else
//             Update_PWM5(ch5_pwm);
// #endif
// #endif
//         }
        
//         if (mem_conf.pwm_chnls[CH6_VAL_OFFSET])
//         {
//             unsigned char slow_sgm = slow_segment[CH6_VAL_OFFSET];
            
//             ch6_pwm = HARD_Map_New_DMX_Data(
//                 mem_conf.segments[CH6_VAL_OFFSET],
//                 *(ch_val + CH6_VAL_OFFSET),
//                     slow_sgm,
//                     ch_minimun_value[CH6_VAL_OFFSET]);

// #ifdef USE_PWM_WITH_DELTA
//             last_ch6_pwm = CalcNewDelta (last_ch6_pwm, ch6_pwm);
//             Update_PWM6(last_ch6_pwm);
// #else
//             ch6_pwm = MA16_U16Circular (&st_sp6, ch6_pwm);
// #ifdef USE_PWM_WITH_DITHER
//             TIM_LoadDitherSequences(5, ch6_pwm);
// #else
//             Update_PWM6(ch6_pwm);
// #endif
// #endif
        // }
        
    }
#endif    //USE_PWM_DIRECT_OR_DELTA


}


void UpdateFiltersTest_Reset (void)
{
#ifdef USE_FILTER_LENGHT_16
    MA16_U16Circular_Reset(&st_sp1);
    MA16_U16Circular_Reset(&st_sp2);
    MA16_U16Circular_Reset(&st_sp3);
    MA16_U16Circular_Reset(&st_sp4);
    MA16_U16Circular_Reset(&st_sp5);
    MA16_U16Circular_Reset(&st_sp6);
#endif
    ch2_iir.b_param_to_div_by_1000 = 2;
    ch2_iir.a_param_to_div_by_1000 = 998;
    ch2_iir.output_z1 = 0;
}


//--- end of file ---//

