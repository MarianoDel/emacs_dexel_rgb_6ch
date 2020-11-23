//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### RESET_MODE.H ##############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _RESET_MODE_H_
#define _RESET_MODE_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
resp_t ResetMode ( parameters_typedef *, sw_actions_t);
void ResetModeReset (void);
void ResetMode_Factory_Defaults (parameters_typedef *);

#endif    /* _RESET_MODE_H_ */

//--- end of file ---//

