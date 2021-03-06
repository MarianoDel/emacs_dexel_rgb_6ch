//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MASTER_SLAVE_MENU.H #######################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _MASTER_SLAVE_MENU_H_
#define _MASTER_SLAVE_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
resp_t MasterSlaveMenu (parameters_typedef *, sw_actions_t);
void MasterSlaveMenuReset (void);


#endif    /* _MASTER_SLAVE_MENU_H_ */

//--- end of file ---//

