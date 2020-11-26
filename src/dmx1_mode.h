//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX1_MODE.H ###############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _DMX1_MODE_H_
#define _DMX1_MODE_H_

//-- includes to help configurations --//
#include "switches_answers.h"


// Exported Types Constants and Macros -----------------------------------------



// Exported Module Functions ---------------------------------------------------
resp_t DMX1Mode (unsigned char *, sw_actions_t);
void DMX1ModeReset (void);
void DMX1Mode_UpdateTimers (void);


#endif    /* _DMX1_MODE_H_ */

//--- end of file ---//
