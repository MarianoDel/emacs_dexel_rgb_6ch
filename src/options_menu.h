//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### OPTIONS_MENU.H ############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _OPTIONS_MENU_H_
#define _OPTIONS_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"


// Exported Types Constants and Macros -----------------------------------------
typedef struct {
    char * argv [8];
    unsigned char options_qtty;
    unsigned char options_selected;
    
} options_menu_st;


// Module Exported Functions ---------------------------------------------------
resp_t OptionsMenu (options_menu_st *, sw_actions_t);
void OptionsMenuReset (void);


#endif    /* _OPTIONS_MENU_H_ */

//--- end of file ---//

