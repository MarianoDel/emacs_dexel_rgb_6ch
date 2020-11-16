//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MANUAL_MENU.H #############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _MANUAL_MENU_H_
#define _MANUAL_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
resp_t ManualMenu (parameters_typedef *, sw_actions_t);
void ManualMenuReset (void);


#endif    /* _MANUAL_MENU_H_ */

//--- end of file ---//

