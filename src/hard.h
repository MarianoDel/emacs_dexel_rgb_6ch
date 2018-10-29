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


//-- Defines For Configuration -------------------
//---- Configuration for Hardware Versions -------
#define HARDWARE_VERSION_1_0


#define SOFTWARE_VERSION_1_0
// #define SOFTWARE_VERSION_1_1

//---- Features Configuration ----------------
// #define WITH_GRANDMASTER
#define WITH_BIDIRECTIONAL
// #define USE_FILTER_LENGHT_8
#define USE_FILTER_LENGHT_16
// #define USE_OVERTEMP_PROT


//------ Configuration for Firmware-Channels -----


//---- End of Features Configuration ----------



//--- Hardware Welcome Code ------------------//
#ifdef HARDWARE_VERSION_1_0
#define HARD "Hardware Version: 1.0\n"
#endif
#ifdef HARDWARE_VERSION_2_0
#define HARD "Hardware Version: 2.0\n"
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

//-------- Configuration for Outputs-Channels -----
#define MAX_CURRENT_IN_ADC    820

#define MIN_CURRENT_INT    0
#define MIN_CURRENT_DEC    7
#define MAX_CURRENT_INT    2
#define MAX_CURRENT_DEC    0

//esto es dmx_data * MAX_CURRENT * MAX_CURRENT_IN_ADC_COMPENSATED / 512
//255 * 2.0 * 823 / 512 = 820
#define MAX_CURRENT_IN_ADC_COMPENSATED    (MAX_CURRENT_IN_ADC + 3)

#define MIN_MAINS_VOLTAGE    20
#define MAX_MAINS_VOLTAGE    48
#define MIN_MAX_POWER    40
#define MAX_MAX_POWER    100

//---- Configuration for Firmware-Programs --------


//-------- Configuration for Outputs-Firmware ------
//---- Temperaturas en el LM335
//37	2,572
//40	2,600
//45	2,650
//50	2,681
//55	2,725
//60	2,765
#define TEMP_IN_30		3226
#define TEMP_IN_35		3279
#define TEMP_IN_50		3434
#define TEMP_IN_65		3591
#define TEMP_DISCONECT		4000


//-- End Of Defines For Configuration ---------------

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





#define SWITCHES_TIMER_RELOAD	10

#define SWITCHES_THRESHOLD_FULL	300		//3 segundos
#define SWITCHES_THRESHOLD_HALF	100		//1 segundo
#define SWITCHES_THRESHOLD_MIN	5		//50 ms

#define TTIMER_FOR_CAT_DISPLAY			2000	//tiempo entre que dice canal y el numero
#define TIMER_STANDBY_TIMEOUT_REDUCED	2000	//reduced 2 segs
#define TIMER_STANDBY_TIMEOUT			6000	//6 segundos
#define DMX_DISPLAY_SHOW_TIMEOUT		30000	//30 segundos


//ESTADOS DE LOS SWITCHES
typedef enum
{    
    S_NO = 0,
    S_MIN,
    S_HALF,
    S_FULL
} sw_state_t;

//ESTADOS DEL MAIN
typedef enum
{    
    MAIN_INIT = 0,
    MAIN_HARDWARE_INIT,
    MAIN_GET_CONF,
    MAIN_IN_MASTER_MODE,
    MAIN_IN_SLAVE_MODE,
    MAIN_IN_PROGRAMS_MODE,
    MAIN_IN_OVERTEMP,
    MAIN_IN_OVERTEMP_B,
    MAIN_ENTERING_MAIN_MENU,
    MAIN_IN_MAIN_MENU
    
} main_state_t;

typedef enum {
    resp_ok = 0,
    resp_continue,
    resp_selected,
    resp_change,
    resp_change_all_up,
    resp_working,
    resp_error,
    resp_need_to_save,
    resp_finish

} resp_t;

typedef struct {
    //asking for
    unsigned short sp_current;
    unsigned char channel;

    //results
    unsigned short duty_getted;
    unsigned short real_current_getted;
    unsigned short filtered_current_getted;
    
} led_current_settings_t;

/* Module Functions ------------------------------------------------------------*/
sw_state_t CheckS1 (void);
sw_state_t CheckS2 (void);
void UpdateSwitches (void);
resp_t UpdateDutyCycle (led_current_settings_t *);
void UpdateDutyCycleReset (void);
void PWMChannelsReset (void);

#endif /* HARD_H_ */
