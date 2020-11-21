//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MASTER_SLAVE_MODE.H #######################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _MASTER_SLAVE_MODE_H_
#define _MASTER_SLAVE_MODE_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"

    

// Module Exported Functions ---------------------------------------------------
resp_t MasterSlaveMode (parameters_typedef *, sw_actions_t);
void MasterSlaveModeReset (void);
void MasterSlaveMode_UpdateTimers (void);

#endif    /* _MASTER_SLAVE_MODE_H_ */

//--- end of file ---//

