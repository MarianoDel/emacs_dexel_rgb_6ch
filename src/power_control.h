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

//--- Exported Module Functions ------------------------------
void PwrCtrl_SetPWM (unsigned char, unsigned char);


#endif /* _POWER_CONTROL_H_ */

//--- end of file ---//
