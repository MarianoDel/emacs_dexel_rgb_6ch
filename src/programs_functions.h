//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### PROGRAMS_FUNCTIONS.H ##################
//---------------------------------------------

#ifndef _PROGRAMS_FUNCTIONS_H_
#define _PROGRAMS_FUNCTIONS_H_

#include "hard.h"
#include "tim.h"

//-------- Defines -------------
#define WHITE_PWM(X)	Change_PWM4(X)
#define BLUE_PWM(X)	Change_PWM3(X)
#define GREEN_PWM(X)	Change_PWM2(X)
#define RED_PWM(X)	Change_PWM1(X)

#define MAX_FADE 255
//#define MAX_FADE 256

//states del P7
#define P7_COLOR1_ON		0
#define P7_COLOR1_OFF		1
#define P7_COLOR2_ON		2
#define P7_COLOR2_OFF		3
#define P7_COLOR3_ON		4
#define P7_COLOR3_OFF		5
#define P7_COLOR4_ON		6
#define P7_COLOR4_OFF		7
#define P7_COLOR5_ON		8
#define P7_COLOR5_OFF		9
#define P7_COLOR6_ON		10
#define P7_COLOR6_OFF		11
#define P7_COLOR7_ON		12
#define P7_COLOR7_OFF		13

//states del P8
#define P8_INCREASE_COLOR1		0
#define P8_DECREASE_COLOR1		1
#define P8_INCREASE_COLOR2		2
#define P8_DECREASE_COLOR2		3
#define P8_INCREASE_COLOR3		4
#define P8_DECREASE_COLOR3		5
#define P8_INCREASE_COLOR4		6
#define P8_DECREASE_COLOR4		7
#define P8_INCREASE_COLOR5		8
#define P8_DECREASE_COLOR5		9
#define P8_INCREASE_COLOR6		10
#define P8_DECREASE_COLOR6		11
#define P8_INCREASE_COLOR7		12
#define P8_DECREASE_COLOR7		13


//states del P9
#define P9_FIRST_COLOR7							0
#define P9_INCREASE_COLOR1_DECREASE_COLOR7		1
#define P9_INCREASE_COLOR2_DECREASE_COLOR1		2
#define P9_INCREASE_COLOR3_DECREASE_COLOR2		3
#define P9_INCREASE_COLOR4_DECREASE_COLOR3		4
#define P9_INCREASE_COLOR5_DECREASE_COLOR4		5
#define P9_INCREASE_COLOR6_DECREASE_COLOR5		6
#define P9_INCREASE_COLOR7_DECREASE_COLOR6		7

//-------- Functions -------------
void UpdateProgTimers (void);
void Func_PX_Ds(unsigned char, unsigned char, unsigned char);
void ResetLastValues(void);
void Func_PX(unsigned char, unsigned char);
//void Func_For_Cat(unsigned char, unsigned char, unsigned char, unsigned char);

#endif /* PROGRAMS_FUNCTIONS_H_ */
