//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "font.h"
#include "parameters.h"
#include "switches_answers.h"
#include "ssd1306_gfx.h"
#include "ssd1306_params.h"
#include "dmx1_menu.h"
#include "display_utils.h"

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


// Externals -------------------------------------------------------------------
parameters_typedef mem_conf;
extern uint8_t* _displaybuf;
extern uint16_t _displaybuf_size;

// Globals ---------------------------------------------------------------------
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;


// Module Functions to Test ----------------------------------------------------
void display_clear(void);
void display_update(void);

// unsigned char mem_buffer [DISPLAYHEIGHT * DISPLAYWIDTH / 8] = { 0 };


typedef struct {
    int first_line;
    int last_line;
    int first_segment;

} GGRAM_displayed_st;

typedef enum {
    DISPLAY_NONE = 0,
    DISPLAY_UP,
    DISPLAY_DOWN

} display_actions_e;

void draw_box_tittle (WINDOW *, char *);
void draw_box_bottom (WINDOW *, char *);
void draw_box_bottom_right (WINDOW *, char *);
void draw_box_content (WINDOW *, char *);

void ShowDisplay (WINDOW *, GGRAM_displayed_st *, unsigned char *);


void * KeyboardInput (void * arg);
    

// Module Functions ------------------------------------------------------------
sw_actions_t action = do_nothing;
display_actions_e display_actions = DISPLAY_NONE;
int main_loop = 1;
int dmx_up = 0;
int dmx_dwn = 0;

WINDOW * ggram_win;
WINDOW * help_win;
GGRAM_displayed_st ggram_displayed;

int main(int argc, char *argv[])
{
    initscr();			/* Start curses mode 		*/
    cbreak();			/* Line buffering disabled, Pass on
					 * everty thing to me 		*/
    keypad(stdscr, TRUE);		/* I need that nifty F1 	*/

    start_color();			/* Start color 			*/

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);    
    refresh();
    
    ggram_win = newwin(34, 130, 0, 0);
    wattron(ggram_win, COLOR_PAIR(1));
    wborder(ggram_win, '|','|','-','-','+','+','+','+');
    wrefresh(ggram_win);	

    help_win = newwin(3, 130, 34, 0);
    wattron(help_win, COLOR_PAIR(2));
    wborder(help_win, '|','|','-','-','+','+','+','+');
    wrefresh(help_win);	

    draw_box_tittle(help_win, "Help Menu");    
    mvwprintw(help_win,1,1, "u -> up  d -> dwn  s -> show or not addr j -> dmx up  k -> dmx down  F1 -> quit");
    wrefresh(help_win);

    ggram_displayed.first_line = 0;
    ggram_displayed.last_line = 16;
    ggram_displayed.first_segment = 0;
    ShowDisplay (ggram_win, &ggram_displayed, _displaybuf);

    //start main loop
    pthread_t p1;
    int rc;

    rc = pthread_create(&p1, NULL, KeyboardInput, (void *)rc);
    if (rc){
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    ///////////////////////////////////////////////////////////////
    // THIS IS THE RAM MEMORY GETTED FROM FLASH (SAVED OR EMPTY) //
    ///////////////////////////////////////////////////////////////
    //memory empty use some defaults
    mem_conf.program_type = SLAVE_MODE;
    mem_conf.master_send_dmx_enable = 0;
    mem_conf.last_program_in_flash = 9;
    mem_conf.last_program_deep_in_flash = 0;
    mem_conf.dmx_first_channel = 10;
    mem_conf.dmx_channel_quantity = 6;
    mem_conf.max_power = 200;

    gfx_init(DISPLAYWIDTH, DISPLAYHEIGHT);    

    ////////////////////////////////////////////
    // THIS IS THE RAM DATA FOR THE DMX1 MODE //
    ////////////////////////////////////////////
    dmx1_menu_data_t dmx1_st;
    dmx1_st.dmx_first_chnl = &mem_conf.dmx_first_channel;
    // char s_temp[100];
    // sprintf(s_temp, "dmx: %d addr: %d", *dmx1_st.dmx_first_chnl, dmx1_st.dmx_first_chnl);
    // mvwprintw(help_win,1,1, s_temp);
    // wrefresh(help_win);

    unsigned char ch[6] = {0, 2, 3, 10, 128, 230};
    dmx1_st.pchannels = ch;

    int new_loop = 1;
    
    resp_t resp = resp_continue;
    DMX1ModeMenuReset();

    do {

        if (dmx_up)
        {
            for (int i = 0; i < 6; i++)
            {                
                if (ch[i] < 255)
                    ch[i] += 1;
            }
            new_loop = 1;
            dmx_up = 0;
        }

        if (dmx_dwn)
        {
            for (int i = 0; i < 6; i++)
            {                
                if (ch[i] > 0)
                    ch[i] -= 1;
            }
            new_loop = 1;
            dmx_dwn = 0;
        }

        if (action == selection_up)
        {
            if (mem_conf.dmx_first_channel < 506)
                mem_conf.dmx_first_channel++;

            new_loop = 1;
            
        }

        if (action == selection_dwn)
        {
            if (mem_conf.dmx_first_channel > 1)
                mem_conf.dmx_first_channel--;

            new_loop = 1;
            
        }

        if (action == selection_enter)
        {
            if (dmx1_st.show_addres)
                dmx1_st.show_addres = 0;
            else
                dmx1_st.show_addres = 1;
            
            new_loop = 1;
        }
        
        if (new_loop)
        {
            resp = DMX1ModeMenu(&dmx1_st);
            if (resp == resp_finish)
                new_loop = 0;
        }
        action = do_nothing;
        usleep(2000);

        ///////////////////////////////////////////////
        // Check if any move on display is necessary //
        ///////////////////////////////////////////////
        if (display_actions == DISPLAY_UP)
        {
            if (ggram_displayed.first_segment > 0)
            {
                ggram_displayed.first_segment--;
                ShowDisplay (ggram_win, &ggram_displayed, _displaybuf);
            }
            display_actions = DISPLAY_NONE;
        }

        if (display_actions == DISPLAY_DOWN)
        {
            if (ggram_displayed.first_segment < 4)
            {
                ggram_displayed.first_segment++;
                ShowDisplay (ggram_win, &ggram_displayed, _displaybuf);
            }
            display_actions = DISPLAY_NONE;
        }

    } while (main_loop);

    //end of main loop

    
    
    attroff(COLOR_PAIR(1));
    // getch();
    endwin();
}


