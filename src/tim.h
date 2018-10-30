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

//--- Exported types ---//
//--- Exported constants ---//
#define DUTY_50_PERCENT		500
#define DUTY_60_PERCENT		600
#define DUTY_65_PERCENT		650
#define DUTY_70_PERCENT		700
#define DUTY_80_PERCENT		800
#define DUTY_90_PERCENT		900
#define DUTY_95_PERCENT		950
#define DUTY_100_PERCENT	1000
#define DUTY_ALWAYS			(DUTY_100_PERCENT + 1)



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

#define TIM1DisableInterrupt      TIM1->DIER &= ~TIM_DIER_UIE;
#define TIM1EnableInterrupt      TIM1->DIER |= TIM_DIER_UIE;

#define Update_PWM1(X)    Update_TIM1_CH1(X)
#define Update_PWM2(X)    Update_TIM1_CH2(X)
#define Update_PWM3(X)    Update_TIM3_CH1(X)
#define Update_PWM4(X)    Update_TIM3_CH2(X)
#define Update_PWM5(X)    Update_TIM3_CH3(X)
#define Update_PWM6(X)    Update_TIM3_CH4(X)

//--- Exported functions ---//
void TIM1_BRK_UP_TRG_COM_IRQHandler (void);
void TIM3_IRQHandler (void);
void TIM_3_Init(void);
void TIM_1_Init_Irq (void);
void TIM_1_Init_Only_PWM (void);
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

void Change_PWM1 (unsigned char);
void Change_PWM2 (unsigned char);
void Change_PWM3 (unsigned char);
void Change_PWM4 (unsigned char);
void Change_PWM5 (unsigned char);
void Change_PWM6 (unsigned char);

void Wait_ms (unsigned short wait);
#endif
//--- End ---//


//--- END OF FILE ---//
