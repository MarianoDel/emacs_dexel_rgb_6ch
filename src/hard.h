//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARD.H #################################
//---------------------------------------------

#ifndef _HARD_H_
#define _HARD_H_

#include "stm32f0xx.h"
#include "switches_answers.h"


//-- Defines For Configuration -------------------
//---- Configuration for Hardware Versions -------
// #define HARDWARE_VERSION_1_0    //prototipos 2018 4 unidades
// #define HARDWARE_VERSION_2_0    //prototipos 2020 4 unidades
#define HARDWARE_VERSION_2_2    //prototipos 2020 4 unidades


#define SOFTWARE_VERSION_1_0
// #define SOFTWARE_VERSION_1_1

//---- Features Configuration ----------------
// #define WITH_GRANDMASTER
#define WITH_BIDIRECTIONAL
// #define USE_FILTER_LENGHT_8
#define USE_FILTER_LENGHT_16

//--- Check the temp sensor ---//
#define USE_OVERTEMP_PROT

//--- Check the voltage sensor ---//
#define USE_OVERVOLTAGE_PROT
#define USE_UNDERVOLTAGE_PROT

// --- How to control the PWM generation, select only one --- //
#define USE_PWM_DIRECT    //mapea lo que llega en el dmx al PWM directo
// #define USE_PWM_WITH_DELTA    //mapea la corriente pero la busca punto a punto

// --- Apply Power Control on DMX1 Mode ---//
// #define WITH_POWER_CONTROL
// #define WITH_POWER_CONTROL_SHOW_IN_DISPLAY

// --- Filters checked by int on DMX ---//
#define CHECK_FILTERS_BY_INT
// #define USE_CTRL_FAN_FOR_INT_FILTERS_UPDATE

// --- Update the display OLED on DMX? ---//
// #define NO_DISPLAY_UPDATE_ON_DMX

// --- How often we update the PWM outputs --- //
#define DMX_UPDATE_TIMER_WITH_DELTA    3    //tick en 1ms
#define DMX_UPDATE_TIMER    5    //si pongo esto en 2 se ven saltos en el blanco incluso con delta-single-step

// --- After configuration what to do with the flash --- //
#define SAVE_FLASH_IMMEDIATE
// #define SAVE_FLASH_WITH_TIMEOUT

// --- PWM Frequency --- //
// #define USE_FREQ_48KHZ
// #define USE_FREQ_24KHZ
#define USE_FREQ_16KHZ
// #define USE_FREQ_4_8KHZ

// --- Usart2 Modes --- //
#define USART2_DEBUG_MODE
// #define USART2_WIFI_MODE


//------ Configuration for Firmware-Channels -----
#define WHITE_AS_IN_RGB		//el blanco lo forma con los 3 colores
//#define WHITE_AS_WHITE	//el blanco tiene leds blancos individuales


// --- Sanity checks --- //
#if ((!defined USE_PWM_DIRECT) \
     && (!defined USE_PWM_WITH_DELTA))

#error "Must select what type of PWM generation to use"
#endif

//---- End of Features Configuration ----------

//-- Configuration for some Analog things ------
//---- Voltage Sensor
#define MIN_PWR_SUPPLY    VOLTS_20
#define MAX_PWR_SUPPLY    VOLTS_50

#define VOLTS_20    730
#define VOLTS_50    1825
//---- LM335 measurement Temperatures
//37	2,572V
//40	2,600V
//45	2,650V
//50	2,681V
//55	2,725V
//60	2,765V
#define TEMP_IN_30		3226
#define TEMP_IN_35		3279
#define TEMP_IN_50		3434
#define TEMP_IN_65		3591
#define TEMP_DISCONECT		4000


//--- Hardware Welcome Code ------------------//
#ifdef HARDWARE_VERSION_2_2
#define HARD "Hardware Version: 2.2\n"
#endif
#ifdef HARDWARE_VERSION_2_0
#define HARD "Hardware Version: 2.0\n"
#endif
#ifdef HARDWARE_VERSION_1_0
#define HARD "Hardware Version: 1.0\n"
#endif

//--- Software Welcome Code ------------------//
#ifdef SOFTWARE_VERSION_1_2
#define SOFT "Software Version: 1.2\n"
#endif
#ifdef SOFTWARE_VERSION_1_1
#define SOFT "Software Version: 1.1\n"
#endif
#ifdef SOFTWARE_VERSION_1_0
#define SOFT "Software Version: 1.0\n"
#endif


//-- End Of Defines For Configuration ---------------

#ifdef HARDWARE_VERSION_2_2
// GPIOC pin13
// GPIOC pin14
// GPIOC pin15    NC
// GPIOF pin0    
// GPIOF pin1    NC

