//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "comm.h"

#include "parameters.h"
#include "switches_answers.h"

#include "tests_ok.h"

#include <stdio.h>
// #include <stdlib.h>
#include <string.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
unsigned char usart2_have_data = 0;
char new_uart_msg [200] = { 0 };
char last_uart_sended [200] = { 0 };
unsigned char fixed_colors_from_comms = 0;


// Module Auxialiary Functions -------------------------------------------------
void Usart2Send (char * msg);
unsigned char ReadUsart2Buffer (unsigned char * bout, unsigned short max_len);
void FixedMenu_SetColors (unsigned char color);



// Module Functions for testing ------------------------------------------------
void Test_Comms (void);
    

// Module Functions ------------------------------------------------------------


int main(int argc, char *argv[])
{

    Test_Comms ();
    
}


void Test_Comms (void)
{
    // Red Test
    fixed_colors_from_comms = 0;
    usart2_have_data = 1;
    strcpy(new_uart_msg, "red\n");

    UpdateCommunications();
    
    if (!(strcmp(last_uart_sended, "ok\n")))        
    {
        if ((usart2_have_data == 0) && (fixed_colors_from_comms == 0x01))
            PrintOK();
        else
            PrintERR();
            
    }

    // Green Test
    fixed_colors_from_comms = 0;
    usart2_have_data = 1;
    strcpy(new_uart_msg, "green\n");

    UpdateCommunications();
    
    if (!(strcmp(last_uart_sended, "ok\n")))        
    {
        if ((usart2_have_data == 0) && (fixed_colors_from_comms == 0x02))
            PrintOK();
        else
            PrintERR();
            
    }

    // Blue Test
    fixed_colors_from_comms = 0;
    usart2_have_data = 1;
    strcpy(new_uart_msg, "blue\n");

    UpdateCommunications();
    
    if (!(strcmp(last_uart_sended, "ok\n")))        
    {
        if ((usart2_have_data == 0) && (fixed_colors_from_comms == 0x04))
            PrintOK();
        else
            PrintERR();
            
    }
    
    // Warm Test
    fixed_colors_from_comms = 0;
    usart2_have_data = 1;
    strcpy(new_uart_msg, "warm\n");

    UpdateCommunications();
    
    if (!(strcmp(last_uart_sended, "ok\n")))        
    {
        if ((usart2_have_data == 0) && (fixed_colors_from_comms == 0x08))
            PrintOK();
        else
            PrintERR();
            
    }
    
    // Cold Test
    fixed_colors_from_comms = 0;
    usart2_have_data = 1;
    strcpy(new_uart_msg, "cold\n");

    UpdateCommunications();
    
    if (!(strcmp(last_uart_sended, "ok\n")))        
    {
        if ((usart2_have_data == 0) && (fixed_colors_from_comms == 0x10))
            PrintOK();
        else
            PrintERR();
            
    }
}


unsigned char ReadUsart2Buffer (unsigned char * bout, unsigned short max_len)
{
    unsigned char len = 0;
    len = strlen(new_uart_msg);
    if (max_len > len)
        strcpy(bout, new_uart_msg);
    else
        printf("error on ReadUsart2Buffer max_len\n");

    return len;
}


void Usart2Send (char * msg)
{
    strcpy(last_uart_sended, msg);
}


void FixedMenu_SetColors (unsigned char color)
{
    fixed_colors_from_comms |= color;
}

//--- end of file ---//


