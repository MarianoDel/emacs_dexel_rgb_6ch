//----------------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DISPLAY_UTILS.H ##############################
//----------------------------------------------------
#ifndef _DISPLAY_UTILS_H_
#define _DISPLAY_UTILS_H_


// Module Exported Types Constants and Macros ----------------------------------
#define Display_BlankLine1(X)    Display_BlankLine(1,(X))
#define Display_BlankLine2(X)    Display_BlankLine(2,(X))
#define Display_BlankLine3(X)    Display_BlankLine(3,(X))
#define Display_BlankLine4(X)    Display_BlankLine(4,(X))
#define Display_BlankLine5(X)    Display_BlankLine(5,(X))
#define Display_BlankLine6(X)    Display_BlankLine(6,(X))
#define Display_BlankLine7(X)    Display_BlankLine(7,(X))
#define Display_BlankLine8(X)    Display_BlankLine(8,(X))

#define Display_SetLine1(X)    Display_SetLine(1,(X))
#define Display_SetLine2(X)    Display_SetLine(2,(X))
#define Display_SetLine3(X)    Display_SetLine(3,(X))
#define Display_SetLine4(X)    Display_SetLine(4,(X))
#define Display_SetLine5(X)    Display_SetLine(5,(X))
#define Display_SetLine6(X)    Display_SetLine(6,(X))
#define Display_SetLine7(X)    Display_SetLine(7,(X))
#define Display_SetLine8(X)    Display_SetLine(8,(X))


#define ALL_LINE_LENGTH    21
#define ALL_LINE_LENGTH_NULL    (ALL_LINE_LENGTH + 1)


// Module Exported Functions ---------------------------------------------------
void Display_StartLines (void);
void Display_ClearLines (void);

void Display_BlankLine (unsigned char);
void Display_SetLine (unsigned char, char *);
void Display_SetOptions (unsigned char, char *);


#endif    /* _DISPLAY_UTILS_H_ */

//--- end of file ---//
