//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### LIMITS_MENU.H #############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _LIMITS_MENU_H_
#define _LIMITS_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
void LimitsMenu_UpdateTimer (void);
void LimitsMenuReset (void);
resp_t LimitsMenu (parameters_typedef *, sw_actions_t);


#endif    /* _LIMITS_MENU_H_ */

//--- end of file ---//

