//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### FIXED_MENU.H ##############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _FIXED_MENU_H_
#define _FIXED_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"
#include "parameters.h"


// Module Exported Types Constants and Macros ----------------------------------
#define FIXED_RED    0x01
#define FIXED_GREEN    0x02
#define FIXED_BLUE    0x04
#define FIXED_WARM    0x08
#define FIXED_COLD    0x10



// Module Exported Functions ---------------------------------------------------
void FixedMenu_UpdateTimer (void);
void FixedMenuReset (void);
resp_t FixedMenu (parameters_typedef *, sw_actions_t);
void FixedMenu_SetColors (unsigned char);

#endif    /* _FIXED_MENU_H_ */

//--- end of file ---//

