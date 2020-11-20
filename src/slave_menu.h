//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### SLAVE_MENU.H ##############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _SLAVE_MENU_H_
#define _SLAVE_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
void SlaveMenu_UpdateTimer (void);
void SlaveMenuReset (void);
resp_t SlaveMenu (parameters_typedef *, sw_actions_t);


#endif    /* _SLAVE_MENU_H_ */

//--- end of file ---//

