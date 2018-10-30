//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### POWER_CONTROL.H #######################
//---------------------------------------------

#ifndef _POWER_CONTROL_H_
#define _POWER_CONTROL_H_
#include "hard.h"

//-- Mode Configurations ----------

//--- Exported Types -----------------------------------------
typedef enum {
    PCKT_RX_IDLE = 0,
    PCKT_RX_LOOK_FOR_BREAK,
    PCKT_RX_LOOK_FOR_MARK,
    PCKT_RX_LOOK_FOR_START
} pckt_rx_t;

#define PwrCtrl_SetPWM_CH1(X)    PwrCtrl_SetPWM((1),(X))
#define PwrCtrl_SetPWM_CH2(X)    PwrCtrl_SetPWM((2),(X))
#define PwrCtrl_SetPWM_CH3(X)    PwrCtrl_SetPWM((3),(X))
#define PwrCtrl_SetPWM_CH4(X)    PwrCtrl_SetPWM((4),(X))
#define PwrCtrl_SetPWM_CH5(X)    PwrCtrl_SetPWM((5),(X))
#define PwrCtrl_SetPWM_CH6(X)    PwrCtrl_SetPWM((6),(X))

//--- Exported Module Functions ------------------------------
void PwrCtrl_SetPWM (unsigned char, unsigned char);


#endif /* _POWER_CONTROL_H_ */

//--- end of file ---//
