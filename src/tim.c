//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TIM.C ################################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "tim.h"
#include "hard.h"
#include "dmx_transceiver.h"
#include "flash_program.h"

// Externals -------------------------------------------------------------------
extern volatile unsigned short wait_ms_var;

#ifdef USE_DMX_TIMER_FAST
extern volatile unsigned short dmx_timer_hundreds_us_ch1;
extern volatile unsigned short dmx_timer_hundreds_us_ch2;
extern volatile unsigned short dmx_timer_hundreds_us_ch3;
extern volatile unsigned short dmx_timer_hundreds_us_ch4;
extern volatile unsigned short dmx_timer_hundreds_us_ch5;
extern volatile unsigned short dmx_timer_hundreds_us_ch6;
#endif


// Globals ---------------------------------------------------------------------
#ifdef USE_PWM_WITH_DITHER
#define SIZEOF_DITHER_VECT    8
volatile unsigned short v_dither_tim1_ch1[SIZEOF_DITHER_VECT] = { 0 };
volatile unsigned short v_dither_tim1_ch2[SIZEOF_DITHER_VECT] = { 0 };
volatile unsigned short v_dither_tim3_ch1[SIZEOF_DITHER_VECT] = { 0 };
volatile unsigned short v_dither_tim3_ch2[SIZEOF_DITHER_VECT] = { 0 };
volatile unsigned short v_dither_tim3_ch3[SIZEOF_DITHER_VECT] = { 0 };
volatile unsigned short v_dither_tim3_ch4[SIZEOF_DITHER_VECT] = { 0 };
volatile unsigned char dither_sequence_cnt = 0;

//                                              0     1     2     3     4     5     6     7
unsigned char v_sequence[SIZEOF_DITHER_VECT] = {0x00, 0x80, 0x88, 0xA8, 0xAA, 0xBA, 0xBB, 0xFB };
#endif




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

#ifdef HARDWARE_VERSION_2_0
    TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC1E;	//CH2 y CH1 enable on pin
#endif
#ifdef HARDWARE_VERSION_1_0
    TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2P | TIM_CCER_CC1E | TIM_CCER_CC1P;	//CH2 y CH1 enable on pin
#endif
    TIM1->BDTR |= TIM_BDTR_MOE;
    
    TIM1->ARR = DUTY_100_PERCENT;
    TIM1->CNT = 0;

#if defined USE_FREQ_48KHZ
    TIM1->PSC = 0;
#elif defined USE_FREQ_24KHZ
    TIM1->PSC = 1;
#elif defined USE_FREQ_16KHZ
    TIM1->PSC = 2;
#else
#error "set freq on hard.h"
#endif


    //Configuracion Pines
    //Alternate Fuction
    temp = GPIOA->AFR[1];
    temp &= 0xFFFFFF00;    
    temp |= 0x00000022;			//PA9 -> AF2; PA8 -> AF2
    GPIOA->AFR[1] = temp;

#ifdef USE_PWM_WITH_DITHER
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 9);
#endif
    
    // Enable timer interrupt ver UDIS
    // TIM1->DIER |= TIM_DIER_UIE;
    // NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
    // NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 1);
    
    TIM1->CR1 |= TIM_CR1_CEN;
}


#ifdef USE_PWM_WITH_DITHER
void TIM_LoadDitherSequences (unsigned char which_ch, unsigned short new_duty)
{
    volatile unsigned short * p1;

    switch (which_ch)
    {
    case 0:
        p1 = v_dither_tim1_ch1;
        break;

    case 1:
        p1 = v_dither_tim1_ch2;
        break;

    case 2:
        p1 = v_dither_tim3_ch1;
        break;

    case 3:
        p1 = v_dither_tim3_ch2;
        break;

    case 4:
        p1 = v_dither_tim3_ch3;
        break;

    case 5:
        p1 = v_dither_tim3_ch4;
        break;
    }        
            
    unsigned char seq_index = (unsigned char) (new_duty & 0x0007);
    unsigned char seq = v_sequence[seq_index];    

    unsigned short adj_duty = new_duty >> 3;
    unsigned short adj_duty_plus_one = adj_duty + 1;

    for (unsigned char i = 0; i < SIZEOF_DITHER_VECT; i++)
    {
        if (seq & 0x01)
            *(p1 + i) = adj_duty_plus_one;
        else
            *(p1 + i) = adj_duty;

        seq >>= 1;
    }
}


void TIM1_BRK_UP_TRG_COM_IRQHandler (void)    //48KHz or USE_FREQ_XXKHZ on hard.h
{
    //update the pwm mosfet channels with the pre-calculed sequences
    TIM1->CCR1 = v_dither_tim1_ch1[dither_sequence_cnt];
    TIM1->CCR2 = v_dither_tim1_ch2[dither_sequence_cnt];
    TIM3->CCR1 = v_dither_tim3_ch1[dither_sequence_cnt];
    TIM3->CCR2 = v_dither_tim3_ch2[dither_sequence_cnt];
    TIM3->CCR3 = v_dither_tim3_ch3[dither_sequence_cnt];
    TIM3->CCR4 = v_dither_tim3_ch4[dither_sequence_cnt];

    //update the sequence counter
    if (dither_sequence_cnt < (SIZEOF_DITHER_VECT - 1))
        dither_sequence_cnt++;
    else
        dither_sequence_cnt = 0;

    //clear flag
    if (TIM1->SR & TIM_SR_UIF)
        TIM1->SR &= ~TIM_SR_UIF;
}
#endif


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

#if defined USE_FREQ_48KHZ
    TIM3->PSC = 0;
#elif defined USE_FREQ_24KHZ
    TIM3->PSC = 1;
#elif defined USE_FREQ_16KHZ
    TIM3->PSC = 2;
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
    TIM16->ARR = a;
    TIM16->CR1 |= TIM_CR1_CEN;
}


void TIM17_IRQHandler (void)	
{
#ifdef USE_DMX_TIMER_FAST
    if (dmx_timer_hundreds_us_ch1)
        dmx_timer_hundreds_us_ch1--;

    if (dmx_timer_hundreds_us_ch2)
        dmx_timer_hundreds_us_ch2--;

    if (dmx_timer_hundreds_us_ch3)
        dmx_timer_hundreds_us_ch3--;

    if (dmx_timer_hundreds_us_ch4)
        dmx_timer_hundreds_us_ch4--;

    if (dmx_timer_hundreds_us_ch5)
        dmx_timer_hundreds_us_ch5--;

    if (dmx_timer_hundreds_us_ch6)
        dmx_timer_hundreds_us_ch6--;
#endif
    if (TIM17->SR & TIM_SR_UIF)
        TIM17->SR &= ~TIM_SR_UIF;    
}


//100us tick
void TIM_17_Init (void)    //en centanas de microsegundos
{
    if (!RCC_TIM17_CLK)
        RCC_TIM17_CLK_ON;

    //Configuracion del timer.
    TIM17->ARR = 100;
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


//--- end of file ---//
