//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARDWARE_MODE.H ###########################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _HARDWARE_MODE_H_
#define _HARDWARE_MODE_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
void HardwareMode_UpdateTimers (void);
resp_t HardwareMode (parameters_typedef *, sw_actions_t);
void HardwareModeReset (void);


#endif    /* _HARDWARE_MODE_H_ */

//--- end of file ---//

