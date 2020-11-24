//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEMP_MENU.H ###############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _TEMP_MENU_H_
#define _TEMP_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
void TempMenu_UpdateTimer (void);
void TempMenuReset (void);
resp_t TempMenu (parameters_typedef *, sw_actions_t);


#endif    /* _TEMP_MENU_H_ */

//--- end of file ---//

