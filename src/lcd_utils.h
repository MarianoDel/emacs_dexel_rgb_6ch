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

//-- README ------------------------------
//necesita dos funciones de teclas para los menues
//checkS1()
//checkS2()

//-- Config Defines for the LCD used ----------
#define LINE_LENGTH    8
#define USE_SCROLL_SECOND_LINE
#define USE_SCROLL_FIRST_LINE

#if (LINE_LENGTH == 8)
#define LINE_LENGTH_8
#endif

#if (LINE_LENGTH == 16)
#define LINE_LENGTH_16
#endif


//-- Config Defines for the functions used ----------
//for blinking in ms
#define TT_BLINKING_IN_ON    1000
#define TT_BLINKING_IN_OFF   600

//for show select in ms
#define TT_SHOW_SELECT_IN_ON    1000
#define TT_SHOW_SELECT_IN_OFF   500

//for func scroll in ms
#define TT_SCROLL    250


//-- Internals Defines ----------

//las respuestas las paso hard.h porque las reuso en otro lados
// typedef enum {
//     resp_ok = 0,
//     resp_continue,
//     resp_selected,
//     resp_change,
//     resp_change_all_up,
//     resp_working,
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
#define BLINKING_MARK_N_SENDING_FIRST_LINE     6
#define BLINKING_MARK_N_SENDING_SECOND_LINE    7
#define BLINKING_SPACE_N    8

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
#define OPTIONS_WAIT_SELECT_TIMEOUT        5

#define OPTIONS_CHANGE_SELECT	10

//estados de la funcion OPTIONS ON OFF
#define OPTIONS_ONOFF_INIT          0
#define OPTIONS_ONOFF_REDRAW        1
#define OPTIONS_ONOFF_WAIT_IN_ON    2
#define OPTIONS_ONOFF_WAIT_IN_OFF   3
#define OPTIONS_ONOFF_SELECT_OPTION    4
#define OPTIONS_ONOFF_WAIT_FREE_S1    5
#define OPTIONS_ONOFF_CHANGE_OPTION    6


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
#define CHANGE_PROGRAMS    3
#define CHANGE_VOLTAGE_MAINS    4
#define CHANGE_MAX_POWER        5

#define CHANGE_RESET	  0x80

//wrapers de la funcion FuncChange
#define FuncChangePercent(X)	FuncChange(X, CHANGE_PERCENT, 0, 100)
#define FuncChangeSecs(X)		FuncChange(X, CHANGE_SECS, 0, 10)
#define FuncChangeSecsMove(X)		FuncChange(X, CHANGE_SECS, 30, 120)
#define FuncChangeChannels(X)	FuncChange(X, CHANGE_CHANNELS, 1, 511)
#define FuncChangeChannelsQuantity(X)	FuncChange(X, CHANGE_CHANNELS, 1, 6)
#define FuncChangePercentReset()	FuncChangeReset()
#define FuncChangeSecsReset()	FuncChangeReset()
#define FuncChangeChannelsReset()	FuncChangeReset()
#define FuncChangePrograms(X)	FuncChange(X, CHANGE_PROGRAMS, 1, 9)
#define FuncChangeProgramsSequence(X)	FuncChange(X, CHANGE_PROGRAMS, 0, 9)


//-------- Functions -------------
void UpdateTimerLCD (void);
unsigned char FuncShowBlink (const char * , const char * , unsigned char, unsigned char);
unsigned char FuncShowSelect (const char *);
unsigned char FuncShowSelectv2 (const char *);
unsigned char FuncOptions (const char *, const char *, unsigned char *, unsigned char, unsigned char);
unsigned char FuncOptionsOnOff (unsigned char *);
unsigned char FuncScroll1 (const char *);
unsigned char FuncScroll2 (const char *);

//unsigned char FuncChange (unsigned char *);
unsigned char FuncChange (unsigned short *, unsigned char, unsigned short, unsigned short);
unsigned char FuncChangeOnOff (unsigned char *);

void FuncOptionsReset (void);
void FuncShowSelectv2Reset (void);
void FuncChangeReset (void);

void LCDClearScreen (void);

unsigned char FuncChangeDecimals (unsigned char *, unsigned char *, 
                                  unsigned char , unsigned char ,    
                                  unsigned char , unsigned char);    


#endif /* MAIN_MENU_H_ */