int last_box_tittle_len = 0;
void draw_box_tittle (WINDOW * pw, char * tittle)
{
    int tittle_len = strlen(tittle);

    int dx = 0;
    int dy = 0;
    getmaxyx(pw, dy, dx);

    //clean last tittle
    int tittle_x = (dx - last_box_tittle_len) / 2;
    wmove(pw, 0, tittle_x);
    for (int i = 0; i < last_box_tittle_len; i++)
        waddch(pw, '-');

    last_box_tittle_len = tittle_len;
    
    //write new tittle
    if (dx >= tittle_len + 2)
    {
        tittle_x = (dx - tittle_len) / 2;
        mvwprintw(pw, 0, tittle_x, "%s", tittle);
        wrefresh(pw);
    }       
}


int last_box_bottom_len = 0;
void draw_box_bottom (WINDOW * pw, char * tittle)
{
    int tittle_len = strlen(tittle);

    int dx = 0;
    int dy = 0;
    getmaxyx(pw, dy, dx);

    //clean last tittle
    int tittle_x = (dx - last_box_bottom_len) / 2;
    wmove(pw, dy - 1, tittle_x);
    for (int i = 0; i < last_box_bottom_len; i++)
        waddch(pw, '-');

    last_box_bottom_len = tittle_len;
    
    if (dx >= tittle_len + 2)
    {
        tittle_x = (dx - tittle_len) / 2;
        mvwprintw(pw, dy - 1, tittle_x, "%s", tittle);
        wrefresh(pw);
    }       
}


int last_box_bottom_right_len = 0;
void draw_box_bottom_right (WINDOW * pw, char * tittle)
{
    int tittle_len = strlen(tittle);

    int dx = 0;
    int dy = 0;
    getmaxyx(pw, dy, dx);

    //clean last tittle
    int tittle_x = dx - last_box_bottom_right_len - 2;
    wmove(pw, dy - 1, tittle_x);
    for (int i = 0; i < last_box_bottom_right_len; i++)
        waddch(pw, '-');

    last_box_bottom_right_len = tittle_len;    
    
    tittle_x = dx - tittle_len - 2;
    mvwprintw(pw, dy - 1, tittle_x, "%s", tittle);

    wmove(pw, dy - 1, dx - 40);
    wrefresh(pw);
}


void * KeyboardInput (void * arg)
{
    int ch;
    while((ch = getch()) != KEY_F(1))
    {
        switch(ch)
        {
        case KEY_UP:
            display_actions = DISPLAY_UP;
            break;
            
        case KEY_DOWN:
            display_actions = DISPLAY_DOWN;
            break;

        case 'u':
            action = selection_up;
            break;

        case 'd':
            action = selection_dwn;
            break;

        case 's':
            action = selection_enter;
            break;

        case 'b':
            action = selection_back;
            break;

        case 'j':
            dmx_up = 1;
            break;

        case 'k':
            dmx_dwn = 1;
            break;
            
        }
        ch = 0;
    }
    
    main_loop = 0;
    pthread_exit(NULL);
}


unsigned int disp_cnt = 0;
void ShowDisplay (WINDOW * pw, GGRAM_displayed_st * pdisp, unsigned char * mem)
{    
    //which lines of ggram to show
    //sanity checks
    if (pdisp->first_segment > 4)
        return;

    //64 lines grouped by 8; only show 4 segments of the 8 on the display
    int lines_cntr = 0;
    int first_seg_z = pdisp->first_segment * DISPLAYWIDTH;
    int last_seg_z = (pdisp->first_segment + 4) * DISPLAYWIDTH;
    for (int z = first_seg_z; z < last_seg_z; z += DISPLAYWIDTH)
    {
        //8 lines from 128bytes (DISPLAYWIDTH)
        for (int j = 0; j < DISPLAYHEIGHT / 8; j++)
        {
            unsigned char c = 1;
            c <<= j;
            wmove(pw, lines_cntr+j+1, 1);
            for (int i = 0; i < DISPLAYWIDTH; i++)
            {
                if (mem[z + i] & c)
                    // waddch(pw, '*'|A_BOLD);
                    waddch(pw, '*'|A_STANDOUT);
                else
                    waddch(pw, ' ');
            }
        }
        lines_cntr+=8;
    }

    disp_cnt++;
    
    char str [30] = { 0 };
    sprintf(str, "cntr: %d", disp_cnt);
    draw_box_bottom_right(pw, str);
    sprintf(str, "line num: %d", pdisp->first_segment * 8);
    draw_box_tittle(pw, str);
    sprintf(str, "line num: %d", (pdisp->first_segment + 4) * 8);
    draw_box_bottom(pw, str);
}


void display_clear(void)
{
    memset( _displaybuf, 0x00, _displaybuf_size );
}


void display_update(void)
{
    ShowDisplay(ggram_win, &ggram_displayed, _displaybuf);
}


//--- end of file ---//