//GPIOC pin0
//GPIOC pin1
//GPIOC pin2    ADC input V_SENSE_48V

//GPIOC pin3    NC

//GPIOA pin0
//GPIOA pin1    NC

//GPIOA pin2    Usart2tx
//GPIOA pin3    Usart2rx

//GPIOF pin4    
//GPIOF pin5    NC

//GPIOA pin4    

//GPIOA pin5    Encoder SW
#define ENC_SW ((GPIOA->IDR & 0x0020) == 0)

//GPIOA pin6    
//GPIOA pin7    NC

//GPIOC pin4    ADC input LM335_VO

//GPIOC pin5    NC

//GPIOB pin0    TIM3 CH3    (output CH2)
//GPIOB pin1    TIM3 CH4    (output CH1)

//GPIOB pin2    NC
//GPIOB pin10    NC
//GPIOB pin11    NC
//GPIOB pin12    NC
//GPIOB pin13    NC
//GPIOB pin14    Encoder DT
#define ENC_DT ((GPIOB->IDR & 0x4000) == 0)

//GPIOB pin15    Encoder CLK
#define ENC_CLK ((GPIOB->IDR & 0x8000) == 0)

//GPIOC pin6    NC
//GPIOC pin7    NC
//GPIOC pin8    NC
//GPIOC pin9    NC

//GPIOA pin8    TIM1 CH1 or TEST_PIN    (output CH6)
#define TEST_PIN1 ((GPIOA->ODR & 0x0100) != 0)
#define TEST_PIN1_ON (GPIOA->BSRR = 0x00000100)
#define TEST_PIN1_OFF (GPIOA->BSRR = 0x01000000)

//GPIOA pin9    TIM1 CH2    (output CH5)

//GPIOA pin10    NC
//GPIOA pin11    NC
//GPIOA pin12    NC
//GPIOA pin13    NC

//GPIOF pin6    I2C2 SCL
//GPIOF pin7    I2C2 SDA

//GPIOA pin14    NC
//GPIOA pin15    NC

//GPIOC pin10    NC
//GPIOC pin11    NC
//GPIOC pin12    NC

//GPIOD pin2    NC

//GPIOB pin3
#define SW_RX_TX ((GPIOB->ODR & 0x0008) != 0)
#define SW_RX_TX_DE (GPIOB->BSRR = 0x00000008)
#define SW_RX_TX_RE_NEG (GPIOB->BSRR = 0x00080000)

//GPIOB pin4     TIM3 CH1    (output CH3)
//GPIOB pin5     TIM3 CH2    (output CH4)

//GPIOB pin6     USART1 Tx
#define DMX_TX_PIN ((GPIOB->ODR & 0x0040) != 0)
#define DMX_TX_PIN_ON (GPIOB->BSRR = 0x00000040)
#define DMX_TX_PIN_OFF (GPIOB->BSRR = 0x00400000)

//GPIOB pin7     USART1 Rx

//GPIOB pin8
#define EXTI_Input ((GPIOB->IDR & 0x0100) != 0)
#define DMX_INPUT EXTI_Input

//GPIOB pin9     
#define CTRL_FAN ((GPIOB->ODR & 0x0200) != 0)
#define CTRL_FAN_ON (GPIOB->BSRR = 0x00000200)
#define CTRL_FAN_OFF (GPIOB->BSRR = 0x02000000)

#ifdef USE_TESTS_PIN
#define TEST_PIN2    CTRL_FAN
#define TEST_PIN2_ON    CTRL_FAN_ON
#define TEST_PIN2_OFF    CTRL_FAN_OFF
#endif

#endif    //HARDWARE_VERSION_2_2

#ifdef HARDWARE_VERSION_2_0
//GPIOC pin13
//GPIOC pin14
//GPIOC pin15    NC

//GPIOF pin0    
//GPIOF pin1    NC

//GPIOC pin0
//GPIOC pin1
//GPIOC pin2    ADC inputs

//GPIOC pin3    NC

//GPIOA pin0
//GPIOA pin1    2 ADC channels

//GPIOA pin2
//GPIOA pin3    Usart2

//GPIOF pin4    NC
//GPIOF pin5    NC

//GPIOA pin4    ADC

//GPIOA pin5    NC

//GPIOA pin6    ADC

//GPIOA pin7    NC

//GPIOC pin4    ADC

//GPIOC pin5    NC

//GPIOB pin0
//GPIOB pin1    TIM3 CH1 - CH4

//GPIOB pin2    NC
//GPIOB pin10    NC
//GPIOB pin11    NC
//GPIOB pin12    NC
//GPIOB pin13    NC
//GPIOB pin14
#define S4_PIN ((GPIOB->IDR & 0x8000) == 0)

//GPIOB pin15
#define S3_PIN ((GPIOB->IDR & 0x4000) == 0)

