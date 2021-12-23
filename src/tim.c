//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TIM.C ################################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "tim.h"
#include "hard.h"
#include "dmx_transceiver.h"
#include "flash_program.h"

// Externals -------------------------------------------------------------------
extern volatile unsigned short wait_ms_var;



// Globals ---------------------------------------------------------------------

// Module Private Functions ----------------------------------------------------
unsigned short CalcNewDelta (unsigned short, unsigned short);


// Module Functions ------------------------------------------------------------
void Update_TIM1_CH1 (unsigned short a)
{
    TIM1->CCR1 = a;
}

void Update_TIM1_CH2 (unsigned short a)
{
    TIM1->CCR2 = a;
}

void Update_TIM3_CH1 (unsigned short a)
{
    TIM3->CCR1 = a;
}

void Update_TIM3_CH2 (unsigned short a)
{
    TIM3->CCR2 = a;
}

void Update_TIM3_CH3 (unsigned short a)
{
    TIM3->CCR3 = a;
}

void Update_TIM3_CH4 (unsigned short a)
{
    TIM3->CCR4 = a;
}

void Wait_ms (unsigned short wait)
{
    wait_ms_var = wait;

    while (wait_ms_var);
}


//-------------------------------------------//
// @brief  TIM configure.
// @param  None
// @retval None
//------------------------------------------//
void TIM3_IRQHandler (void)	
{
    //bajar flag
    if (TIM3->SR & TIM_SR_UIF)	//bajo el flag
        TIM3->SR &= ~TIM_SR_UIF;
}


void TIM_1_Init (void)
{
    unsigned long temp;

    if (!RCC_TIM1_CLK)
        RCC_TIM1_CLK_ON;

    //Configuracion del timer.
    TIM1->CR1 = 0x00;		//clk int / 1; upcounting
    TIM1->CR2 |= TIM_CR2_MMS_1;		//UEV -> TRG0

    TIM1->SMCR = 0x0000;
    TIM1->CCMR1 = 0x6060;			//CH2 y CH1 output PWM mode 1
    TIM1->CCMR2 = 0x0000;
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE;

#ifdef HARDWARE_VERSION_2_2
    TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC1E;	//CH2 y CH1 enable on pin
#endif
#ifdef HARDWARE_VERSION_2_0
    TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC1E;	//CH2 y CH1 enable on pin
#endif
#ifdef HARDWARE_VERSION_1_0
    TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2P | TIM_CCER_CC1E | TIM_CCER_CC1P;	//CH2 y CH1 enable on pin
#endif
    TIM1->BDTR |= TIM_BDTR_MOE;
    
    TIM1->ARR = DUTY_100_PERCENT;
    TIM1->CNT = 0;

#if (defined USE_FREQ_16KHZ) || (defined USE_FREQ_12KHZ)
    TIM1->PSC = 0;
#elif (defined USE_FREQ_8KHZ) || (defined USE_FREQ_6KHZ)
    TIM1->PSC = 1;
#elif defined USE_FREQ_4KHZ
    TIM1->PSC = 2;
#elif defined USE_FREQ_4_8KHZ
    TIM1->PSC = 9;
#else
#error "set freq on hard.h"
#endif


    //Configuracion Pines
    //Alternate Fuction
    temp = GPIOA->AFR[1];
    temp &= 0xFFFFFF00;    
    temp |= 0x00000022;			//PA9 -> AF2; PA8 -> AF2
    GPIOA->AFR[1] = temp;

    // Enable timer interrupt ver UDIS
    // TIM1->DIER |= TIM_DIER_UIE;
    // NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
    // NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 1);
    
    TIM1->CR1 |= TIM_CR1_CEN;
}


void TIM_3_Init (void)
{
    unsigned long temp;

    if (!RCC_TIM3_CLK)
        RCC_TIM3_CLK_ON;

    //Configuracion del timer.
    TIM3->CR1 = 0x00;		//clk int / 1; upcounting
    TIM3->CR2 = 0x00;		//igual al reset

    TIM3->SMCR |= TIM_SMCR_SMS_2;			//trigger: reset mode; link timer 1
    // TIM3->SMCR |= TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1;	//trigger: trigger mode; link timer 1
    TIM3->CCMR1 = 0x6060;      //CH1, CH2 output PWM mode 1 (channel active TIM3->CNT < TIM3->CCR1)
    TIM3->CCMR2 = 0x6060;      //CH3, CH4 output PWM mode 1 (channel active TIM3->CNT < TIM3->CCR1)
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE;
    TIM3->CCMR2 |= TIM_CCMR2_OC3PE | TIM_CCMR2_OC4PE;

#ifdef HARDWARE_VERSION_2_2
    //CH4 CH3 CH2 y CH1 enable on pin
    TIM3->CCER |= TIM_CCER_CC4E | TIM_CCER_CC3E | TIM_CCER_CC2E | TIM_CCER_CC1E;
#endif
#ifdef HARDWARE_VERSION_2_0
    //CH4 CH3 CH2 y CH1 enable on pin
    TIM3->CCER |= TIM_CCER_CC4E | TIM_CCER_CC3E | TIM_CCER_CC2E | TIM_CCER_CC1E;
#endif
#ifdef HARDWARE_VERSION_1_0
    //CH4 CH3 CH2 y CH1 enable on pin & polarity reversal
    TIM3->CCER |= TIM_CCER_CC4E | TIM_CCER_CC4P |
        TIM_CCER_CC3E | TIM_CCER_CC3P |
        TIM_CCER_CC2E |
        TIM_CCER_CC1E | TIM_CCER_CC1P;
#endif
    

    TIM3->ARR = DUTY_100_PERCENT;        //tick cada 20.83us --> 48KHz
    TIM3->CNT = 0;

#if (defined USE_FREQ_16KHZ) || (defined USE_FREQ_12KHZ)
    TIM3->PSC = 0;
#elif (defined USE_FREQ_8KHZ) || (defined USE_FREQ_6KHZ)
    TIM3->PSC = 1;
#elif defined USE_FREQ_4KHZ
    TIM3->PSC = 2;
#elif defined USE_FREQ_4_8KHZ
    TIM3->PSC = 9;
#else
#error "set freq on hard.h"
#endif


    //Configuracion Pines
    //Alternate Fuction
    // temp = GPIOA->AFR[0];
    // temp &= 0x00FFFFFF;
    // temp |= 0x11000000;			//PA7 -> AF1; PA6 -> AF1
    // GPIOA->AFR[0] = temp;

    temp = GPIOB->AFR[0];
    temp &= 0xFF00FF00;                 //PB5 -> AF1; PB4 -> AF1
    temp |= 0x00110011;			//PB1 -> AF1; PB0 -> AF1
    GPIOB->AFR[0] = temp;

    // Enable timer ver UDIS
    //TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1 |= TIM_CR1_CEN;

}


