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

void MainMenu_SetTitle (char *);
void MainMenu_BlankOptions (void);
void MainMenu_SetOptions (unsigned char);

void MainMenu_BlankAllLines (void);
void MainMenu_BlankLine1 (void);
void MainMenu_BlankLine2 (void);
void MainMenu_BlankLine3 (void);
void MainMenu_BlankLine4 (void);
void MainMenu_BlankLine5 (void);
void MainMenu_BlankLine6 (void);
void MainMenu_SetLine1 (char * s);
void MainMenu_SetLine2 (char * s);
void MainMenu_SetLine3 (char * s);
void MainMenu_SetLine4 (char * s);
void MainMenu_SetLine5 (char * s);
void MainMenu_SetLine6 (char * s);


#endif    /* _MAINMENU_H_ */