//GPIOC pin6    NC
//GPIOC pin7    NC
//GPIOC pin8    NC
//GPIOC pin9    NC

//GPIOA pin8    or TEST_PIN
#define TEST_PIN1 ((GPIOA->ODR & 0x0100) != 0)
#define TEST_PIN1_ON (GPIOA->BSRR = 0x00000100)
#define TEST_PIN1_OFF (GPIOA->BSRR = 0x01000000)
//GPIOA pin9    TIM1 CH1 - CH2

//GPIOA pin10    NC

//GPIOA pin11
#define S1_PIN ((GPIOA->IDR & 0x0800) == 0)

//GPIOA pin12
#define S2_PIN ((GPIOA->IDR & 0x1000) == 0)

//GPIOA pin13    NC

//GPIOF pin6    
//GPIOF pin7    I2C2

//GPIOA pin14    NC
//GPIOA pin15    NC

//GPIOC pin10    NC
//GPIOC pin11    NC
//GPIOC pin12    NC

//GPIOD pin2    NC

//GPIOB pin3    
#define SW_RX_TX ((GPIOB->ODR & 0x0008) != 0)
#define SW_RX_TX_DE (GPIOB->BSRR = 0x00000008)
#define SW_RX_TX_RE_NEG (GPIOB->BSRR = 0x00080000)

//GPIOB pin4     
//GPIOB pin5     TIM3 CH1 - CH2

//GPIOB pin6     Tx
#define DMX_TX_PIN ((GPIOB->ODR & 0x0040) != 0)
#define DMX_TX_PIN_ON (GPIOB->BSRR = 0x00000040)
#define DMX_TX_PIN_OFF (GPIOB->BSRR = 0x00400000)
//GPIOB pin7     Rx Usart 1

//GPIOB pin8
#define EXTI_Input ((GPIOB->IDR & 0x0100) != 0)
#define DMX_INPUT EXTI_Input

//GPIOB pin9     
#define CTRL_FAN ((GPIOB->ODR & 0x0200) != 0)
#define CTRL_FAN_ON (GPIOB->BSRR = 0x00000200)
#define CTRL_FAN_OFF (GPIOB->BSRR = 0x02000000)

#ifdef USE_TESTS_PIN
#define TEST_PIN2    CTRL_FAN
#define TEST_PIN2_ON    CTRL_FAN_ON
#define TEST_PIN2_OFF    CTRL_FAN_OFF
#endif

#endif    //HARDWARE_VERSION_2_0

#ifdef HARDWARE_VERSION_1_0
//GPIOC pin13
#define LCD_E ((GPIOC->ODR & 0x2000) != 0)
#define LCD_E_ON GPIOC->BSRR = 0x00002000
#define LCD_E_OFF GPIOC->BSRR = 0x20000000

//GPIOC pin14
#define LCD_RS ((GPIOC->ODR & 0x4000) != 0)
#define LCD_RS_ON GPIOC->BSRR = 0x00004000
#define LCD_RS_OFF GPIOC->BSRR = 0x40000000

//GPIOC pin15
#define CTRL_BKL ((GPIOC->ODR & 0x8000) != 0)
#define CTRL_BKL_ON GPIOC->BSRR = 0x00008000
#define CTRL_BKL_OFF GPIOC->BSRR = 0x80000000

//GPIOC pin0
//GPIOC pin1
//GPIOC pin2
//GPIOC pin3    LCD D4-D7

//GPIOA pin0
//GPIOA pin1    2 ADC channels

//GPIOA pin2
//GPIOA pin3    Usart2

//GPIOF pin4    NC
//GPIOF pin5    NC

//GPIOA pin4
//GPIOA pin5
//GPIOA pin6
//GPIOA pin7    
//GPIOC pin4    5 ADC channels

//GPIOC pin5

//GPIOB pin0
//GPIOB pin1    TIM3 CH1 - CH4

//GPIOB pin2    NC
//GPIOB pin10    NC
//GPIOB pin11    NC
//GPIOB pin12    NC
//GPIOB pin13    NC
//GPIOB pin14    NC
//GPIOB pin15    NC

//GPIOC pin6    NC
//GPIOC pin7    NC
//GPIOC pin8    NC
//GPIOC pin9    NC

//GPIOA pin8    
//GPIOA pin9    TIM1 CH1 - CH2

//para pruebas con TIM1 que no me anda!!!
#define TIM_CH1 ((GPIOA->ODR & 0x0100) != 0)
#define TIM_CH1_ON GPIOA->BSRR = 0x00000100
#define TIM_CH1_OFF GPIOA->BSRR = 0x01000000

