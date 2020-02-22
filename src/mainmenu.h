//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F303
// ##
// #### MAINMENU.H #################################
//---------------------------------------------
#ifndef _MAINMENU_H_
#define _MAINMENU_H_

#include "hard.h"

// Config Defines --------------------------------------------------------------


// Common Defines --------------------------------------------------------------
    

// Module Exported Functions ---------------------------------------------------
resp_t MainMenu_Update (sw_actions_t);
void MainMenu_Init (void);


#endif    /* _MAINMENU_H_ */
