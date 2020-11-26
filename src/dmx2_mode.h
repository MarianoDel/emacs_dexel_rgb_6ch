//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX2_MODE.H ###############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _DMX2_MODE_H_
#define _DMX2_MODE_H_

//-- includes to help configurations --//
#include "switches_answers.h"


// Exported Types Constants and Macros -----------------------------------------



// Exported Module Functions ---------------------------------------------------
resp_t DMX2Mode (unsigned char *, sw_actions_t);
void DMX2ModeReset (void);
void DMX2Mode_UpdateTimers (void);


#endif    /* _DMX1_MODE_H_ */

//--- end of file ---//
