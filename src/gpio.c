//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### GPIO.C ################################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "stm32f0xx.h"
#include "hard.h"



//--- Private typedef ---//
//--- Private define ---//
//--- Private macro ---//
//--- Private variables ---//
//--- Private function prototypes ---//
//--- Private functions ---//

//-------------------------------------------//
// @brief  GPIO configure.
// @param  None
// @retval None
//------------------------------------------//
void GPIO_Config (void)
{
    unsigned long temp;

    //--- MODER ---//
    //00: Input mode (reset state)
    //01: General purpose output mode
    //10: Alternate function mode
    //11: Analog mode

    //--- OTYPER ---//
    //These bits are written by software to configure the I/O output type.
    //0: Output push-pull (reset state)
    //1: Output open-drain

    //--- ORSPEEDR ---//
    //These bits are written by software to configure the I/O output speed.
    //x0: Low speed.
    //01: Medium speed.
    //11: High speed.
    //Note: Refer to the device datasheet for the frequency.

    //--- PUPDR ---//
    //These bits are written by software to configure the I/O pull-up or pull-down
    //00: No pull-up, pull-down
    //01: Pull-up
    //10: Pull-down
    //11: Reserved


#ifdef GPIOA_ENABLE
    //--- GPIO A ---//
    if (!GPIOA_CLK)
        GPIOA_CLK_ON;

    temp = GPIOA->MODER;	//2 bits por pin
    temp &= 0xFC300000;		//PA0 - PA1 (analog input); PA2 - PA3 alternative; PA4 - PA7 analog
    temp |= 0x000AFFAF;		//PA8 - PA9 (alternative); PA11 & PA12 input
    // temp |= 0x0005FFAF;		//PA8 - PA9 out PARA PRUEBAS; PA11 & PA12 input    
    GPIOA->MODER = temp;

    temp = GPIOA->OTYPER;	//1 bit por pin
    temp &= 0xFFFFFCFF;         //PA8 PA9 open drain
    temp |= 0x00000300;
    GPIOA->OTYPER = temp;

    temp = GPIOA->OSPEEDR;	//2 bits por pin
    temp &= 0xFFF0FF0F;         //PA2-PA3 low speed; PA8-PA9 low speed
    temp |= 0x00000000;		
    GPIOA->OSPEEDR = temp;

    temp = GPIOA->PUPDR;	//2 bits por pin
    temp &= 0xFC3FFFFF;         //PA11 PA12 pull up
    temp |= 0x01400000;
    GPIOA->PUPDR = temp;
    
#endif

#ifdef GPIOB_ENABLE

    //--- GPIO B ---//
    if (!GPIOB_CLK)
        GPIOB_CLK_ON;

    temp = GPIOB->MODER;	//2 bits por pin
    temp &= 0xFFF00030;		//PB0 PB1 (alternative); PB3 out; PB4 - PB7 alternative
    temp |= 0x0004AA4A;         //PB8 exti (input); PB9 out
    GPIOB->MODER = temp;

    temp = GPIOB->OTYPER;	//1 bit por pin
    temp &= 0xFFFFFFCC;        //PB1 PB0 open drain; PB4 - PB5 open drain
    temp |= 0x00000033;
    GPIOB->OTYPER = temp;

    temp = GPIOB->OSPEEDR;	//2 bits por pin
    temp &= 0xFFF00000;        //
    temp |= 0x00000000;		//low speed
    GPIOB->OSPEEDR = temp;

    temp = GPIOB->PUPDR;	//2 bits por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOB->PUPDR = temp;

#endif

#ifdef GPIOC_ENABLE

    //--- GPIO F ---//
    if (!GPIOC_CLK)
        GPIOC_CLK_ON;

    temp = GPIOC->MODER;	//2 bits por pin
    temp &= 0x03FFFC00;		//PC0 - PC3 out; PC4 analog
    temp |= 0x54000355;         //PC13 - PC15 out;
    GPIOC->MODER = temp;

    temp = GPIOC->OTYPER;	//1 bit por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOC->OTYPER = temp;

    temp = GPIOC->OSPEEDR;	//2 bits por pin
    temp &= 0x03FFFC00;
    temp |= 0x00000000;
    GPIOC->OSPEEDR = temp;

    temp = GPIOC->PUPDR;	//2 bits por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOC->PUPDR = temp;

#endif

#ifdef GPIOF_ENABLE

    //--- GPIO F ---//
    if (!GPIOF_CLK)
        GPIOF_CLK_ON;

    temp = GPIOF->MODER;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->MODER = temp;

    temp = GPIOF->OTYPER;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->OTYPER = temp;

    temp = GPIOF->OSPEEDR;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->OSPEEDR = temp;

    temp = GPIOF->PUPDR;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->PUPDR = temp;

#endif

    
#ifdef WITH_EXTI
    //Interrupt en PB8
    if (!SYSCFG_CLK)
        SYSCFG_CLK_ON;

    //EXTICR tiene 16bits, 4bits para cada exti, exti3->exti0 por registro de 16 bits
    // SYSCFG->EXTICR[0] = 0x0000; //Select Port A
    SYSCFG->EXTICR[2] = 0x0001; //Select Port B on EXTI8
    
    EXTI->IMR |= 0x0100; 			//Corresponding mask bit for interrupts PB8
    EXTI->EMR |= 0x0000; 			//Corresponding mask bit for events
    EXTI->RTSR |= 0x0100; 			//Pin Interrupt line on rising edge PB8
    EXTI->FTSR |= 0x0100; 			//Pin Interrupt line on falling edge PB8

    NVIC_EnableIRQ(EXTI4_15_IRQn);
    NVIC_SetPriority(EXTI4_15_IRQn, 2);
#endif
}

inline void EXTIOff (void)
{
    EXTI->IMR &= ~0x00000100;
}

inline void EXTIOn (void)
{
    EXTI->IMR |= 0x00000100;
}

void PB6_to_Alternative (void)
{
    unsigned int temp;

    temp = GPIOB->MODER;	//2 bits por pin
    temp &= 0xFFFFCFFF;		//PB6 (alternative)
    temp |= 0x00002000; 
    GPIOB->MODER = temp;    
}

void PB6_to_PushPull (void)
{
    unsigned int temp;
    
    temp = GPIOB->MODER;	//2 bits por pin
    temp &= 0xFFFFCFFF;		//PB6 out
    temp |= 0x00001000;         
    GPIOB->MODER = temp;
}

//--- end of file ---//
