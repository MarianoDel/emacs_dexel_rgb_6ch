//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### VERSION_MENU.H ############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _VERSION_MENU_H_
#define _VERSION_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
void VersionMenu_UpdateTimer (void);
void VersionMenuReset (void);
resp_t VersionMenu (parameters_typedef *, sw_actions_t);


#endif    /* _VERSION_MENU_H_ */

//--- end of file ---//

