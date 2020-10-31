//----------------------------------------------------------
// #### PROYECTO DEXEL 6CH BIDIRECCIONAL - Custom Board ####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.H ###################################
//----------------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _TEST_FUNTIONS_H_
#define _TEST_FUNTIONS_H_


// Exported Types --------------------------------------------------------------


// Exported Constants or Macros ------------------------------------------------


// Exported Functions ----------------------------------------------------------
void TF_Voltage_Temperature (void);
void TF_Control_Fan (void);
void TF_Oled_Screen (void);
void TF_Oled_and_Main_Menu (void);
void TF_Oled_and_Slave_Mode (void);
void TF_Oled_and_Programs_Mode (void);
void TF_Oled_and_Master_Mode (void);

#endif    /* _TEST_FUNTIONS_H_ */

//--- end of file ---//

