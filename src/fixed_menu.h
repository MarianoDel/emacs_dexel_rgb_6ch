//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### FIXED_MENU.H ##############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _FIXED_MENU_H_
#define _FIXED_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
void FixedMenu_UpdateTimer (void);
void FixedMenuReset (void);
resp_t FixedMenu (parameters_typedef *, sw_actions_t);


#endif    /* _FIXED_MENU_H_ */

//--- end of file ---//

