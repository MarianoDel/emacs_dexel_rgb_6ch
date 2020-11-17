//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### COLORS_MENU.H ##############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _COLORS_MENU_H_
#define _COLORS_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
void ColorsMenu_UpdateTimer (void);
void ColorsMenuReset (void);
resp_t ColorsMenu (parameters_typedef *, sw_actions_t);


#endif    /* _COLORS_MENU_H_ */

//--- end of file ---//

