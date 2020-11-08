//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "mainmenu.h"
#include "font.h"
#include "parameters.h"
#include "switches_answers.h"

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


// Externals -------------------------------------------------------------------
parameters_typedef mem_conf;

// Globals ---------------------------------------------------------------------
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;


// Module Functions to Test ----------------------------------------------------
void display_clear(void);
void display_update(void);
void display_setPixel( int16_t x, int16_t y, uint16_t color );
void display_line( int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color );

void gfx_init( int16_t width, int16_t height );
void gfx_hvLine( int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color );
int16_t gfx_width(void);
int16_t gfx_height(void);
uint8_t gfx_rotation(void);
void gfx_setCursor( int16_t x, int16_t y );
void gfx_setTextSize( uint8_t size );
void gfx_setTextColor( uint16_t color );
void gfx_setTextBg( uint16_t color );
void gfx_setTextWrap( uint8_t w );
void gfx_setRotation( uint8_t x );
void gfx_rotation_adjust( int16_t* px, int16_t* py );
void gfx_drawPixel( int16_t x, int16_t y, uint16_t color );
void gfx_hvLine( int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color );
void gfx_drawLine( int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color );
void gfx_drawRect( int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color );
void gfx_fillRect( int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color );
void gfx_drawCircle( int16_t x0, int16_t y0, int16_t r,uint16_t color );
void gfx_drawTriangle( int16_t x0, int16_t y0,int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color );
void gfx_drawChar( int16_t x, int16_t y, unsigned char c,uint16_t color, uint16_t bg, uint8_t size);
void gfx_write(uint8_t ch);
void gfx_print( const char* s );
void gfx_println( const char* s );

#define DISPLAYHEIGHT    64
#define DISPLAYWIDTH    128
// unsigned char mem_buffer [DISPLAYHEIGHT * DISPLAYWIDTH / 8] = {[256 ... 384] = 0xFF };
unsigned char mem_buffer [DISPLAYHEIGHT * DISPLAYWIDTH / 8] = { 0 };

// see data sheet page 25 for Graphic Display Data RAM organization
// 8 pages, each page a row of DISPLAYWIDTH bytes
// start address of of row: y/8*DISPLAYWIDTH
// x pos in row: == x 
#define GDDRAM_ADDRESS(X,Y) ((mem_buffer)+((Y)/8)*(DISPLAYWIDTH)+(X))

// lower 3 bit of y determine vertical pixel position (pos 0...7) in GDDRAM byte
// (y&0x07) == position of pixel on row (page). LSB is top, MSB bottom
#define GDDRAM_PIXMASK(Y) (1 << ((Y)&0x07))

#define PIXEL_ON(X,Y) (*GDDRAM_ADDRESS(x,y) |= GDDRAM_PIXMASK(y))
#define PIXEL_OFF(X,Y) (*GDDRAM_ADDRESS(x,y) &= ~GDDRAM_PIXMASK(y))
#define PIXEL_TOGGLE(X,Y) (*GDDRAM_ADDRESS(x,y) ^= GDDRAM_PIXMASK(y))

#define BLACK 0
#define WHITE 1
#define INVERSE 2   

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