void TIM_14_Init (void)
{
    if (!RCC_TIM14_CLK)
        RCC_TIM14_CLK_ON;

    //Configuracion del timer.
    TIM14->CR1 = 0x00;		//clk int / 1; upcounting; uev
    TIM14->PSC = 47;			//tick cada 1us
    TIM14->ARR = 0xFFFF;			//para que arranque
    TIM14->EGR |= 0x0001;
}


void TIM_16_Init (void)
{
    if (!RCC_TIM16_CLK)
        RCC_TIM16_CLK_ON;

    //Configuracion del timer.
    TIM16->ARR = 0;
    TIM16->CNT = 0;
    TIM16->PSC = 47;

    // Enable timer interrupt ver UDIS
    TIM16->DIER |= TIM_DIER_UIE;
    TIM16->CR1 |= TIM_CR1_URS | TIM_CR1_OPM;	//solo int cuando hay overflow y one shot

    NVIC_EnableIRQ(TIM16_IRQn);
    NVIC_SetPriority(TIM16_IRQn, 7);
}


void TIM16_IRQHandler (void)	//es one shoot
{
    SendDMXPacket(PCKT_UPDATE);

    if (TIM16->SR & 0x01)
        TIM16->SR = 0x00;    //bajar flag
}


void OneShootTIM16 (unsigned short a)
{
    // TIM16->CNT = 0;
    TIM16->ARR = a;
    TIM16->CR1 |= TIM_CR1_CEN;
}




//100us tick
void TIM_17_Init (void)    //en centanas de microsegundos
{
    if (!RCC_TIM17_CLK)
        RCC_TIM17_CLK_ON;

    //Configuracion del timer.
    TIM17->ARR = 200;
    TIM17->CNT = 0;
    TIM17->PSC = 47;

    //Configuracion canal PWM
    // TIM17->CCMR1 = 0x0060;      //CH1 output PWM mode 1 (channel active TIM->CNT < TIM->CCR1)

    // Enable timer ver UDIS
    // TIM17->DIER |= TIM_DIER_CC1IE | TIM_DIER_UIE;
    TIM17->DIER |= TIM_DIER_UIE;    
    TIM17->CR1 |= TIM_CR1_CEN;

    NVIC_EnableIRQ(TIM17_IRQn);
    NVIC_SetPriority(TIM17_IRQn, 4);
}


unsigned short CalcNewDelta (unsigned short last_ch_delta, unsigned short ch_value)
{
#ifdef DELTA_MULTIPLE_STEPS_100
    if ((last_ch_delta + 100) < ch_value)
        last_ch_delta += 10;
    else if ((last_ch_delta + 50) < ch_value)
        last_ch_delta += 5;
    else if ((last_ch_delta + 20) < ch_value)
        last_ch_delta += 2;
    else if (last_ch_delta < ch_value)
        last_ch_delta += 1;


    if (last_ch_delta > (ch_value + 100))
        last_ch_delta -= 10;
    else if (last_ch_delta > (ch_value + 50))
        last_ch_delta -= 5;
    else if (last_ch_delta > (ch_value + 20))
        last_ch_delta -= 2;
    else if (last_ch_delta > ch_value)
        last_ch_delta -= 1;
#endif

#ifdef DELTA_MULTIPLE_STEPS_50
    if ((last_ch_delta + 50) < ch_value)
        last_ch_delta += 3;
    else if ((last_ch_delta + 25) < ch_value)
        last_ch_delta += 2;
    else if (last_ch_delta < ch_value)
        last_ch_delta += 1;


    if (last_ch_delta > (ch_value + 50))
        last_ch_delta -= 3;
    else if (last_ch_delta > (ch_value + 25))
        last_ch_delta -= 2;
    else if (last_ch_delta > ch_value)
        last_ch_delta -= 1;
#endif
    
#ifdef DELTA_SINGLE_STEP
    if (last_ch_delta < ch_value)
        last_ch_delta++;

    if (last_ch_delta > ch_value)
        last_ch_delta--;
#endif
    
    return last_ch_delta;
}

//--- end of file ---//
