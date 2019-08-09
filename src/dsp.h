//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DSP.H #################################
//---------------------------------------------
#ifndef _DSP_H_
#define _DSP_H_

//--- Defines for configuration -----------------
// #define USE_PID_CONTROLLERS
// #define USE_PID_FIXED_CONSTANTS
// #define USE_PID_UPDATED_CONSTANTS

// #define USE_MA8_CIRCULAR
#define USE_MA16_CIRCULAR
// #define USE_MA32_CIRCULAR

//--- Exported constants ------------------------

//--- Exported types ----------------------------
typedef struct {
    unsigned short v_ma[16];
    unsigned short * p_ma;
    unsigned int total_ma;
} ma16_data_obj_t;

// #define MAFilter32Pote(X)  MAFilter32Circular(X, v_pote_samples, &v_pote_index, &pote_sumation)

//--- Module Functions --------------------------
unsigned short RandomGen (unsigned int);
unsigned char MAFilter (unsigned char, unsigned char *);
unsigned short MAFilterFast (unsigned short ,unsigned short *);
unsigned short MAFilterFast16 (unsigned short, unsigned short *);
unsigned short MAFilter8 (unsigned short *);
unsigned short MAFilter32 (unsigned short, unsigned short *);
void SetNewValueInVector (unsigned short, unsigned short *);
unsigned short MAFilter32Circular (unsigned short, unsigned short *, unsigned char *, unsigned int *);

short PID (short, short);
short PID_roof (short, short, short, short *, short *);
void DSP_Vector_Calcule_Frequencies (unsigned short *,
                                     unsigned char,
                                     unsigned short *,
                                     unsigned char,
                                     unsigned char *);
unsigned short DSP_Vector_Get_Max_Value (unsigned short *, unsigned char);
unsigned short DSP_Vector_Get_Min_Value (unsigned short *, unsigned char);

#ifdef USE_MA16_CIRCULAR
void MA16Circular_Reset (ma16_data_obj_t *);
unsigned short MA16Circular (ma16_data_obj_t *, unsigned short);
unsigned short MA16Circular_Only_Calc (ma16_data_obj_t *);
#endif



#endif /* _DSP_H_ */
