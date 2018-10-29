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

//--- VARIABLES EXTERNAS ---//
extern volatile unsigned char timer_1seg;
extern volatile unsigned short timer_led_comm;
extern volatile unsigned short wait_ms_var;
extern parameters_typedef mem_conf;

//--- VARIABLES GLOBALES ---//

volatile unsigned short timer_1000 = 0;
volatile unsigned char tim14_counter = 0;

volatile unsigned short tim_soft_pwm_counter = 0;
volatile unsigned short tim_soft_pwm_ch1 = 0;
volatile unsigned short tim_soft_pwm_ch2 = 0;
volatile unsigned short tim_soft_pwm_ch3 = 0;
volatile unsigned short tim_soft_pwm_ch4 = 0;
volatile unsigned short tim_soft_pwm_ch5 = 0;
volatile unsigned short tim_soft_pwm_ch6 = 0;

#define pwm_ch1 tim_soft_pwm_ch1
#define pwm_ch2 tim_soft_pwm_ch2
#define pwm_ch3 tim_soft_pwm_ch3
#define pwm_ch4 tim_soft_pwm_ch4
#define pwm_ch5 tim_soft_pwm_ch5
#define pwm_ch6 tim_soft_pwm_ch6

//--- FUNCIONES DEL MODULO ---//
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

void Change_PWM1 (unsigned char a)
{
    TIM1->DIER &= ~TIM_DIER_UIE;
    pwm_ch1 = a;
    TIM1->DIER |= TIM_DIER_UIE;
}

void Change_PWM2 (unsigned char a)
{
    TIM1->DIER &= ~TIM_DIER_UIE;
    pwm_ch2 = a;
    TIM1->DIER |= TIM_DIER_UIE;
}

void Change_PWM3 (unsigned char a)
{
    TIM1->DIER &= ~TIM_DIER_UIE;
    pwm_ch3 = a;
    TIM1->DIER |= TIM_DIER_UIE;
}

void Change_PWM4 (unsigned char a)
{
    TIM1->DIER &= ~TIM_DIER_UIE;
    pwm_ch4 = a;
    TIM1->DIER |= TIM_DIER_UIE;
}

void Change_PWM5 (unsigned char a)
{
    TIM1->DIER &= ~TIM_DIER_UIE;
    pwm_ch5 = a;
    TIM1->DIER |= TIM_DIER_UIE;
}

void Change_PWM6 (unsigned char a)
{
    TIM1->DIER &= ~TIM_DIER_UIE;
    pwm_ch6 = a;
    TIM1->DIER |= TIM_DIER_UIE;
}

//-------------------------------------------//
// @brief  TIM configure.
// @param  None
// @retval None
//------------------------------------------//
void TIM1_BRK_UP_TRG_COM_IRQHandler (void)	//48Khz
{
    if (tim_soft_pwm_counter < 255)
    {        
        tim_soft_pwm_counter++;
        if (tim_soft_pwm_counter != 255)
        {
            if (tim_soft_pwm_counter >= pwm_ch1)
                Update_PWM1(0);
            if (tim_soft_pwm_counter >= pwm_ch2)
                Update_PWM2(0);
            if (tim_soft_pwm_counter >= pwm_ch3)
                Update_PWM3(0);
            if (tim_soft_pwm_counter >= pwm_ch4)
                Update_PWM4(0);
            if (tim_soft_pwm_counter >= pwm_ch5)
                Update_PWM5(0);
            if (tim_soft_pwm_counter >= pwm_ch6)
                Update_PWM6(0);
        }
    }
    else
    {
        tim_soft_pwm_counter = 0;
        //arranco todos los soft pwm
        if (pwm_ch1)
            Update_PWM1(mem_conf.pwm_chnls[0]);
        if (pwm_ch2)
            Update_PWM2(mem_conf.pwm_chnls[1]);
        if (pwm_ch3)
            Update_PWM3(mem_conf.pwm_chnls[2]);
        if (pwm_ch4)
            Update_PWM4(mem_conf.pwm_chnls[3]);
        if (pwm_ch5)
            Update_PWM5(mem_conf.pwm_chnls[4]);
        if (pwm_ch6)
            Update_PWM6(mem_conf.pwm_chnls[5]);
    }
        //bajar flag
    if (TIM1->SR & 0x01)	//bajo el flag
        TIM1->SR = 0x00;
}

