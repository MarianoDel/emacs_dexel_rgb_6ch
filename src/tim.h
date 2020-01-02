//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TIM.H ################################
//---------------------------------------------

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _TIM_H_
#define _TIM_H_

#include "hard.h"

//--- Exported types ---//
//--- Exported constants ---//
#ifdef USE_FREQ_48KHZ
#define DUTY_TRANSISTORS_ON	10
#define DUTY_50_PERCENT		500
#define DUTY_60_PERCENT		600
#define DUTY_65_PERCENT		650
#define DUTY_70_PERCENT		700
#define DUTY_80_PERCENT		800
#define DUTY_85_PERCENT		850
#define DUTY_90_PERCENT		900
#define DUTY_95_PERCENT		950
#define DUTY_100_PERCENT	999
#endif
#ifdef USE_FREQ_72KHZ
#define DUTY_TRANSISTORS_ON	10
#define DUTY_50_PERCENT		333
#define DUTY_60_PERCENT		400
#define DUTY_90_PERCENT		600
#define DUTY_95_PERCENT		633
#define DUTY_100_PERCENT	666
#endif
#define DUTY_ALWAYS			(DUTY_100_PERCENT + 1)

#define DUTY_LOW_TO_DUTY_FAST    10
#define DUTY_LOW_100_PERCENT    (DUTY_100_PERCENT * DUTY_LOW_TO_DUTY_FAST + DUTY_LOW_TO_DUTY_FAST - 1)

#define TIM_CNTR_FOR_DMX_MAX    255
#define TIM_CNTR_FOR_DMX_DELTA    (TIM_CNTR_FOR_DMX_MAX - TIM_CNTR_FOR_DMX_MODE_CHANGE)
#define TIM_CNTR_FOR_DMX_MODE_CHANGE    155


//--- Exported macro ---//
#define RCC_TIM1_CLK 		(RCC->APB2ENR & 0x00000800)
#define RCC_TIM1_CLK_ON 	RCC->APB2ENR |= 0x00000800
#define RCC_TIM1_CLK_OFF 	RCC->APB2ENR &= ~0x00000800

#define RCC_TIM3_CLK 		(RCC->APB1ENR & 0x00000002)
#define RCC_TIM3_CLK_ON 	RCC->APB1ENR |= 0x00000002
#define RCC_TIM3_CLK_OFF 	RCC->APB1ENR &= ~0x00000002

#define RCC_TIM6_CLK 		(RCC->APB1ENR & 0x00000010)
#define RCC_TIM6_CLK_ON 	RCC->APB1ENR |= 0x00000010
#define RCC_TIM6_CLK_OFF 	RCC->APB1ENR &= ~0x00000010

#define RCC_TIM14_CLK 		(RCC->APB1ENR & 0x00000100)
#define RCC_TIM14_CLK_ON 	RCC->APB1ENR |= 0x00000100
#define RCC_TIM14_CLK_OFF 	RCC->APB1ENR &= ~0x00000100

#define RCC_TIM15_CLK 		(RCC->APB2ENR & 0x00010000)
#define RCC_TIM15_CLK_ON 	RCC->APB2ENR |= 0x00010000
#define RCC_TIM15_CLK_OFF 	RCC->APB2ENR &= ~0x00010000

#define RCC_TIM16_CLK 		(RCC->APB2ENR & 0x00020000)
#define RCC_TIM16_CLK_ON 	RCC->APB2ENR |= 0x00020000
#define RCC_TIM16_CLK_OFF 	RCC->APB2ENR &= ~0x00020000

#define RCC_TIM17_CLK 		(RCC->APB2ENR & 0x00040000)
#define RCC_TIM17_CLK_ON 	RCC->APB2ENR |= 0x00040000
#define RCC_TIM17_CLK_OFF 	RCC->APB2ENR &= ~0x00040000

#define TIM1DisableInterrupt     (TIM1->DIER &= ~TIM_DIER_UIE)
#define TIM1EnableInterrupt      (TIM1->DIER |= TIM_DIER_UIE)

#define TIM17DisableInterrupt    (TIM17->DIER &= ~(TIM_DIER_CC1IE | TIM_DIER_UIE))
#define TIM17EnableInterrupt     (TIM17->DIER |= TIM_DIER_CC1IE | TIM_DIER_UIE)

#define TIM17_NEW_CH1    0x01
#define TIM17_NEW_CH2    0x02
#define TIM17_NEW_CH3    0x04
#define TIM17_NEW_CH4    0x08
#define TIM17_NEW_CH5    0x10
#define TIM17_NEW_CH6    0x20

#define Update_PWM1_FAST(X)    Update_TIM1_CH1(X)
#define Update_PWM2_FAST(X)    Update_TIM1_CH2(X)
#define Update_PWM1_LOW(X)    Update_TIM3_CH1(X)
#define Update_PWM2_LOW(X)    Update_TIM3_CH2(X)
#define Update_PWM5(X)    Update_TIM3_CH3(X)
#define Update_PWM6(X)    Update_TIM3_CH4(X)

//--- Exported functions ---//
void TIM1_BRK_UP_TRG_COM_IRQHandler (void);
void TIM3_IRQHandler (void);
void TIM_3_Init(void);
void TIM_3_Init_Edge_Align (void);
void TIM_1_Init_Irq (void);
void TIM_1_Init_Only_PWM (void);
void TIM_1_Init_Edge_Align (void);
void TIM_6_Init (void);
void TIM14_IRQHandler (void);
void TIM_14_Init(void);
void TIM16_IRQHandler (void);
void TIM_16_Init(void);
void OneShootTIM16 (unsigned short);
void TIM17_IRQHandler (void);
void TIM_17_Init(void);
void Update_TIM1_CH1 (unsigned short);
void Update_TIM1_CH2 (unsigned short);
void Update_TIM3_CH1 (unsigned short);
void Update_TIM3_CH2 (unsigned short);
void Update_TIM3_CH3 (unsigned short);
void Update_TIM3_CH4 (unsigned short);

void Wait_ms (unsigned short wait);
#endif
//--- End ---//


//--- END OF FILE ---//
