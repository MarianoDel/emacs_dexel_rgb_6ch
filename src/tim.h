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

#include "hard.h"    //for freq conf

//--- Exported types ---//
//--- Exported constants ---//
#define DUTY_MAX_ALLOWED  (DUTY_95_PERCENT)
#define DUTY_ALWAYS       (DUTY_100_PERCENT + 1)

#ifdef USE_FREQ_16KHZ
#define DUTY_100_PERCENT	3000
#endif

#ifdef USE_FREQ_12KHZ
#define DUTY_100_PERCENT	4000
#endif

#ifdef USE_FREQ_8KHZ
#define DUTY_100_PERCENT	3000
#endif

#ifdef USE_FREQ_6KHZ
#define DUTY_100_PERCENT	4000
#endif

#ifdef USE_FREQ_4KHZ
#define DUTY_100_PERCENT	4000
#endif

#ifdef USE_FREQ_4_8KHZ
#define DUTY_TRANSISTORS_ON	7
#define DUTY_50_PERCENT		500
#define DUTY_60_PERCENT		600
#define DUTY_65_PERCENT		650
#define DUTY_70_PERCENT		700
#define DUTY_75_PERCENT		750
#define DUTY_80_PERCENT		800
#define DUTY_85_PERCENT		850
#define DUTY_87_PERCENT		875
#define DUTY_90_PERCENT		900
#define DUTY_95_PERCENT		950
#define DUTY_100_PERCENT	1000
#endif



//--- Exported macro ---//
#define RCC_TIM1_CLK 		(RCC->APB2ENR & 0x00000800)
#define RCC_TIM1_CLK_ON 	(RCC->APB2ENR |= 0x00000800)
#define RCC_TIM1_CLK_OFF 	(RCC->APB2ENR &= ~0x00000800)

#define RCC_TIM3_CLK 		(RCC->APB1ENR & 0x00000002)
#define RCC_TIM3_CLK_ON 	(RCC->APB1ENR |= 0x00000002)
#define RCC_TIM3_CLK_OFF 	(RCC->APB1ENR &= ~0x00000002)

#define RCC_TIM6_CLK 		(RCC->APB1ENR & 0x00000010)
#define RCC_TIM6_CLK_ON 	(RCC->APB1ENR |= 0x00000010)
#define RCC_TIM6_CLK_OFF 	(RCC->APB1ENR &= ~0x00000010)

#define RCC_TIM14_CLK 		(RCC->APB1ENR & 0x00000100)
#define RCC_TIM14_CLK_ON 	(RCC->APB1ENR |= 0x00000100)
#define RCC_TIM14_CLK_OFF 	(RCC->APB1ENR &= ~0x00000100)

#define RCC_TIM15_CLK 		(RCC->APB2ENR & 0x00010000)
#define RCC_TIM15_CLK_ON 	(RCC->APB2ENR |= 0x00010000)
#define RCC_TIM15_CLK_OFF 	(RCC->APB2ENR &= ~0x00010000)

#define RCC_TIM16_CLK 		(RCC->APB2ENR & 0x00020000)
#define RCC_TIM16_CLK_ON 	(RCC->APB2ENR |= 0x00020000)
#define RCC_TIM16_CLK_OFF 	(RCC->APB2ENR &= ~0x00020000)

#define RCC_TIM17_CLK 		(RCC->APB2ENR & 0x00040000)
#define RCC_TIM17_CLK_ON 	(RCC->APB2ENR |= 0x00040000)
#define RCC_TIM17_CLK_OFF 	(RCC->APB2ENR &= ~0x00040000)

#define TIM1DisableInterrupt     (TIM1->DIER &= ~TIM_DIER_UIE)
#define TIM1EnableInterrupt      (TIM1->DIER |= TIM_DIER_UIE)

#define TIM17DisableInterrupt    (TIM17->DIER &= ~TIM_DIER_UIE)
#define TIM17EnableInterrupt     (TIM17->DIER |= TIM_DIER_UIE)


#define EnablePreload_TIM1_CH1    (TIM1->CCMR1 |= TIM_CCMR1_OC1PE)
#define EnablePreload_TIM1_CH2    (TIM1->CCMR1 |= TIM_CCMR1_OC2PE)
#define EnablePreload_TIM3_CH1    (TIM3->CCMR1 |= TIM_CCMR1_OC1PE)
#define EnablePreload_TIM3_CH2    (TIM3->CCMR1 |= TIM_CCMR1_OC2PE)
#define EnablePreload_TIM3_CH3    (TIM3->CCMR2 |= TIM_CCMR2_OC3PE)
#define EnablePreload_TIM3_CH4    (TIM3->CCMR2 |= TIM_CCMR2_OC4PE)

#define DisablePreload_TIM1_CH1    (TIM1->CCMR1 &= ~TIM_CCMR1_OC1PE)
#define DisablePreload_TIM1_CH2    (TIM1->CCMR1 &= ~TIM_CCMR1_OC2PE)
#define DisablePreload_TIM3_CH1    (TIM3->CCMR1 &= ~TIM_CCMR1_OC1PE)
#define DisablePreload_TIM3_CH2    (TIM3->CCMR1 &= ~TIM_CCMR1_OC2PE)
#define DisablePreload_TIM3_CH3    (TIM3->CCMR2 &= ~TIM_CCMR2_OC3PE)
#define DisablePreload_TIM3_CH4    (TIM3->CCMR2 &= ~TIM_CCMR2_OC4PE)


//--- Exported functions ---//
void TIM1_BRK_UP_TRG_COM_IRQHandler (void);
void TIM3_IRQHandler (void);
void TIM_3_Init(void);
void TIM_1_Init (void);
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

#endif    /* _TIM_H_ */

//--- end of file ---//
