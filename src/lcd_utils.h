//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### LCD_UTILS.H #############################
//---------------------------------------------

#ifndef _LCD_UTILS_H_
#define _LCD_UTILS_H_

//-- Config Defines ----------
#define USE_SCROLL_SECOND_LINE
#define USE_SCROLL_FIRST_LINE

//las respuestas las paso hard.h
// typedef enum {
//     resp_ok = 0,
//     resp_continue,
//     resp_error,
//     resp_finish

// } resp_t;

//estados de la funcion BLINKING
#define BLINKING_INIT       0
#define BLINKING_MARK_D     1
#define BLINKING_SPACE_D    2
#define BLINKING_MARK_C     3
#define BLINKING_SPACE_C    4
#define BLINKING_MARK_N     5
#define BLINKING_SPACE_N    6

//modos de blinking
#define BLINK_DIRECT    0
#define BLINK_CROSS     1
#define BLINK_NO        2

//respuestas de las funciones
#define RESP_CONTINUE		0
#define RESP_SELECTED		1
#define RESP_CHANGE			2
#define RESP_CHANGE_ALL_UP	3
#define RESP_WORKING		4

#define RESP_FINISH			10
#define RESP_YES			11
#define RESP_NO				12
#define RESP_NO_CHANGE		13

#define RESP_OK				20
#define RESP_NOK			21
#define RESP_NO_ANSWER		22
#define RESP_TIMEOUT		23
#define RESP_READY			24



//estados de la funcion SHOW SELECT
#define SHOW_SELECT_INIT				0
#define SHOW_SELECT_1					1
#define SHOW_SELECT_2					2
#define SHOW_SELECT_3					3
#define SHOW_SELECT_SELECTED			4
#define SHOW_SELECT_SELECTED_1			5
#define SHOW_SELECT_CHANGE				6
#define SHOW_SELECT_CHANGE_1			7

//estados de la funcion OPTIONS
#define OPTIONS_INIT					0
#define OPTIONS_WAIT_SELECT			1
#define OPTIONS_WAIT_SELECT_1			2
#define OPTIONS_WAIT_SELECT_2			3
#define OPTIONS_WAIT_SELECT_3			4

#define OPTIONS_CHANGE_SELECT	10

//estados de la funcion SCROLL
#define SCROLL_INIT			0
#define SCROLL_SENDING			1
#define SCROLL_FINISH			2

//estados de la funcion CHANGE
#define CHANGE_INIT					0
#define CHANGE_WAIT_SELECT			1


//modos de change
#define CHANGE_PERCENT    0
#define CHANGE_SECS		  1
#define CHANGE_CHANNELS	  2
#define CHANGE_RESET	  0x80

//wrapers de la funcion FuncChange
#define FuncChangePercent(X)	FuncChange(X, CHANGE_PERCENT, 0, 100)
#define FuncChangeSecs(X)		FuncChange(X, CHANGE_SECS, 0, 10)
#define FuncChangeSecsMove(X)		FuncChange(X, CHANGE_SECS, 30, 120)
#define FuncChangeChannels(X)	FuncChange(X, CHANGE_CHANNELS, 1, 255)
#define FuncChangePercentReset()	FuncChangeReset()
#define FuncChangeSecsReset()	FuncChangeReset()
#define FuncChangeChannelsReset()	FuncChangeReset()

//-------- Functions -------------
void UpdateTimerLCD (void);
unsigned char FuncShowBlink (const char * , const char * , unsigned char, unsigned char);
unsigned char FuncShowSelect (const char *);
unsigned char FuncShowSelectv2 (const char *);
unsigned char FuncOptions (const char *, const char *, unsigned char *, unsigned char, unsigned char);
unsigned char FuncScroll1 (const char *);
unsigned char FuncScroll2 (const char *);

//unsigned char FuncChange (unsigned char *);
unsigned char FuncChange (unsigned char *, unsigned char , unsigned char , unsigned char );

void FuncOptionsReset (void);
void FuncShowSelectv2Reset (void);
void FuncChangeReset (void);

#endif /* MAIN_MENU_H_ */
