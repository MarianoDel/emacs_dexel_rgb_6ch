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
typedef enum {
    do_nothing = 0,
    selection_up,
    selection_dwn,
    selection_enter,
    selection_back

} mm_action_t;
    

// Module Exported Functions ---------------------------------------------------
resp_t MainMenu_Update (mm_action_t);
void MainMenu_Init (void);


#endif    /* _MAINMENU_H_ */
