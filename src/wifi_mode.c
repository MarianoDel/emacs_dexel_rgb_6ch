//----------------------------------------------------------------------
// #### PROYECTO DEXEL/LUIS_CASINO 6CH BIDIRECCIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### WIFI_MODE.C ####################################################
//----------------------------------------------------------------------
#include "wifi_mode.h"
#include "programs_functions.h"

/* Module Private Types -------------------------------------------------------*/
typedef enum {
    WIFI_RGBW = 0,
    WIFI_FUNDIDO_W,
    WIFI_FUNDIDO_W1,
    WIFI_FUNDIDO_W2,
    WIFI_FUNDIDO_RGB,
    WIFI_FUNDIDO_RGB1,
    WIFI_FUNDIDO_RGB2,
    WIFI_FUNDIDO_RGB3    

} wifi_state_t;

/* Externals variables --------------------------------------------------------*/
extern volatile unsigned char wifi_timer;

/* Global variables -----------------------------------------------------------*/
wifi_state_t wifi_state = WIFI_FUNDIDO_RGB;


/* Module Private Function Declarations ---------------------------------------*/


/* Module Functions Definitions -----------------------------------------------*/
void FuncsWifiMode (unsigned char * ch_val)
{
    //llamo a la funcion de programas

    // COMM_ReadMsgs(&wifi_state, ch_values);    //TODO: ver esto podrian modificarse sin querer

    switch(wifi_state)
    {
    case WIFI_RGBW:
        //paso los valores que llegan por el serie a los filtros
                
        break;

    case WIFI_FUNDIDO_RGB:
        for (unsigned char i = 0; i < 6; i++)
            *(ch_val + i) = 0;
                
        wifi_state++;
        Func_Fading_Reset();
        break;

    case WIFI_FUNDIDO_RGB1:
        if (!wifi_timer)
        {
            //busco los nuevos valores para el fade
            resp_t resp = resp_continue;
            do {
                resp = Func_Fading(ch_val, 0);
            }
            while (resp == resp_continue);
            wifi_timer = TIMER_WIFI_FADE;
            if (resp == resp_finish)
                wifi_state++;
        }
        break;

    case WIFI_FUNDIDO_RGB2:
        if (!wifi_timer)
        {
            //busco los nuevos valores para el fade
            resp_t resp = resp_continue;
            do {
                resp = Func_Fading(ch_val, 1);
            }
            while (resp == resp_continue);
            wifi_timer = TIMER_WIFI_FADE;
            if (resp == resp_finish)
                wifi_state++;
        }
        break;

    case WIFI_FUNDIDO_RGB3:
        if (!wifi_timer)
        {
            //busco los nuevos valores para el fade
            resp_t resp = resp_continue;
            do {
                resp = Func_Fading(ch_val, 2);
            }
            while (resp == resp_continue);
            wifi_timer = TIMER_WIFI_FADE;
            if (resp == resp_finish)
                wifi_state = WIFI_FUNDIDO_RGB1;
        }
        break;
                
    case WIFI_FUNDIDO_W:
        for (unsigned char i = 0; i < 6; i++)
            *(ch_val + i) = 0;
                
        wifi_state++;
        Func_Fading_Reset();
        break;

    case WIFI_FUNDIDO_W1:
        if (!wifi_timer)
        {
            //busco los nuevos valores para el fade
            resp_t resp = resp_continue;
            do {
                resp = Func_Fading(ch_val, 3);
            }
            while (resp == resp_continue);
            // resp = Func_Fading(ch_val, 3);
            wifi_timer = TIMER_WIFI_FADE;
        }
        break;

    default:
        wifi_state = WIFI_FUNDIDO_W;
        break;
    }

}

//--- end of file ---//