#define TIM_CH2 ((GPIOA->ODR & 0x0200) != 0)
#define TIM_CH2_ON GPIOA->BSRR = 0x00000200
#define TIM_CH2_OFF GPIOA->BSRR = 0x02000000

//GPIOA pin10    NC

//GPIOA pin11
#define S1_PIN ((GPIOA->IDR & 0x0800) == 0)

//GPIOA pin12
#define S2_PIN ((GPIOA->IDR & 0x1000) == 0)

//GPIOA pin13    NC

//GPIOF pin6    NC
//GPIOF pin7    NC

//GPIOA pin14    NC
//GPIOA pin15    NC

//GPIOC pin10    NC
//GPIOC pin11    NC
//GPIOC pin12    NC

//GPIOD pin2    NC

//GPIOB pin3    
#define SW_RX_TX ((GPIOB->ODR & 0x0008) != 0)
#define SW_RX_TX_DE GPIOB->BSRR = 0x00000008
#define SW_RX_TX_RE_NEG GPIOB->BSRR = 0x00080000

//GPIOB pin4     
//GPIOB pin5     TIM3 CH1 - CH2

//GPIOB pin6     Tx
#define DMX_TX_PIN ((GPIOB->ODR & 0x0040) != 0)
#define DMX_TX_PIN_ON GPIOB->BSRR = 0x00000040
#define DMX_TX_PIN_OFF GPIOB->BSRR = 0x00400000
//GPIOB pin7     Rx Usart 1

//GPIOB pin8
#define EXTI_Input ((GPIOB->IDR & 0x0100) != 0)
#define DMX_INPUT EXTI_Input

//GPIOB pin9     
#define CTRL_FAN ((GPIOB->ODR & 0x0200) != 0)
#define CTRL_FAN_ON GPIOB->BSRR = 0x00000200
#define CTRL_FAN_OFF GPIOB->BSRR = 0x02000000

#endif    //HARDWARE_VERSION_1_0


// Exported Types & Macros -----------------------------------------------------
#define CH1_VAL_OFFSET    0
#define CH2_VAL_OFFSET    1
#define CH3_VAL_OFFSET    2
#define CH4_VAL_OFFSET    3
#define CH5_VAL_OFFSET    4
#define CH6_VAL_OFFSET    5

typedef enum {
    SW_NO = 0,
    SW_MIN,
    SW_HALF,
    SW_FULL
    
} resp_sw_t;

// Switches filter states
// typedef enum
// {    
//     S_NO = 0,
//     S_MIN,
//     S_HALF,
//     S_FULL
// } sw_state_t;

// Switches actions
// typedef enum {
//     do_nothing = 0,
//     selection_up,
//     selection_dwn,
//     selection_enter,
//     selection_back

// } sw_actions_t;

//ESTADOS DEL MAIN
typedef enum
{    
    MAIN_INIT = 0,
    MAIN_HARDWARE_INIT,
    MAIN_GET_CONF,
    MAIN_IN_DMX1_MODE,
    MAIN_IN_DMX2_MODE,
    MAIN_IN_MASTER_SLAVE_MODE,
    MAIN_IN_MANUAL_MODE,
    MAIN_IN_RESET_MODE,
    MAIN_IN_WIFI_MODE,    
    MAIN_IN_OVERTEMP,
    MAIN_IN_OVERTEMP_B,
    MAIN_IN_OVERVOLTAGE,
    MAIN_IN_OVERVOLTAGE_B,
    MAIN_IN_UNDERVOLTAGE,
    MAIN_IN_UNDERVOLTAGE_B,
    MAIN_ENTERING_MAIN_MENU,
    MAIN_ENTERING_MAIN_MENU_WAIT_FREE,    
    MAIN_IN_MAIN_MENU
    
} main_state_t;

// typedef enum {
//     resp_ok = 0,
//     resp_continue,
//     resp_selected,
//     resp_change,
//     resp_change_all_up,
//     resp_working,
//     resp_error,
//     resp_need_to_save,
//     resp_finish,
//     //main menu
//     resp_save

// } resp_t;



// Module Exported Functions ---------------------------------------------------
void HARD_Timeouts (void);
resp_sw_t CheckSET (void);
unsigned char CheckCCW (void);
unsigned char CheckCW (void);
void UpdateSwitches (void);
void UpdateEncoder (void);

#ifdef HARDWARE_VERSION_2_0
#define SW_BACK()    CheckS1()
#define SW_ENTER()    CheckS2()
#define SW_UP()    CheckS3()
#define SW_DWN()    CheckS4()

sw_state_t CheckS1 (void);
sw_state_t CheckS2 (void);
sw_state_t CheckS3 (void);
sw_state_t CheckS4 (void);
sw_actions_t CheckSW (void);
#endif

#endif /* HARD_H_ */

//--- end of file ---//
