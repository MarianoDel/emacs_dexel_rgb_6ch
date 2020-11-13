//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MODO_SLAVE.H #############################
//---------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _MODO_SLAVE_H_
#define _MODO_SLAVE_H_

//-- includes to help configurations --//
#include "switches_answers.h"


// Exported Types Constants and Macros -----------------------------------------
#define TT_MENU_TIMEOUT    30000
#define TT_DMX_RECEIVING    1000



// Exported Module Functions ---------------------------------------------------
void FuncSlaveMode (unsigned char *, sw_actions_t);
void FuncSlaveModeReset (void);
void UpdateTimerSlaveMode (void);


#endif    /* _SLAVE_MODE_H_ */

//--- end of file ---//
