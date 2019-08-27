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

//comunicaciones
#include "uart.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>

/* Module Private Types -------------------------------------------------------*/
typedef enum {
    WIFI_RGBW = 0,
    WIFI_RGBW1,
    WIFI_FUNDIDO_W,
    WIFI_FUNDIDO_W1,
    WIFI_FUNDIDO_W2,
    WIFI_FUNDIDO_RGB,
    WIFI_FUNDIDO_RGB1,
    WIFI_FUNDIDO_RGB2,
    WIFI_FUNDIDO_RGB3    

} wifi_state_t;

typedef enum {
    RGB_SIGNAL_ON,
    RGB_SIGNAL_OFF,
    W_SIGNAL_ON,
    W_SIGNAL_OFF

} fade_t;


/* Externals variables --------------------------------------------------------*/
extern volatile unsigned char wifi_timer;
extern unsigned char usart2_have_data;


/* Global variables -----------------------------------------------------------*/
wifi_state_t wifi_state = WIFI_FUNDIDO_RGB;
char buffMessages [100];
unsigned char rgbw_sliders [6] = { 0 };

char s_fade_rgb [] = {"fade_rgb:"};
char s_fade_w [] = {"fade_w:"};
char s_on [] = {"on"};
char s_off [] = {"off"};
char s_R_from_sliders [] = {"R:"};
char s_G_from_sliders [] = {"G:"};
char s_B_from_sliders [] = {"B:"};
char s_W_from_sliders [] = {"W:"};


/* Module Private Function Declarations ---------------------------------------*/
void WIFI_UpdateCommunications (void);
unsigned char WIFI_SerialProcess (void);
resp_t WIFI_InterpretarMsg (void);
void WIFI_SetFadeType (fade_t);
void WIFI_SetPowerLed (unsigned char, unsigned short);

/* Module Functions Definitions -----------------------------------------------*/
void FuncsWifiMode (unsigned char * ch_val)
{
    //llamo a la funcion de programas
    WIFI_UpdateCommunications();

    switch(wifi_state)
    {
    case WIFI_RGBW:
        for (unsigned char i = 0; i < 6; i++)
            *(ch_val + i) = 0;
                
        wifi_state++;
        break;

    case WIFI_RGBW1:
        //paso los valores que llegan por el serie a los filtros
        *(ch_val + 0) = rgbw_sliders [0];
        *(ch_val + 1) = rgbw_sliders [1];
        *(ch_val + 2) = rgbw_sliders [2];
        *(ch_val + 3) = rgbw_sliders [3];
        *(ch_val + 4) = rgbw_sliders [4];
        *(ch_val + 5) = rgbw_sliders [5];
                
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


void WIFI_UpdateCommunications (void)
{
    if (WIFI_SerialProcess() > 2)	//si tiene algun dato significativo
    {
        WIFI_InterpretarMsg();
    }
}


//Procesa consultas desde la raspberry
//carga el buffer buffMessages y avisa con el flag msg_ready
unsigned char WIFI_SerialProcess (void)
{
    unsigned char bytes_readed = 0;

    if (usart2_have_data)
    {
        usart2_have_data = 0;
        bytes_readed = ReadUsart2Buffer((unsigned char *) buffMessages, sizeof(buffMessages));
    }
    return bytes_readed;
}


resp_t WIFI_InterpretarMsg (void)
{
    resp_t resp = resp_ok;
    char * pStr = buffMessages;
    unsigned short new_power = 0;
    unsigned char decimales = 0;
    // char b [30];

    //-- Fade RGB Settings
    if (strncmp(pStr, s_fade_rgb, sizeof(s_fade_rgb) - 1) == 0)
    {
        pStr += sizeof(s_fade_rgb) - 1;    //normalizo al payload

        if (strncmp(pStr, s_on, sizeof(s_on) - 1) == 0)
            WIFI_SetFadeType (RGB_SIGNAL_ON);
        else if (strncmp(pStr, s_off, sizeof(s_off) - 1) == 0)
            WIFI_SetFadeType (RGB_SIGNAL_OFF);
        else
            resp = resp_error;
    }

    //-- Fade W Settings
    else if (strncmp(pStr, s_fade_w, sizeof(s_fade_w) - 1) == 0)
    {
        pStr += sizeof(s_fade_w) - 1;    //normalizo al payload

        if (strncmp(pStr, s_on, sizeof(s_on) - 1) == 0)
            WIFI_SetFadeType (W_SIGNAL_ON);
        else if (strncmp(pStr, s_off, sizeof(s_off) - 1) == 0)
            WIFI_SetFadeType (W_SIGNAL_OFF);
        else
            resp = resp_error;
    }

    //-- RGBW Settings
    else if (strncmp(pStr, s_R_from_sliders, sizeof(s_R_from_sliders) - 1) == 0)
    {
        resp = resp_error;
        
        pStr += sizeof(s_R_from_sliders) - 1;    //esto llega sin espacios

        //busco valor de R
        //lo que viene son 1 2 o 3 bytes
        decimales = StringIsANumber(pStr, &new_power);
        if (decimales < 4)
        {
            WIFI_SetPowerLed (0, new_power);
            pStr += decimales + 1;

            //busco valor de G
            if (strncmp(pStr, s_G_from_sliders, sizeof(s_G_from_sliders) - 1) == 0)
            {
                pStr += sizeof(s_G_from_sliders) - 1;    //esto llega sin espacios

                //lo que viene son 1 2 o 3 bytes
                decimales = StringIsANumber(pStr, &new_power);
                if (decimales < 4)
                {
                    WIFI_SetPowerLed (1, new_power);
                    pStr += decimales + 1;
                    
                    //busco valor de B
                    if (strncmp(pStr, s_B_from_sliders, sizeof(s_B_from_sliders) - 1) == 0)
                    {
                        pStr += sizeof(s_B_from_sliders) - 1;    //esto llega sin espacios

                        //lo que viene son 1 2 o 3 bytes
                        decimales = StringIsANumber(pStr, &new_power);
                        if (decimales < 4)
                        {
                            WIFI_SetPowerLed (2, new_power);
                            pStr += decimales + 1;

                            //busco valor de W
                            if (strncmp(pStr, s_W_from_sliders, sizeof(s_W_from_sliders) - 1) == 0)
                            {
                                pStr += sizeof(s_W_from_sliders) - 1;    //esto llega sin espacios

                                //lo que viene son 1 2 o 3 bytes
                                decimales = StringIsANumber(pStr, &new_power);
                                if (decimales < 4)
                                {
                                    WIFI_SetPowerLed (3, new_power);
                                }
                            }
                        }
                    }
                }
            }
        }
        else
            resp = resp_error;
    }

    return resp;
}


void WIFI_SetFadeType (fade_t fade_type)
{
    switch (fade_type)
    {
    case RGB_SIGNAL_ON:
        wifi_state = WIFI_FUNDIDO_RGB;
        break;

    case RGB_SIGNAL_OFF:
        wifi_state = WIFI_RGBW;        
        break;

    case W_SIGNAL_ON:
        wifi_state = WIFI_FUNDIDO_W;        
        break;

    case W_SIGNAL_OFF:
        wifi_state = WIFI_RGBW;            
        break;

    default:
        break;
    }
}


void WIFI_SetPowerLed (unsigned char ch, unsigned short new_power)
{
    if (ch < 6)
    {
        if (new_power > 255)
            new_power = 255;
        
        rgbw_sliders[ch] = (unsigned char) new_power;
    }
}

//--- end of file ---//
