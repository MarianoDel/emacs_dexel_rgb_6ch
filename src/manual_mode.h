//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MANUAL_MODE.H #############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _MANUAL_MODE_H_
#define _MANUAL_MODE_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
resp_t ManualMode (parameters_typedef *, sw_actions_t);
void ManualModeReset (void);
void ManualMode_UpdateTimers (void);

#endif    /* _MANUAL_MODE_H_ */

//--- end of file ---//

