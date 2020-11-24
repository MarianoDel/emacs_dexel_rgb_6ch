//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### CURRENT_MENU.H ############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _CURRENT_MENU_H_
#define _CURRENT_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
void CurrentMenu_UpdateTimer (void);
void CurrentMenuReset (void);
resp_t CurrentMenu (parameters_typedef *, sw_actions_t);


#endif    /* _CURRENT_MENU_H_ */

//--- end of file ---//

