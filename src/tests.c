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
void ShowDisplay (unsigned char *);
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
    int height;
    int width;
    int starty;
    int startx;

} WIN_st;
    
void draw_win(WINDOW *);
void draw_box(WIN_st *);
void draw_box_tittle (WIN_st *, char *);

void * KeyboardInput (void * arg);
    

// Module Functions ------------------------------------------------------------
sw_actions_t action = do_nothing;
int main_loop = 1;

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string);

int main(int argc, char *argv[])
{
    WIN_st ggram_win;
    WIN_st help_win;
    
    initscr();			/* Start curses mode 		*/
    start_color();			/* Start color 			*/

    init_pair(1, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(1));

    ggram_win.starty = 0;
    ggram_win.startx = 0;
    ggram_win.height = 30;
    ggram_win.width = 130;
    
    draw_box(&ggram_win);

    help_win.starty = 30;
    help_win.startx = 0;
    help_win.height = 3;
    help_win.width = 130;
    
    draw_box(&help_win);    
    
    print_in_middle(stdscr, LINES / 2, 0, 0, "Viola !!! In color ...");
    refresh();

    draw_box_tittle(&help_win, "lines in this sheet");
    
    attroff(COLOR_PAIR(1));
    getch();
    endwin();
}

void draw_box (WIN_st * pw)
{
    int x1 = pw->startx;
    int y1 = pw->starty;
    int x2 = pw->startx + pw->width;
    int y2 = pw->starty + pw->height;
    int dx = pw->width;
    int dy = pw->height;
    
    mvhline(y1, x1, '-', dx);
    mvhline(y2, x1, '-', dx);
    mvvline(y1, x1, '|', dy);
    mvvline(y1, x2, '|', dy);

    mvwprintw(stdscr, 0, 30, "y1: %d x1: %d y2: %d x2: %d", y1, x1, y2, x2);
    refresh();
    
    mvaddch(y1, x1, '+');
    mvaddch(y1, x2, '+');
    mvaddch(y2, x1, '+');
    mvaddch(y2, x2, '+');
    
}


void draw_box_tittle (WIN_st * pw, char * tittle)
{
    int x1 = pw->startx;
    int y1 = pw->starty;
    int x2 = pw->startx + pw->width;
    int y2 = pw->starty + pw->height;
    int dx = pw->width;
    int dy = pw->height;

    int tittle_len = strlen(tittle);

    if (dx >= tittle_len + 2)
    {
        int tittle_x = (dx - tittle_len) / 2;
        mvwprintw(stdscr, y1, tittle_x, "%s", tittle);
        refresh();
    }       
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

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string)
{	int length, x, y;
	float temp;

	if(win == NULL)
		win = stdscr;
	getyx(win, y, x);
	if(startx != 0)
		x = startx;
	if(starty != 0)
		y = starty;
	if(width == 0)
		width = 80;

	length = strlen(string);
	temp = (width - length)/ 2;
	x = startx + (int)temp;
	mvwprintw(win, y, x, "%s", string);
        mvwprintw(win, y+1, x, "lines: %d columns: %d", LINES, COLS);
	refresh();
}

// int main (int argc, char *argv[])
// {
//     // pthread_t p1;
//     // int rc;

//     // rc = pthread_create(&p1, NULL, KeyboardInput, (void *)rc);
//     // if (rc){
//     //     printf("ERROR; return code from pthread_create() is %d\n", rc);
//     //     exit(-1);
//     // }

//     // gfx_init(DISPLAYWIDTH, DISPLAYHEIGHT);    
//     // MainMenu_Init ();
//     // do {

//     //     MainMenu_Update(action);
            
//     // } while (main_loop);

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
    int loop = 1;
    char sel = 0;

    while(loop)
    {
        action = do_nothing;
        printf("u -> up  d -> dwn  s -> select  b -> back  q -> quit\n");
        printf("action? ");
        scanf(" %c", &sel);

        switch (sel)
        {
        case 'u':
            action = selection_up;
            printf("selection up\n");
            break;

        case 'd':
            action = selection_dwn;
            printf("selection dwn\n");            
            break;

        case 's':
            action = selection_enter;
            printf("selection enter\n");            
            break;

        case 'b':
            action = selection_back;
            printf("selection back\n");            
            break;

        case 'q':
            loop = 0;
            printf("exiting program\n");
            break;

        default:
            printf("error!\n");
            break;
        }
        sel = 0;
    }

    main_loop = 0;
    pthread_exit(NULL);
}


unsigned int disp_cnt = 0;
void ShowDisplay (unsigned char * mem)
{
    //first square side
    printf("\n+");
    for (int i = 0; i < DISPLAYWIDTH; i++)
        printf("-");
    printf("+\n");

    //64 lines grouped by 8
    for (int z = 0; z < DISPLAYWIDTH * 8; z+=DISPLAYWIDTH)
    {
        for (int j = 0; j < DISPLAYHEIGHT / 8; j++)
        {
            printf("|");
            unsigned char c = 1;
            c <<= j;        
            for (int i = 0; i < DISPLAYWIDTH; i++)
            {
                if (mem[z + i] & c)
                    printf("#");
                else
                    printf(" ");
            }
            printf("|\n");        
        }
    }
    
    //last square side
    printf("+");
    for (int i = 0; i < DISPLAYWIDTH; i++)
        printf("-");
    printf("+\n");

    disp_cnt++;
    printf("display counter: %d\n", disp_cnt);
}


void display_clear(void)
{
    memset( mem_buffer, 0x00, sizeof(mem_buffer) );
}

void display_update(void)
{
    ShowDisplay(mem_buffer);
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