void TIM3_IRQHandler (void)	//1 ms
{
    /*
      Usart_Time_1ms ();

      if (timer_1seg)
      {
      if (timer_1000)
      timer_1000--;
      else
      {
      timer_1seg--;
      timer_1000 = 1000;
      }
      }

      if (timer_led_comm)
      timer_led_comm--;

      if (timer_standby)
      timer_standby--;
    */
    //bajar flag
    if (TIM3->SR & 0x01)	//bajo el flag
        TIM3->SR = 0x00;
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

    TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2P | TIM_CCER_CC1E | TIM_CCER_CC1P;	//CH2 y CH1 enable on pin

    TIM1->BDTR |= TIM_BDTR_MOE;
    
    TIM1->ARR = DUTY_100_PERCENT;
    TIM1->CNT = 0;
    TIM1->PSC = 0;		

    //Configuracion Pines
    //Alternate Fuction
    temp = GPIOA->AFR[1];
    temp &= 0xFFFFFF00;    
    temp |= 0x00000022;			//PA9 -> AF2; PA8 -> AF2
    GPIOA->AFR[1] = temp;

    // Enable timer interrupt ver UDIS
    TIM1->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 6);
    
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

    TIM3->CCER |= TIM_CCER_CC4E | TIM_CCER_CC4P | TIM_CCER_CC3E | TIM_CCER_CC3P | TIM_CCER_CC2E | TIM_CCER_CC2P | TIM_CCER_CC1E | TIM_CCER_CC1P;	//CH4 CH3 CH2 y CH1 enable on pin & polarity reversal


    TIM3->ARR = DUTY_100_PERCENT;        //tick cada 20.83us --> 48KHz
    TIM3->CNT = 0;
    TIM3->PSC = 0;	      

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



// void TIM14_IRQHandler (void)	//20us
// {
//     if (TIM14->SR & 0x01)
//         TIM14->SR = 0x00;    //bajar flag

//     if (tim14_counter < 255)
//     {
//         tim14_counter++;

//         if (tim14_counter > set_laser_ch1)
//             LASER_CH1_OFF;

//         if (tim14_counter > set_laser_ch2)
//             LASER_CH2_OFF;

//         if (tim14_counter > set_laser_ch3)
//             LASER_CH3_OFF;

//         if (tim14_counter > set_laser_ch4)
//             LASER_CH4_OFF;
//     }
//     else
//     {
//         tim14_counter = 0;    //Overflow
//         if (set_laser_ch1)
//             LASER_CH1_ON;

//         if (set_laser_ch2)
//             LASER_CH2_ON;

//         if (set_laser_ch3)
//             LASER_CH3_ON;

//         if (set_laser_ch4)
//             LASER_CH4_ON;
//     }
// }

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

// void TIM_14_Init (void)
// {
//     if (!RCC_TIM14_CLK)
//         RCC_TIM14_CLK_ON;

//     //Configuracion del timer.
//     TIM14->CR1 = 0x00;		//clk int / 1; upcounting; uev
//     TIM14->PSC = 47;		//tick cada 1us
//     // TIM14->ARR = 20;	//int cada 20us
//     TIM14->ARR = 20;	//int cada 10us para pruebas
//     TIM14->EGR |= 0x0001;

//     // Enable timer interrupt ver UDIS
//     TIM14->DIER |= TIM_DIER_UIE;
//     TIM14->CR1 |= TIM_CR1_URS | TIM_CR1_CEN;	//solo int cuando hay overflow y one shot

//     NVIC_EnableIRQ(TIM14_IRQn);
//     NVIC_SetPriority(TIM14_IRQn, 8);
    
// }

void TIM17_IRQHandler (void)	//100uS
{
    //bajar flag
    if (TIM17->SR & 0x01)
        TIM17->SR = 0x00;
}


void TIM_17_Init (void)
{
    if (!RCC_TIM17_CLK)
        RCC_TIM17_CLK_ON;

    //Configuracion del timer.
    TIM17->ARR = 100; //100us
    TIM17->CNT = 0;
    TIM17->PSC = 47;
    TIM17->EGR = TIM_EGR_UG;

    // Enable timer ver UDIS
    TIM17->DIER |= TIM_DIER_UIE;
    TIM17->CR1 |= TIM_CR1_CEN;

    NVIC_EnableIRQ(TIM17_IRQn);
    NVIC_SetPriority(TIM17_IRQn, 8);
}

// void TIM17_IRQHandler (void)	//100uS
// {
//
// 	//if (GPIOA_PIN0_OUT)
// 	//	GPIOA_PIN0_OFF;
// 	//else
// 	//	GPIOA_PIN0_ON;
//
// 	if (TIM17->SR & 0x01)
// 		//bajar flag
// 		TIM17->SR = 0x00;
// }
//
//
// void TIM_17_Init (void)
// {
//
// 	NVIC_InitTypeDef NVIC_InitStructure;
//
// 	if (!RCC_TIM17_CLK)
// 		RCC_TIM17_CLK_ON;
//
// 	//Configuracion del timer.
// 	TIM17->ARR = 2000; //10m
// 	TIM17->CNT = 0;
// 	TIM17->PSC = 479;
// 	TIM17->EGR = TIM_EGR_UG;
//
// 	// Enable timer ver UDIS
// 	TIM17->DIER |= TIM_DIER_UIE;
// 	TIM17->CR1 |= TIM_CR1_CEN;
//
// 	NVIC_InitStructure.NVIC_IRQChannel = TIM17_IRQn;
// 	NVIC_InitStructure.NVIC_IRQChannelPriority = 5;
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// 	NVIC_Init(&NVIC_InitStructure);
// }

//--- end of file ---//
