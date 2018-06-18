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
#define FIRST_POWER_BOARD
// #define SECOND_POWER_BOARD

//------ Configuration for Firmware-Channels -----
// #define LED_AND_LASER_SAME_POWER    //se toma la potencia de laser como master
#define LED_AND_LASER_DIFFERENT_POWER

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


//---- Configuration for Firmware-Programs --------


//-------- Configuration for Outputs-Firmware ------


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
#define S1 ((GPIOA->IDR & 0x0800) == 0)

//GPIOA pin12
#define S2 ((GPIOA->IDR & 0x1000) == 0)

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
#define SW_RX_TX_ON GPIOB->BSRR = 0x00000008
#define SW_RX_TX_OFF GPIOB->BSRR = 0x00080000

//GPIOB pin4     
//GPIOB pin5     TIM3 CH1 - CH2

//GPIOB pin6     
//GPIOB pin7     Usart 1

//GPIOB pin8
#define EXTI_PIN ((GPIOB->IDR & 0x0100) == 0)

//GPIOB pin9     
#define CTRL_FAN ((GPIOB->ODR & 0x0200) != 0)
#define CTRL_FAN_ON GPIOB->BSRR = 0x00000200
#define CTRL_FAN_OFF GPIOB->BSRR = 0x02000000



//ESTADOS DEL LED
typedef enum
{    
    START_BLINKING = 0,
    WAIT_TO_OFF,
    WAIT_TO_ON,
    WAIT_NEW_CYCLE
} led_state_t;

//ESTADOS DEL BUZZER
typedef enum
{    
    BUZZER_INIT = 0,
    BUZZER_TO_STOP,

    BUZZER_MULTIPLE_LONG,
    BUZZER_MULTIPLE_LONGA,
    BUZZER_MULTIPLE_LONGB,

    BUZZER_MULTIPLE_HALF,
    BUZZER_MULTIPLE_HALFA,
    BUZZER_MULTIPLE_HALFB,

    BUZZER_MULTIPLE_SHORT,
    BUZZER_MULTIPLE_SHORTA,
    BUZZER_MULTIPLE_SHORTB
    
} buzzer_state_t;

//COMANDOS DEL BUZZER	(tienen que ser los del estado de arriba)
#define BUZZER_STOP_CMD		BUZZER_TO_STOP
#define BUZZER_LONG_CMD		BUZZER_MULTIPLE_LONG
#define BUZZER_HALF_CMD		BUZZER_MULTIPLE_HALF
#define BUZZER_SHORT_CMD	BUZZER_MULTIPLE_SHORT

#define TIM_BIP_SHORT		50
#define TIM_BIP_SHORT_WAIT	100
#define TIM_BIP_HALF		200
#define TIM_BIP_HALF_WAIT	500
#define TIM_BIP_LONG		1200
#define TIM_BIP_LONG_WAIT	1500


//Estados Externos de LED BLINKING
#define LED_NO_BLINKING    0
#define LED_TREATMENT_STANDBY    1
#define LED_TREATMENT_GENERATING    2

#define LED_TREATMENT_ERROR    6

#define I_Sense_Ch1 adc_ch[0]
#define I_Sense_Ch2 adc_ch[1]
#define I_Sense_Ch3 adc_ch[2]
#define I_Sense_Ch4 adc_ch[3]


/* Module Functions ------------------------------------------------------------*/
void ChangeLed (unsigned char);
void UpdateLed (void);
void UpdateBuzzer (void);
void BuzzerCommands(unsigned char, unsigned char);

#endif /* HARD_H_ */
