//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Functions to Test ----------------------------------------------------
void ShowDisplay (unsigned char *);

unsigned char mem_buffer [128] = {[0 ... 127] = '#' };

// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    // printf("Ingrese letra a mostrar o enter para terminar: ");

    // char b = 1;
    // char a = 0;
    // system("/bin/stty raw");
    // while (b)
    // {
    //     a = getchar();
    //     printf("\r\ncaracter: %i\r\n", a);
    //     if (a == 'q')
    //         b = 0;
    // }
    
    // system("/bin/stty cooked");
    // printf("\n");

    ShowDisplay (mem_buffer);
    // for (unsigned char i = 0; i < sizeof(mem_buffer); i++)
    //     printf("%c",mem_buffer[i]);

    // printf("\n");
    return 0;
}


void ShowDisplay (unsigned char * mem)
{
    //first square side
    printf("+");
    for (int i = 0; i < 128; i++)
        printf("-");
    printf("+\n");

    //64 lines
    for (int j = 0; j < 64; j++)
    {
        printf("|");
        for (int i = 0; i < 128; i++)
        {
            if (mem[i])
                printf("#");
            else
                printf(" ");
        }
        printf("|\n");
    }
    
    //last square side
    printf("+");
    for (int i = 0; i < 128; i++)
        printf("-");
    printf("+\n");    
}


//--- end of file ---//