void draw_win(WINDOW *);
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

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string);

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
    // attron(COLOR_PAIR(1));
    refresh();

    
    ggram_win = newwin(34, 130, 0, 0);
    wattron(ggram_win, COLOR_PAIR(1));
    wborder(ggram_win, '|','|','-','-','+','+','+','+');
    wrefresh(ggram_win);	

    help_win = newwin(3, 130, 34, 0);
    wattron(help_win, COLOR_PAIR(2));
    wborder(help_win, '|','|','-','-','+','+','+','+');
    wrefresh(help_win);	

    int x_ggram = 0;
    int y_ggram = 0;
    getmaxyx(ggram_win, y_ggram, x_ggram);
    
    char str [130] = { 0 };
    sprintf(str, "line num: %d", y_ggram);

    draw_box_tittle(help_win, "Help Menu");    
    draw_box_tittle(ggram_win, "lines in this sheet");
    draw_box_bottom(ggram_win, str);
    draw_box_bottom(ggram_win, "un string demasiado largo");
    draw_box_bottom_right(ggram_win, "cntr: 200");

    mvwprintw(help_win,1,1, "u -> up  d -> dwn  s -> select  b -> back  F1 -> quit");
    wrefresh(help_win);

    ggram_displayed.first_line = 0;
    ggram_displayed.last_line = 16;
    ggram_displayed.first_segment = 0;
    ShowDisplay (ggram_win, &ggram_displayed, mem_buffer);

    //start main loop
    pthread_t p1;
    int rc;

    rc = pthread_create(&p1, NULL, KeyboardInput, (void *)rc);
    if (rc){
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    gfx_init(DISPLAYWIDTH, DISPLAYHEIGHT);    
    MainMenu_Init ();
    do {

        MainMenu_Update(action);
        action = do_nothing;
        usleep(2000);

        if (display_actions == DISPLAY_UP)
        {
            if (ggram_displayed.first_segment > 0)
            {
                ggram_displayed.first_segment--;
                ShowDisplay (ggram_win, &ggram_displayed, mem_buffer);
            }
            display_actions = DISPLAY_NONE;
        }

        if (display_actions == DISPLAY_DOWN)
        {
            if (ggram_displayed.first_segment < 4)
            {
                ggram_displayed.first_segment++;
                ShowDisplay (ggram_win, &ggram_displayed, mem_buffer);
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


void draw_win(WINDOW *local_win)
{	
    wborder(local_win,'|','|','-','-','+','+','+','+');    
    /* The parameters taken are 
     * 1. win: the window on which to operate
     * 2. ls: character to be used for the left side of the window 
     * 3. rs: character to be used for the right side of the window 
     * 4. ts: character to be used for the top side of the window 
     * 5. bs: character to be used for the bottom side of the window 
     * 6. tl: character to be used for the top left corner of the window 
     * 7. tr: character to be used for the top right corner of the window 
     * 8. bl: character to be used for the bottom left corner of the window 
     * 9. br: character to be used for the bottom right corner of the window
     */
    wrefresh(local_win);
}


// int main (int argc, char *argv[])
// {

//     initscr();
//     start_color();			/* Start color functionality	*/
	
//     init_pair(1, COLOR_CYAN, COLOR_BLACK);
//     printw("A Big string which i didn't care to type fully ");    
//     mvchgat(0, 0, -1, A_BLINK, 1, NULL);	
// 	/* 
// 	 * First two parameters specify the position at which to start 
// 	 * Third parameter number of characters to update. -1 means till 
// 	 * end of line
// 	 * Forth parameter is the normal attribute you wanted to give 
// 	 * to the charcter
// 	 * Fifth is the color index. It is the index given during init_pair()
// 	 * use 0 if you didn't want color
// 	 * Sixth one is always NULL 
// 	 */
//     // refresh();
//     getch();
//     endwin();        
    
//     return 0;
// }


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
                    waddch(pw, '#');
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
    memset( mem_buffer, 0x00, sizeof(mem_buffer) );
}

void display_update(void)
{
    ShowDisplay(ggram_win, &ggram_displayed, mem_buffer);
}

// used by gfx_ functions. Needs to be implemented by display_
void display_setPixel( int16_t x, int16_t y, uint16_t color ){
    
    if( (x < 0) || (x >= DISPLAYWIDTH) || (y < 0) || (y >= DISPLAYHEIGHT) )
        return;

    switch( color ){
        case WHITE: 
            PIXEL_ON(x,y);
            break;
        case BLACK:
            PIXEL_OFF(x,y);
            break;
        case INVERSE: 
            PIXEL_TOGGLE(x,y);
            break;
    }
}


// draws horizontal or vertical line
// Note: no check for valid coords, this needs to be done by caller
// should only be called from gfx_hvline which is doing all validity checking
void display_line( int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color ){

    if( x1 == x2 ){
        // vertical
        uint8_t* pstart = GDDRAM_ADDRESS(x1,y1);
        uint8_t* pend = GDDRAM_ADDRESS(x2,y2);       
        uint8_t* ptr = pstart;             
        
        while( ptr <= pend ){
            
            uint8_t mask;
            if( ptr == pstart ){
                // top
                uint8_t lbit = y1 % 8;
                // bottom (line can be very short, all inside this one byte)
                uint8_t ubit = lbit + y2 - y1;
                if( ubit >= 7 )
                    ubit = 7;
                mask = ((1 << (ubit-lbit+1)) - 1) << lbit;    
            }else if( ptr == pend ){
                // top is always bit 0, that makes it easy
                // bottom
                mask = (1 << (y2 % 8)) - 1;    
            }

            if( ptr == pstart || ptr == pend ){
                switch( color ){
                    case WHITE:     *ptr |= mask; break;
                    case BLACK:     *ptr &= ~mask; break;
                    case INVERSE:   *ptr ^= mask; break;
                };  
            }else{
                switch( color ){
                    case WHITE:     *ptr = 0xff; break;
                    case BLACK:     *ptr = 0x00; break;
                    case INVERSE:   *ptr ^= 0xff; break;
                };  
            }
            
            ptr += DISPLAYWIDTH;
        }
    }else{
        // horizontal
        uint8_t* pstart = GDDRAM_ADDRESS(x1,y1);
        uint8_t* pend = pstart + x2 - x1;
        uint8_t pixmask = GDDRAM_PIXMASK(y1);    

        uint8_t* ptr = pstart;
        while( ptr <= pend ){
            switch( color ){
                case WHITE:     *ptr |= pixmask; break;
                case BLACK:     *ptr &= ~pixmask; break;
                case INVERSE:   *ptr ^= pixmask; break;
            };
            ptr++;
        }
    }
}

// ============================================================
// graphics library stuff

int16_t WIDTH, HEIGHT; // This is the 'raw' display w/h - never changes
static int16_t _width, _height; // Display w/h as modified by current rotation
static int16_t cursor_x, cursor_y;
static uint16_t textcolor, textbgcolor;
static uint8_t textsize;
uint8_t rotation;
uint8_t wrap; // If set, 'wrap' text at right edge of display



void gfx_init( int16_t width, int16_t height ){
    WIDTH = width;
    HEIGHT = height;
    _width = WIDTH;
    _height = HEIGHT;
    
    rotation = 0;
    cursor_y = cursor_x = 0;
    textsize = 1;
    textcolor = textbgcolor = 0xFFFF;
    wrap = 1;
}

// Return the size of the display (per current rotation)
int16_t gfx_width(void){
    return _width;
}

int16_t gfx_height(void){
    return _height;
}

uint8_t gfx_rotation(void){
    return rotation;
}

void gfx_setCursor( int16_t x, int16_t y ){
    cursor_x = x;
    cursor_y = y;
}

void gfx_setTextSize( uint8_t size ){
    textsize = (size > 0) ? size : 1;
}

void gfx_setTextColor( uint16_t color ){
    // For 'transparent' background, we'll set the bg
    // to the same as fg instead of using a flag
    textcolor = textbgcolor = color;
}

void gfx_setTextBg( uint16_t color ){
    textbgcolor = color;
}

void gfx_setTextWrap( uint8_t w ){
    wrap = w;
}

void gfx_setRotation( uint8_t x ){
    
    rotation = (x & 3);
    switch( rotation ){
        case 0:
        case 2:
            _width = WIDTH;
            _height = HEIGHT;
            break;
        case 1:
        case 3:
            _width = HEIGHT;
            _height = WIDTH;
        break;
    }
}

void gfx_rotation_adjust( int16_t* px, int16_t* py ){

    int16_t y0 = *py;
    
    switch( rotation ){
        case 1:
            *py = *px;
            *px = WIDTH - y0 - 1;
            break;
        case 2:
            *px = WIDTH - *px - 1;
            *py = HEIGHT - *py - 1;
            break;
        case 3:
            *py = HEIGHT - *px - 1;
            *px = y0;
            break;
    }
}

void gfx_drawPixel( int16_t x, int16_t y, uint16_t color ){
    
    if( (x < 0) || (x >= _width) || (y < 0) || (y >= _height) )
        return;
    
    gfx_rotation_adjust( &x, &y );

    display_setPixel(x,y,color);
}

// helper function for gfx_drawLine, handles special cases of horizontal and vertical lines
void gfx_hvLine( int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color ){
    
    if( x1 != x2 && y1 != y2 ){
        // neither vertical nor horizontal
        return;
    }    
    
    // bounds check
    if( rotation == 1 || rotation == 3 ){
        if( x1 < 0 || x1 >= HEIGHT || x2 < 0 || x2 >= HEIGHT )
            return;
        if( y1 < 0 || y1 >= WIDTH || y2 < 0 || y2 >= WIDTH )
            return;
    }else{
        if( y1 < 0 || y1 >= HEIGHT || y2 < 0 || y2 >= HEIGHT )
            return;
        if( x1 < 0 || x1 >= WIDTH || x2 < 0 || x2 >= WIDTH )
            return;
    }
    
    gfx_rotation_adjust( &x1, &y1 );
    gfx_rotation_adjust( &x2, &y2 );
    
    // ensure coords are from left to right and top to bottom
    if( (x1 == x2 && y2 < y1) || (y1 == y2 && x2 < x1) ){
        // swap as needed
        int16_t t = x1; x1 = x2; x2 = t;
        t = y1; y1 = y2; y2 = t;
    }
    
    display_line( x1, y1, x2, y2, color );
}

// always use this function for line drawing
void gfx_drawLine( int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color ){
 
    if( x0 == x1 || y0 == y1 ){
        // vertical and horizontal lines can be drawn faster
        gfx_hvLine( x0, y0, x1, y1, color );
        return;
    }
    
    int16_t t;
    
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if( steep ){
        t = x0; x0 = y0; y0 = t;
        t = x1; x1 = y1; y1 = t;
    }
    if( x0 > x1 ){
        t = x0; x0 = x1; x1 = t;
        t = y0; y0 = y1; y1 = t;
    }
    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);
    int16_t err = dx / 2;
    int16_t ystep;
    if( y0 < y1 ){
        ystep = 1;
    }else{
        ystep = -1;
    }
    for( ; x0<=x1; x0++ ){
        if( steep ){
            gfx_drawPixel( y0, x0, color );
        }else{
            gfx_drawPixel( x0, y0, color );
        }
        err -= dy;
        if( err < 0 ){
            y0 += ystep;
            err += dx;
        }
    }
}

void gfx_drawRect( int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color ){
    
    gfx_drawLine( x, y, x+w-1, y, color );
    gfx_drawLine( x, y+h-1, x+w-1, y+h-1, color );
    gfx_drawLine( x, y, x, y+h-1, color );
    gfx_drawLine( x+w-1, y, x+w-1, y+h-1, color );
}

void gfx_fillRect( int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color ){
    int16_t i = 0;
    if( h > w ){
        for( i = x ; i < x+w ; i++ ){
            gfx_drawLine( i, y, i, y+h-1, color );
        }
    }else{
        for( i = y ; i < y+h ; i++ ){
            gfx_drawLine( x, i, x+w-1, i, color );
        }
    }
}


// circle outline
void gfx_drawCircle( int16_t x0, int16_t y0, int16_t r,uint16_t color ){

    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    gfx_drawPixel( x0 , y0+r, color );
    gfx_drawPixel( x0 , y0-r, color );
    gfx_drawPixel( x0+r, y0 , color );
    gfx_drawPixel( x0-r, y0 , color );
    while( x < y ){
        if( f >= 0 ){
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        gfx_drawPixel( x0 + x, y0 + y, color );
        gfx_drawPixel( x0 - x, y0 + y, color );
        gfx_drawPixel( x0 + x, y0 - y, color );
        gfx_drawPixel( x0 - x, y0 - y, color );
        gfx_drawPixel( x0 + y, y0 + x, color );
        gfx_drawPixel( x0 - y, y0 + x, color );
        gfx_drawPixel( x0 + y, y0 - x, color );
        gfx_drawPixel( x0 - y, y0 - x, color );
    }
}

void gfx_drawTriangle( int16_t x0, int16_t y0,int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color ){
    
    gfx_drawLine( x0, y0, x1, y1, color );
    gfx_drawLine( x1, y1, x2, y2, color );
    gfx_drawLine( x2, y2, x0, y0, color );
}


// Draw a character
void gfx_drawChar( int16_t x, int16_t y, unsigned char c,uint16_t color, uint16_t bg, uint8_t size) {
    if( (x >= _width) || // Clip right
        (y >= _height) || // Clip bottom
        ((x + 6 * size - 1) < 0) || // Clip left
        ((y + 8 * size - 1) < 0)) // Clip top
        return;

    int8_t i = 0;
    for( i = 0 ; i < 6 ; i++ ){
        uint8_t line;
        if( i == 5 )
            line = 0x0;
        else
           line = font[(c*5)+i];
        int8_t j = 0;
        for( j = 0; j < 8 ; j++ ){
            if( line & 0x1 ){
                if( size == 1 ) // default size
                    gfx_drawPixel( x+i, y+j, color );
                else { // big size
                    gfx_fillRect( x+(i*size), y+(j*size), size, size, color );
                }
            } else if( bg != color ){
                if( size == 1 ) // default size
                    gfx_drawPixel( x+i, y+j, bg );
                else { // big size
                    gfx_fillRect( x+i*size, y+j*size, size, size, bg );
                }
            }
            line >>= 1;
        }
    }
}

void gfx_write(uint8_t ch)
{
    if( ch == '\n' ){
        cursor_y += textsize*8;
        cursor_x = 0;
    }else if( ch == '\r' ){
        // skip em
    }else{
        gfx_drawChar(cursor_x, cursor_y, ch, textcolor, textbgcolor, textsize);
        cursor_x += textsize*6;
        if( wrap && (cursor_x > (_width - textsize*6)) ){
            cursor_y += textsize*8;
            cursor_x = 0;
        }
    }
}

void gfx_print( const char* s ){
    
    unsigned int len = strlen( s );
    unsigned int i = 0; 
    for( i = 0 ; i < len ; i++ ){
        gfx_write( s[i] );
    }
}

void gfx_println( const char* s ){ 
    gfx_print( s ); 
    gfx_write( '\n' );
}

//--- end of file ---//


