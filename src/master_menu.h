//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MASTER_MENU.H #############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _MASTER_MENU_H_
#define _MASTER_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
resp_t MasterMenu (parameters_typedef *, sw_actions_t);
void MasterMenuReset (void);


#endif    /* _MASTER_MENU_H_ */

//--- end of file ---//

